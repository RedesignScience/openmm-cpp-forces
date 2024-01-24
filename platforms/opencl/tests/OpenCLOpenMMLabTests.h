/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#ifdef WIN32
  #define _USE_MATH_DEFINES // Needed to get M_PI
#endif
#include "openmm/opencl/OpenCLPlatform.h"
#include <map>

extern "C" OPENMM_EXPORT void registerCustomCPPForcesOpenCLKernelFactories();

OpenMM::OpenCLPlatform platform;
std::map<std::string, std::string> properties;

void initializeTests(int argc, char* argv[]) {
    registerCustomCPPForcesOpenCLKernelFactories();
    platform = dynamic_cast<OpenMM::OpenCLPlatform&>(OpenMM::Platform::getPlatformByName("OpenCL"));
    if (argc > 1)
        properties["Precision"] = std::string(argv[1]);
    if (argc > 2)
        properties["DeviceIndex"] = std::string(argv[2]);
    for (auto& p : properties)
        platform.setPropertyDefaultValue(p.first, p.second);
}
