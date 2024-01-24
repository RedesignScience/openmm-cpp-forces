#ifndef OPENMM_REFERENCEOPENMM_LABKERNELFACTORY_H_
#define OPENMM_REFERENCEOPENMM_LABKERNELFACTORY_H_

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
#include <string.h>

using namespace OpenMM;

namespace CustomCPPForces {

/**
 * This KernelFactory creates kernels for the reference implementation of the CustomCPPForces plugin.
 */

class ReferenceCustomCPPForcesKernelFactory : public KernelFactory {
public:
    KernelImpl* createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const;
};

} // namespace CustomCPPForces

#endif /*OPENMM_REFERENCEOPENMM_LABKERNELFACTORY_H_*/
