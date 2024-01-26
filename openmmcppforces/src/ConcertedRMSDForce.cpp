/* -------------------------------------------------------------------------- *
 *                              OpenMM CPP Forces                             *
 *                              =================                             *
 *                                                                            *
 *  A plugin for distributing platform-agnostic OpenMM Forces                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/openmm-cpp-forces                               *
 * -------------------------------------------------------------------------- */

#include "ConcertedRMSDForce.h"
#include "internal/ConcertedRMSDForceImpl.h"

#include "openmm/internal/AssertionUtilities.h"

using namespace OpenMMCPPForces;
using namespace OpenMM;
using namespace std;

ConcertedRMSDForce::ConcertedRMSDForce(const vector<Vec3>& referencePositions) :
        referencePositions(referencePositions) {
}

void ConcertedRMSDForce::setReferencePositions(const std::vector<Vec3>& positions) {
    referencePositions = positions;
}

int ConcertedRMSDForce::addGroup(const vector<int>& particles) {
    groups.push_back(particles);
    return groups.size()-1;
}

int ConcertedRMSDForce::getNumGroups() const {
    return groups.size();
}

const vector<int>& ConcertedRMSDForce::getGroup(int index) const {
    ASSERT_VALID_INDEX(index, groups);
    return groups[index];
}

void ConcertedRMSDForce::setGroup(int index, const std::vector<int>& particles) {
    ASSERT_VALID_INDEX(index, groups);
    groups[index] = particles;
}

void ConcertedRMSDForce::updateParametersInContext(Context& context) {
    dynamic_cast<ConcertedRMSDForceImpl&>(getImplInContext(context)).updateParametersInContext(getContextImpl(context));
}

ForceImpl* ConcertedRMSDForce::createImpl() const {
    return new ConcertedRMSDForceImpl(*this);
}
