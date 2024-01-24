/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "internal/ConcertedRMSDForceImpl.h"

#include "openmm/internal/CustomCPPForceImpl.h"
#include "openmm/internal/ContextImpl.h"
#include "jama/jama_eig.h"
#include <vector>
#include <set>
#include <sstream>

using namespace CustomCPPForces;
using namespace OpenMM;
using namespace std;


void ConcertedRMSDForceImpl::updateParameters(int systemSize) {
    // Check for errors in the specification of particles.
    if (owner.getReferencePositions().size() != systemSize)
        throw OpenMMException(
            "ConcertedRMSDForce: Number of reference positions does not equal number of particles in the System"
        );

    int numGroups = owner.getNumGroups();
    if (numGroups == 0)
        throw OpenMMException("ConcertedRMSDForce: No particle groups have been specified");

    groups.resize(numGroups);
    for (int i = 0; i < numGroups; i++) {
        groups[i] = owner.getGroup(i);
        if (groups[i].size() == 0) {
            groups[i].resize(systemSize);
            for (int j = 0; j < systemSize; j++)
                groups[i][j] = j;
        }
    }

    set<int> distinctParticles;
    for (int k = 0; k < numGroups; k++)
        for (int i : groups[k]) {
            if (i < 0 || i >= systemSize) {
                stringstream msg;
                msg << "ConcertedRMSDForce: Illegal particle index " << i << " in group " << k;
                throw OpenMMException(msg.str());
            }
            if (distinctParticles.find(i) != distinctParticles.end()) {
                stringstream msg;
                msg << "ConcertedRMSDForce: Duplicated particle index " << i << " in group " << k;
                throw OpenMMException(msg.str());
            }
            distinctParticles.insert(i);
        }

    referencePos = owner.getReferencePositions();
    for (auto& group : groups) {
        Vec3 center(0.0, 0.0, 0.0);
        for (int i : group)
            center += referencePos[i];
        center /= group.size();
        for (int i : group)
            referencePos[i] -= center;
    }
}

void ConcertedRMSDForceImpl::initialize(ContextImpl& context) {
    CustomCPPForceImpl::initialize(context);
    updateParameters(context.getSystem().getNumParticles());
}

double ConcertedRMSDForceImpl::computeForce(ContextImpl& context, const vector<Vec3>& positions, vector<Vec3>& forces) {
    // Compute the RMSD and its gradient using the algorithm described in Coutsias et al,
    // "Using quaternions to calculate RMSD" (doi: 10.1002/jcc.20110).  First subtract
    // the centroid from the atom positions.  The reference positions have already been centered.

    vector<Vec3> centeredPos(referencePos.size());
    int numParticles = 0;
    for (auto& group : groups) {
        Vec3 center(0.0, 0.0, 0.0);
        for (int i : group)
            center += positions[i];
        center /= group.size();
        numParticles += group.size();
        for (int i : group)
            centeredPos[i] = positions[i] - center;
    }

    // Compute the correlation matrix.

    double R[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (auto& group : groups)
                for (int index : group)
                    R[i][j] += centeredPos[index][i]*referencePos[index][j];

    // Compute the F matrix.

    Array2D<double> F(4, 4);
    F[0][0] =  R[0][0] + R[1][1] + R[2][2];
    F[1][0] =  R[1][2] - R[2][1];
    F[2][0] =  R[2][0] - R[0][2];
    F[3][0] =  R[0][1] - R[1][0];

    F[0][1] =  R[1][2] - R[2][1];
    F[1][1] =  R[0][0] - R[1][1] - R[2][2];
    F[2][1] =  R[0][1] + R[1][0];
    F[3][1] =  R[0][2] + R[2][0];

    F[0][2] =  R[2][0] - R[0][2];
    F[1][2] =  R[0][1] + R[1][0];
    F[2][2] = -R[0][0] + R[1][1] - R[2][2];
    F[3][2] =  R[1][2] + R[2][1];

    F[0][3] =  R[0][1] - R[1][0];
    F[1][3] =  R[0][2] + R[2][0];
    F[2][3] =  R[1][2] + R[2][1];
    F[3][3] = -R[0][0] - R[1][1] + R[2][2];

    // Find the maximum eigenvalue and eigenvector.

    JAMA::Eigenvalue<double> eigen(F);
    Array1D<double> values;
    eigen.getRealEigenvalues(values);
    Array2D<double> vectors;
    eigen.getV(vectors);

    // Compute the RMSD.

    double sum = 0.0;
    for (auto& group : groups)
        for (int index : group)
            sum += centeredPos[index].dot(centeredPos[index]) + referencePos[index].dot(referencePos[index]);

    double msd = (sum - 2*values[3])/numParticles;
    if (msd < 1e-20) {
        // The particles are perfectly aligned, so all the forces should be zero.
        // Numerical error can lead to NaNs, so just return 0 now.
        return 0.0;
    }
    double rmsd = sqrt(msd);

    // Compute the rotation matrix.

    double q[] = {vectors[0][3], vectors[1][3], vectors[2][3], vectors[3][3]};
    double q00 = q[0]*q[0], q01 = q[0]*q[1], q02 = q[0]*q[2], q03 = q[0]*q[3];
    double q11 = q[1]*q[1], q12 = q[1]*q[2], q13 = q[1]*q[3];
    double q22 = q[2]*q[2], q23 = q[2]*q[3];
    double q33 = q[3]*q[3];
    double U[3][3] = {{q00+q11-q22-q33, 2*(q12-q03), 2*(q13+q02)},
                      {2*(q12+q03), q00-q11+q22-q33, 2*(q23-q01)},
                      {2*(q13-q02), 2*(q23+q01), q00-q11-q22+q33}};

    // Rotate the reference positions and compute the forces.

    for (auto& group : groups)
        for (int index : group) {
            const Vec3& p = referencePos[index];
            Vec3 rotatedRef(U[0][0]*p[0] + U[1][0]*p[1] + U[2][0]*p[2],
                            U[0][1]*p[0] + U[1][1]*p[1] + U[2][1]*p[2],
                            U[0][2]*p[0] + U[1][2]*p[1] + U[2][2]*p[2]);
            forces[index] = -(centeredPos[index] - rotatedRef) / (rmsd*groups[0].size());
    }
    return rmsd;
}

void ConcertedRMSDForceImpl::updateParametersInContext(ContextImpl& context) {
    updateParameters(context.getSystem().getNumParticles());
    context.systemChanged();
}
