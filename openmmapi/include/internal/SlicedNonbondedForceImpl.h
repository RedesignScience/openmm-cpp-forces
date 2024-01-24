#ifndef OPENMM_SLICEDNONBONDEDFORCEIMPL_H_
#define OPENMM_SLICEDNONBONDEDFORCEIMPL_H_

/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "SlicedNonbondedForce.h"
#include "openmm/internal/NonbondedForceImpl.h"
#include "openmm/Kernel.h"
#include <utility>
#include <set>
#include <string>

using namespace OpenMM;

namespace CustomCPPForces {

/**
 * This is the internal implementation of SlicedNonbondedForce.
 */

class OPENMM_EXPORT_OPENMM_LAB SlicedNonbondedForceImpl : public NonbondedForceImpl {
public:
    SlicedNonbondedForceImpl(const SlicedNonbondedForce& owner);
    ~SlicedNonbondedForceImpl();
    void initialize(ContextImpl& context);
    const SlicedNonbondedForce& getOwner() const {
        return owner;
    }
    double calcForcesAndEnergy(ContextImpl& context, bool includeForces, bool includeEnergy, int groups);
    std::vector<std::string> getKernelNames();
    void updateParametersInContext(ContextImpl& context);
    void getPMEParameters(double& alpha, int& nx, int& ny, int& nz) const;
    void getLJPMEParameters(double& alpha, int& nx, int& ny, int& nz) const;
    static double calcDispersionCorrection(const System& system, const SlicedNonbondedForce& force);
    static vector<double> calcDispersionCorrections(const System& system, const SlicedNonbondedForce& force);
private:
    static double evalIntegral(double r, double rs, double rc, double sigma);
    const SlicedNonbondedForce& owner;
    Kernel kernel;
};

} // namespace CustomCPPForces

#endif /*OPENMM_SLICEDNONBONDEDFORCEIMPL_H_*/
