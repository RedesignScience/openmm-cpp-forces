#ifndef OPENMM_CUDAOPENMM_LABKERNELFACTORY_H_
#define OPENMM_CUDAOPENMM_LABKERNELFACTORY_H_

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

using namespace OpenMM;

namespace CustomCPPForces {

/**
 * This KernelFactory creates kernels for the CUDA implementation of the CustomCPPForces plugin.
 */

class CudaCustomCPPForcesKernelFactory : public KernelFactory {
public:
    KernelImpl* createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const;
};

} // namespace OpenMM

#endif /*OPENMM_CUDAOPENMM_LABKERNELFACTORY_H_*/
