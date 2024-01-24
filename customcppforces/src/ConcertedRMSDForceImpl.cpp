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

void ConcertedRMSDForceImpl::initialize(ContextImpl& context) {
    CustomCPPForceImpl::initialize(context);

    // Check for errors in the specification of particles.
    const System& system = context.getSystem();
    int systemSize = system.getNumParticles();
    if (owner.getReferencePositions().size() != systemSize)
        throw OpenMMException(
            "RMSDForce: Number of reference positions does not equal number of particles in the System"
        );

    groups.resize(owner.getNumGroups());
    for (int i = 0; i < owner.getNumGroups(); i++)
        groups[i] = owner.getGroup(i);

    set<int> distinctParticles;
    for (int i : groups[0]) {
        if (i < 0 || i >= systemSize) {
            stringstream msg;
            msg << "ConcertedRMSDForce: Illegal particle index for RMSD: ";
            msg << i;
            throw OpenMMException(msg.str());
        }
        if (distinctParticles.find(i) != distinctParticles.end()) {
            stringstream msg;
            msg << "ConcertedRMSDForce: Duplicated particle index for RMSD: ";
            msg << i;
            throw OpenMMException(msg.str());
        }
        distinctParticles.insert(i);
    }

    referencePos = owner.getReferencePositions();
    Vec3 center(0.0, 0.0, 0.0);
    for (int i : groups[0])
        center += referencePos[i];
    center /= groups[0].size();
    for (Vec3& p : referencePos)
        p -= center;
}

double ConcertedRMSDForceImpl::computeForce(ContextImpl& context, const vector<Vec3>& positions, vector<Vec3>& forces) {
    // Compute the RMSD and its gradient using the algorithm described in Coutsias et al,
    // "Using quaternions to calculate RMSD" (doi: 10.1002/jcc.20110).  First subtract
    // the centroid from the atom positions.  The reference positions have already been centered.

    Vec3 center(0.0, 0.0, 0.0);
    for (int i : groups[0])
        center += positions[i];
    center /= groups[0].size();
    vector<Vec3> centeredPos(groups[0].size());
    for (int i = 0; i < groups[0].size(); i++)
        centeredPos[i] = positions[groups[0][i]] - center;

    // Compute the correlation matrix.

    double R[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < groups[0].size(); k++) {
                int index = groups[0][k];
                R[i][j] += centeredPos[k][i]*referencePos[index][j];
            }

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
    for (int i = 0; i < groups[0].size(); i++) {
        int index = groups[0][i];
        sum += centeredPos[i].dot(centeredPos[i]) + referencePos[index].dot(referencePos[index]);
    }
    double msd = (sum - 2*values[3])/groups[0].size();
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

    for (int i = 0; i < groups[0].size(); i++) {
        const Vec3& p = referencePos[groups[0][i]];
        Vec3 rotatedRef(U[0][0]*p[0] + U[1][0]*p[1] + U[2][0]*p[2],
                        U[0][1]*p[0] + U[1][1]*p[1] + U[2][1]*p[2],
                        U[0][2]*p[0] + U[1][2]*p[1] + U[2][2]*p[2]);
        forces[groups[0][i]] = -(centeredPos[i] - rotatedRef) / (rmsd*groups[0].size());
    }
    return rmsd;
}

void ConcertedRMSDForceImpl::updateParametersInContext(ContextImpl& context) {
    if (referencePos.size() != owner.getReferencePositions().size())
        throw OpenMMException("updateParametersInContext: The number of reference positions has changed");
    groups.resize(owner.getNumGroups());
    for (int i = 0; i < owner.getNumGroups(); i++)
        groups[i] = owner.getGroup(i);
    if (groups[0].size() == 0)
        for (int i = 0; i < referencePos.size(); i++)
            groups[0].push_back(i);
    referencePos = owner.getReferencePositions();
    Vec3 center(0.0, 0.0, 0.0);
    for (int i : groups[0])
        center += referencePos[i];
    center /= groups[0].size();
    for (Vec3& p : referencePos)
        p -= center;
    context.systemChanged();
}
