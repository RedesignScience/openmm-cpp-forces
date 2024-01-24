#ifndef OPENMM_OPENCLOPENMM_LABKERNELFACTORY_H_
#define OPENMM_OPENCLOPENMM_LABKERNELFACTORY_H_

/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "openmm/KernelFactory.h"

namespace CustomCPPForces {

/**
 * This KernelFactory creates kernels for the OpenCL implementation of the CustomCPPForces plugin.
 */

class OpenCLCustomCPPForcesKernelFactory : public OpenMM::KernelFactory {
public:
    OpenMM::KernelImpl* createKernelImpl(std::string name, const OpenMM::Platform& platform, OpenMM::ContextImpl& context) const;
};

} // namespace CustomCPPForces

#endif /*OPENMM_OPENCLOPENMM_LABKERNELFACTORY_H_*/
