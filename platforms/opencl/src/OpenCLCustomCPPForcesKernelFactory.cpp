/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include <exception>

#include "OpenCLCustomCPPForcesKernelFactory.h"
#include "OpenCLCustomCPPForcesKernels.h"
#include "OpenCLParallelCustomCPPForcesKernels.h"
#include "CommonCustomCPPForcesKernels.h"
#include "openmm/opencl/OpenCLContext.h"
#include "openmm/internal/windowsExport.h"
#include "openmm/internal/ContextImpl.h"
#include "openmm/OpenMMException.h"

using namespace CustomCPPForces;
using namespace OpenMM;

extern "C" OPENMM_EXPORT void registerPlatforms() {
}

extern "C" OPENMM_EXPORT void registerKernelFactories() {
    try {
        Platform& platform = Platform::getPlatformByName("OpenCL");
        OpenCLCustomCPPForcesKernelFactory* factory = new OpenCLCustomCPPForcesKernelFactory();
        platform.registerKernelFactory(CalcSlicedNonbondedForceKernel::Name(), factory);
        platform.registerKernelFactory(CalcExtendedCustomCVForceKernel::Name(), factory);
    }
    catch (std::exception ex) {
        // Ignore
    }
}

extern "C" OPENMM_EXPORT void registerCustomCPPForcesOpenCLKernelFactories() {
    try {
        Platform::getPlatformByName("OpenCL");
    }
    catch (...) {
        Platform::registerPlatform(new OpenCLPlatform());
    }
    registerKernelFactories();
}

KernelImpl* OpenCLCustomCPPForcesKernelFactory::createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const {
    OpenCLPlatform::PlatformData& data = *static_cast<OpenCLPlatform::PlatformData*>(context.getPlatformData());
    if (data.contexts.size() > 1) {
        if (name == CalcSlicedNonbondedForceKernel::Name())
            return new OpenCLParallelCalcSlicedNonbondedForceKernel(name, platform, data, context.getSystem());
        throw OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
    }
    OpenCLContext& cl = *data.contexts[0];
    if (name == CalcSlicedNonbondedForceKernel::Name())
        return new OpenCLCalcSlicedNonbondedForceKernel(name, platform, cl, context.getSystem());
    if (name == CalcExtendedCustomCVForceKernel::Name())
        return new OpenCLCalcExtendedCustomCVForceKernel(name, platform, cl);
    throw OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
}
