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
#include <windows.h>
#include <sstream>
#else
#include <dlfcn.h>
#include <dirent.h>
#include <cstdlib>
#endif

#include "SlicedNonbondedForce.h"
#include "SlicedNonbondedForceProxy.h"
#include "ConcertedRMSDForce.h"
#include "ConcertedRMSDForceProxy.h"
#include "openmm/serialization/SerializationProxy.h"

#if defined(WIN32)
    #include <windows.h>
    extern "C" OPENMM_EXPORT_OPENMM_LAB void registerCustomCPPForcesSerializationProxies();
    BOOL WINAPI DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
        if (ul_reason_for_call == DLL_PROCESS_ATTACH)
            registerCustomCPPForcesSerializationProxies();
        return TRUE;
    }
#else
    extern "C" void __attribute__((constructor)) registerCustomCPPForcesSerializationProxies();
#endif

using namespace CustomCPPForces;
using namespace OpenMM;

extern "C" OPENMM_EXPORT_OPENMM_LAB void registerCustomCPPForcesSerializationProxies() {
    SerializationProxy::registerProxy(typeid(SlicedNonbondedForce), new SlicedNonbondedForceProxy());
    SerializationProxy::registerProxy(typeid(ConcertedRMSDForce), new ConcertedRMSDForceProxy());
}
