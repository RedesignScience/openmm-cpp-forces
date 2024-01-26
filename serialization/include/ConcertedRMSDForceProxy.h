#ifndef OPENMM_CONCERTEDRMSDFORCE_PROXY_H_
#define OPENMM_CONCERTEDRMSDFORCE_PROXY_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMM CPP Forces                             *
 *                              =================                             *
 *                                                                            *
 *  A plugin for distributing platform-agnostic OpenMM Forces                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/openmm-cpp-forces                               *
 * -------------------------------------------------------------------------- */

#include "internal/windowsExportOpenMMCPPForces.h"

#include "openmm/serialization/SerializationProxy.h"

using namespace OpenMM;

namespace OpenMMCPPForces {

/**
 * This is a proxy for serializing ConcertedRMSDForce objects.
 */

class CUSTOM_CPP_FORCES_EXPORT ConcertedRMSDForceProxy : public SerializationProxy {
public:
    ConcertedRMSDForceProxy();
    void serialize(const void* object, SerializationNode& node) const;
    void* deserialize(const SerializationNode& node) const;
};

} // namespace OpenMMCPPForces

#endif /*OPENMM_CONCERTEDRMSDFORCE_PROXY_H_*/
