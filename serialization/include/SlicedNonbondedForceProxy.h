#ifndef OPENMM_SLICEDNONBONDEDFORCE_PROXY_H_
#define OPENMM_SLICEDNONBONDEDFORCE_PROXY_H_

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
 * This is a proxy for serializing SlicedNonbondedForce objects.
 */

class OPENMM_EXPORT_OPENMM_LAB SlicedNonbondedForceProxy : public SerializationProxy {
public:
    SlicedNonbondedForceProxy();
    void serialize(const void* object, SerializationNode& node) const;
    void* deserialize(const SerializationNode& node) const;
};

} // namespace OpenMM

#endif /*OPENMM_SLICEDNONBONDEDFORCE_PROXY_H_*/
