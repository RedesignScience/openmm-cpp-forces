
import typing as t

import openmmcppforces as mmcpp
import numpy as np
import openmm as mm
from openmm import unit


def value(x):
    return x/x.unit if unit.is_quantity(x) else x


def ASSERT(cond):
    assert cond


def ASSERT_EQUAL_TOL(expected, found, tol):
    exp = value(expected)
    assert abs(exp - value(found))/max(abs(exp), 1.0) <= tol


def ASSERT_EQUAL_VEC(expected, found, tol):
    ASSERT_EQUAL_TOL(expected.x, found.x, tol)
    ASSERT_EQUAL_TOL(expected.y, found.y, tol)
    ASSERT_EQUAL_TOL(expected.z, found.z, tol)


def assert_forces_and_energy(context, tol):
    state0 = context.getState(getForces=True, getEnergy=True, groups={0})
    state1 = context.getState(getForces=True, getEnergy=True, groups={1})
    for force0, force1 in zip(state0.getForces(), state1.getForces()):
        ASSERT_EQUAL_VEC(force0, force1, tol)
    ASSERT_EQUAL_TOL(state0.getPotentialEnergy(), state1.getPotentialEnergy(), tol)


def estimate_rmsd(
    positions: t.Sequence[mm.Vec3],
    referencePos: t.Sequence[mm.Vec3],
    particles: t.Sequence[int],
) -> float:
    # Estimate the RMSD.  For simplicity we omit the orientation alignment, but they
    # should already be almost perfectly aligned.

    center1 = mm.Vec3(0, 0, 0)
    center2 = mm.Vec3(0, 0, 0)
    for i in particles:
        center1 += referencePos[i]
        center2 += positions[i]
    center1 /= len(particles)
    center2 /= len(particles)
    estimate = 0.0
    for i in particles:
        delta = (referencePos[i] - center1) - (positions[i] - center2)
        estimate += delta.x * delta.x + delta.y * delta.y + delta.z * delta.z
    return np.sqrt(estimate/len(particles))


def test_single_group_rmsd():
    numParticles = 20
    system = mm.System()
    random = np.random.default_rng(0)

    def random_vec3():
        return mm.Vec3(random.random(), random.random(), random.random())

    referencePos = []
    positions = []
    particles = []
    for i in range(numParticles):
        system.addParticle(1.0)
        refpos = 10 * random_vec3()
        referencePos.append(refpos)
        positions.append(refpos + 0.2 * random_vec3())
        if i % 5:
            particles.append(i)
    force = mmcpp.CompositeRMSDForce(referencePos)

    force.addGroup(particles)
    system.addForce(force)
    integrator = mm.VerletIntegrator(0.001)
    platform = mm.Platform.getPlatformByName("Reference")
    context = mm.Context(system, integrator, platform)
    context.setPositions(positions)
    estimate = estimate_rmsd(positions, referencePos, particles)

    # Have the force compute the RMSD.  It should be very slightly less than
    # what we calculated above (since that omitted the rotation).

    state1 = context.getState(getEnergy=True)
    rmsd = state1.getPotentialEnergy().value_in_unit(unit.kilojoule_per_mole)
    ASSERT(rmsd <= estimate)
    ASSERT(rmsd > 0.9*estimate)

    # Translate and rotate all the particles.  This should have no effect on the RMSD.

    transformedPos = []
    cs = np.cos(1.1)
    sn = np.sin(1.1)
    for i in range(numParticles):
        p = positions[i]
        transformedPos.append(
            mm.Vec3(cs*p[0] + sn*p[1] + 0.1, -sn*p[0] + cs*p[1] - 11.3, p[2] + 1.5)
        )
    context.setPositions(transformedPos)
    state1 = context.getState(getEnergy=True, getForces=True)
    ASSERT_EQUAL_TOL(rmsd, state1.getPotentialEnergy(), 1e-4)

    # Take a small step in the direction of the energy gradient and see whether the
    # potential energy changes by the expected amount.

    forces = state1.getForces().value_in_unit(unit.kilojoule_per_mole/unit.nanometer)
    norm = 0.0
    for f in forces:
        norm += f.x * f.x + f.y * f.y + f.z * f.z
    norm = np.sqrt(norm)
    stepSize = 0.1
    step = 0.5 * stepSize / norm
    positions2 = []
    positions3 = []
    for i in range(len(positions)):
        p = transformedPos[i]
        f = forces[i]
        positions2.append(
            mm.Vec3(p[0] - f[0] * step, p[1] - f[1] * step, p[2] - f[2] * step)
        )
        positions3.append(
            mm.Vec3(p[0] + f[0] * step, p[1] + f[1] * step, p[2] + f[2] * step)
        )
    context.setPositions(positions2)
    state2 = context.getState(getEnergy=True)
    rmsd2 = state2.getPotentialEnergy()
    context.setPositions(positions3)
    state3 = context.getState(getEnergy=True)
    rmsd3 = state3.getPotentialEnergy()
    ASSERT_EQUAL_TOL(norm, (rmsd2 - rmsd3)/stepSize, 1e-3)

    # Check that updateParametersInContext() works correctly.

    context.setPositions(transformedPos)
    force.setReferencePositions(transformedPos)
    force.updateParametersInContext(context)
    ASSERT_EQUAL_TOL(0.0, context.getState(getEnergy=True).getPotentialEnergy(), 1e-2)
    context.setPositions(referencePos)
    ASSERT_EQUAL_TOL(rmsd, context.getState(getEnergy=True).getPotentialEnergy(), 1e-4)

    # Verify that giving an empty list of particles is interpreted to mean all
    # particles.

    allParticles = list(range(numParticles))
    estimate = estimate_rmsd(positions, referencePos, allParticles)
    force.setGroup(0, allParticles)
    force.setReferencePositions(referencePos)
    force.updateParametersInContext(context)
    context.setPositions(positions);
    rmsd1 = context.getState(getEnergy=True).getPotentialEnergy()
    force.setGroup(0, [])
    force.updateParametersInContext(context)
    rmsd2 = context.getState(getEnergy=True).getPotentialEnergy()
    ASSERT_EQUAL_TOL(rmsd1, rmsd2, 1e-4)
    rmsd1 = rmsd1 / unit.kilojoule_per_mole
    ASSERT(rmsd1 <= estimate)
    ASSERT(rmsd1 > 0.9*estimate)
