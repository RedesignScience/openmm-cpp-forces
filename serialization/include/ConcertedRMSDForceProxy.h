#ifndef OPENMM_CONCERTEDRMSDFORCE_PROXY_H_
#define OPENMM_CONCERTEDRMSDFORCE_PROXY_H_

/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "internal/windowsExportCustomCPPForces.h"

#include "openmm/serialization/SerializationProxy.h"

using namespace OpenMM;

namespace CustomCPPForces {

/**
 * This is a proxy for serializing ConcertedRMSDForce objects.
 */

class OPENMM_EXPORT_OPENMM_LAB ConcertedRMSDForceProxy : public SerializationProxy {
public:
    ConcertedRMSDForceProxy();
    void serialize(const void* object, SerializationNode& node) const;
    void* deserialize(const SerializationNode& node) const;
};

} // namespace CustomCPPForces

#endif /*OPENMM_CONCERTEDRMSDFORCE_PROXY_H_*/
