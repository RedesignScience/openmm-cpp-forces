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
#include "openmm/reference/ReferencePlatform.h"
#include <map>
#include <string>

extern "C" OPENMM_EXPORT void registerCustomCPPForcesReferenceKernelFactories();

OpenMM::ReferencePlatform platform;
std::map<std::string, std::string> properties;

void initializeTests(int argc, char* argv[]) {
    registerCustomCPPForcesReferenceKernelFactories();
    platform = dynamic_cast<OpenMM::ReferencePlatform&>(OpenMM::Platform::getPlatformByName("Reference"));
}