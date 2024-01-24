#ifndef OPENMM_CONCERTEDRMSDFORCEIMPL_H_
#define OPENMM_CONCERTEDRMSDFORCEIMPL_H_

/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "ConcertedRMSDForce.h"

#include "openmm/internal/CustomCPPForceImpl.h"
#include "openmm/internal/ContextImpl.h"
#include <vector>

using namespace OpenMM;
using namespace std;

namespace CustomCPPForces {

/**
 * This is the internal implementation of ConcertedRMSDForce.
 */

class ConcertedRMSDForceImpl : public CustomCPPForceImpl {
public:
    ConcertedRMSDForceImpl(const ConcertedRMSDForce& owner) : CustomCPPForceImpl(owner), owner(owner) {}
    void initialize(ContextImpl& context);
    double computeForce(ContextImpl& context, const vector<Vec3>& positions, vector<Vec3>& forces);
    const ConcertedRMSDForce& getOwner() const {
        return owner;
    }
    void updateParametersInContext(ContextImpl& context);
private:
    const ConcertedRMSDForce& owner;
    vector<vector<int>> groups;
    vector<Vec3> referencePos;
};

} // namespace CustomCPPForces

#endif /*OPENMM_CONCERTEDRMSDFORCEIMPL_H_*/
