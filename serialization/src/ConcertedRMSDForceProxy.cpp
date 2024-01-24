/* -------------------------------------------------------------------------- *
 *                             OpenMM Laboratory                              *
 *                             =================                              *
 *                                                                            *
 * A plugin for testing low-level code implementation for OpenMM.             *
 *                                                                            *
 * Copyright (c) 2024 Charlles Abreu                                          *
 * https://github.com/craabreu/openmm-lab                                     *
 * -------------------------------------------------------------------------- */

#include "ConcertedRMSDForceProxy.h"
#include "ConcertedRMSDForce.h"

#include "openmm/serialization/SerializationNode.h"
#include "openmm/Force.h"
#include <sstream>

using namespace CustomCPPForces;
using namespace OpenMM;
using namespace std;

ConcertedRMSDForceProxy::ConcertedRMSDForceProxy() : SerializationProxy("ConcertedRMSDForce") {
}

void ConcertedRMSDForceProxy::serialize(const void* object, SerializationNode& node) const {
    node.setIntProperty("version", 0);
    const ConcertedRMSDForce& force = *reinterpret_cast<const ConcertedRMSDForce*>(object);
    node.setIntProperty("forceGroup", force.getForceGroup());
    node.setStringProperty("name", force.getName());
    SerializationNode& positionsNode = node.createChildNode("ReferencePositions");
    for (const Vec3& pos : force.getReferencePositions())
       positionsNode.createChildNode("Position").setDoubleProperty("x", pos[0]).setDoubleProperty("y", pos[1]).setDoubleProperty("z", pos[2]);
    SerializationNode& groupsNode = node.createChildNode("Groups");
    for (int i = 0; i < force.getNumGroups(); i++) {
        const vector<int>& group = force.getGroup(i);
        SerializationNode& groupNode = groupsNode.createChildNode("Group");
        for (int particle : group)
            groupNode.createChildNode("Particle").setIntProperty("index", particle);
    }
}

void* ConcertedRMSDForceProxy::deserialize(const SerializationNode& node) const {
    int version = node.getIntProperty("version");
    if (version != 0)
        throw OpenMMException("Unsupported version number");
    ConcertedRMSDForce* force = NULL;
    try {
        vector<Vec3> positions;
        for (auto& pos : node.getChildNode("ReferencePositions").getChildren())
            positions.push_back(Vec3(pos.getDoubleProperty("x"), pos.getDoubleProperty("y"), pos.getDoubleProperty("z")));
        vector<vector<int>> groups;
        for (auto& group : node.getChildNode("Groups").getChildren()) {
            vector<int> particles;
            for (auto& particle : group.getChildren())
                particles.push_back(particle.getIntProperty("index"));
            groups.push_back(particles);
        }
        force = new ConcertedRMSDForce(positions);
        for (auto& particles : groups)
            force->addGroup(particles);
        force->setForceGroup(node.getIntProperty("forceGroup", 0));
        force->setName(node.getStringProperty("name", force->getName()));
        return force;
    }
    catch (...) {
        if (force != NULL)
            delete force;
        throw;
    }
}
