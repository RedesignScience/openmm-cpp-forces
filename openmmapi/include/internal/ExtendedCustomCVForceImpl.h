#ifndef CUSTOMCPPFORCES_EXTENDEDCUSTOMCVFORCEIMPL_H_
#define CUSTOMCPPFORCES_EXTENDEDCUSTOMCVFORCEIMPL_H_

/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "ExtendedCustomCVForce.h"

#include "openmm/internal/ForceImpl.h"
#include "openmm/Context.h"
#include "openmm/Kernel.h"
#include "openmm/System.h"
#include "openmm/VerletIntegrator.h"
#include <map>
#include <string>
#include <vector>

using namespace OpenMM;

namespace CustomCPPForces {

/**
 * This is the internal implementation of ExtendedCustomCVForce.
 */

class OPENMM_EXPORT_OPENMM_LAB ExtendedCustomCVForceImpl : public ForceImpl {
public:
    ExtendedCustomCVForceImpl(const ExtendedCustomCVForce& owner);
    ~ExtendedCustomCVForceImpl();
    void initialize(ContextImpl& context);
    const ExtendedCustomCVForce& getOwner() const {
        return owner;
    }
    void updateContextState(ContextImpl& context, bool& forcesInvalid) {
        // This force field doesn't update the state directly.
    }
    double calcForcesAndEnergy(ContextImpl& context, bool includeForces, bool includeEnergy, int groups);
    std::map<std::string, double> getDefaultParameters();
    std::vector<std::string> getKernelNames();
    std::vector<std::pair<int, int> > getBondedParticles() const;
    void getCollectiveVariableValues(ContextImpl& context, std::vector<double>& values);
    Context& getInnerContext();
    void updateParametersInContext(ContextImpl& context);
private:
    const ExtendedCustomCVForce& owner;
    Kernel kernel;
    System innerSystem;
    VerletIntegrator innerIntegrator;
    Context* innerContext;
    int forceGroup;  // for compatibility with OpenMM 8.0
};

} // namespace CustomCPPForces

#endif /*CUSTOMCPPFORCES_EXTENDEDCUSTOMCVFORCEIMPL_H_*/
