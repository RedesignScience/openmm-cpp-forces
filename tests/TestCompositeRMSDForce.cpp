/* -------------------------------------------------------------------------- *
 *                              OpenMM CPP Forces                             *
 *                              =================                             *
 *                                                                            *
 *  A plugin for distributing platform-agnostic OpenMM Forces                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/RedesignScience/openmm-cpp-forces                      *
 * -------------------------------------------------------------------------- */

#include "CompositeRMSDForce.h"

#include "openmm/internal/AssertionUtilities.h"
#include "openmm/reference/ReferencePlatform.h"
#include "openmm/Context.h"
#include "openmm/System.h"
#include "openmm/VerletIntegrator.h"
#include "sfmt/SFMT.h"
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace OpenMMCPPForces;
using namespace OpenMM;
using namespace std;

ReferencePlatform platform;

void initializeTests(int argc, char* argv[]) {
    platform = dynamic_cast<OpenMM::ReferencePlatform&>(OpenMM::Platform::getPlatformByName("Reference"));
}

double estimateRMSD(vector<Vec3>& positions, vector<Vec3>& referencePos, vector<int>& particles) {
    // Estimate the RMSD.  For simplicity we omit the orientation alignment, but they should
    // already be almost perfectly aligned.

    Vec3 center1, center2;
    for (int i : particles) {
        center1 += referencePos[i];
        center2 += positions[i];
    }
    center1 /= particles.size();
    center2 /= particles.size();
    double estimate = 0.0;
    for (int i : particles) {
        Vec3 delta = (referencePos[i]-center1) - (positions[i]-center2);
        estimate += delta.dot(delta);
    }
    return sqrt(estimate/particles.size());
}

void testRMSD() {
    const int numParticles = 20;
    System system;
    vector<Vec3> referencePos(numParticles);
    vector<Vec3> positions(numParticles);
    vector<int> particles;
    OpenMM_SFMT::SFMT sfmt;
    init_gen_rand(0, sfmt);
    for (int i = 0; i < numParticles; ++i) {
        system.addParticle(1.0);
        referencePos[i] = Vec3(genrand_real2(sfmt), genrand_real2(sfmt), genrand_real2(sfmt))*10;
        positions[i] = referencePos[i] + Vec3(genrand_real2(sfmt), genrand_real2(sfmt), genrand_real2(sfmt))*0.2;
        if (i%5 != 0)
            particles.push_back(i);
    }
    CompositeRMSDForce* force = new CompositeRMSDForce(referencePos);
    force->addGroup(particles);
    system.addForce(force);
    VerletIntegrator integrator(0.001);
    Context context(system, integrator, platform);
    context.setPositions(positions);
    double estimate = estimateRMSD(positions, referencePos, particles);

    // Have the force compute the RMSD.  It should be very slightly less than
    // what we calculated above (since that omitted the rotation).

    State state1 = context.getState(State::Energy);
    double rmsd = state1.getPotentialEnergy();
    ASSERT(rmsd <= estimate);
    ASSERT(rmsd > 0.9*estimate);

    // Translate and rotate all the particles.  This should have no effect on the RMSD.

    vector<Vec3> transformedPos(numParticles);
    double cs = cos(1.1), sn = sin(1.1);
    for (int i = 0; i < numParticles; i++) {
        Vec3 p = positions[i];
        transformedPos[i] = Vec3( cs*p[0] + sn*p[1] + 0.1,
                                 -sn*p[0] + cs*p[1] - 11.3,
                                  p[2] + 1.5);
    }
    context.setPositions(transformedPos);
    state1 = context.getState(State::Energy | State::Forces);
    ASSERT_EQUAL_TOL(rmsd, state1.getPotentialEnergy(), 1e-4);

    // Take a small step in the direction of the energy gradient and see whether the potential energy changes by the expected amount.

    const vector<Vec3>& forces = state1.getForces();
    double norm = 0.0;
    for (int i = 0; i < (int) forces.size(); ++i)
        norm += forces[i].dot(forces[i]);
    norm = std::sqrt(norm);
    const double stepSize = 0.1;
    double step = 0.5*stepSize/norm;
    vector<Vec3> positions2(numParticles), positions3(numParticles);
    for (int i = 0; i < (int) positions.size(); ++i) {
        Vec3 p = transformedPos[i];
        Vec3 f = forces[i];
        positions2[i] = Vec3(p[0]-f[0]*step, p[1]-f[1]*step, p[2]-f[2]*step);
        positions3[i] = Vec3(p[0]+f[0]*step, p[1]+f[1]*step, p[2]+f[2]*step);
    }
    context.setPositions(positions2);
    State state2 = context.getState(State::Energy);
    context.setPositions(positions3);
    State state3 = context.getState(State::Energy);
    ASSERT_EQUAL_TOL(norm, (state2.getPotentialEnergy()-state3.getPotentialEnergy())/stepSize, 1e-3);

    // Check that updateParametersInContext() works correctly.

    context.setPositions(transformedPos);
    force->setReferencePositions(transformedPos);
    force->updateParametersInContext(context);
    ASSERT_EQUAL_TOL(0.0, context.getState(State::Energy).getPotentialEnergy(), 1e-2);
    context.setPositions(referencePos);
    ASSERT_EQUAL_TOL(rmsd, context.getState(State::Energy).getPotentialEnergy(), 1e-4);

    // Verify that giving an empty list of particles is interpreted to mean all particles.

    vector<int> allParticles;
    for (int i = 0; i < numParticles; i++)
        allParticles.push_back(i);
    estimate = estimateRMSD(positions, referencePos, allParticles);
    force->setGroup(0, allParticles);
    force->setReferencePositions(referencePos);
    force->updateParametersInContext(context);
    context.setPositions(positions);
    double rmsd1 = context.getState(State::Energy).getPotentialEnergy();
    force->setGroup(0, vector<int>());
    force->updateParametersInContext(context);
    double rmsd2 = context.getState(State::Energy).getPotentialEnergy();
    ASSERT_EQUAL_TOL(rmsd1, rmsd2, 1e-4);
    ASSERT(rmsd1 <= estimate);
    ASSERT(rmsd1 > 0.9*estimate);
}

int main(int argc, char* argv[]) {
    try {
        initializeTests(argc, argv);
        testRMSD();
    }
    catch(const exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }
    cout << "Done" << endl;
    return 0;
}
