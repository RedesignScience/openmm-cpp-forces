/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#include "ConcertedRMSDForce.h"

#include "openmm/internal/AssertionUtilities.h"
#include "openmm/serialization/XmlSerializer.h"
#include <iostream>
#include <sstream>

using namespace CustomCPPForces;
using namespace OpenMM;
using namespace std;

void testSerialization() {
    // Create a Force.

    vector<Vec3> refPos;
    for (int i = 0; i < 10; i++)
        refPos.push_back(Vec3(i/5.0, i*1.2, i*i/3.5));
    vector<int> particles;
    for (int i = 0; i < 5; i++)
        particles.push_back(i*i);
    ConcertedRMSDForce force(refPos);
    force.addGroup(particles);
    force.setForceGroup(3);
    force.setName("custom name");

    // Serialize and then deserialize it.

    stringstream buffer;
    XmlSerializer::serialize<ConcertedRMSDForce>(&force, "Force", buffer);
    ConcertedRMSDForce* copy = XmlSerializer::deserialize<ConcertedRMSDForce>(buffer);

    // Compare the two forces to see if they are identical.

    ConcertedRMSDForce& force2 = *copy;
    ASSERT_EQUAL(force.getForceGroup(), force2.getForceGroup());
    ASSERT_EQUAL(force.getName(), force2.getName());
    ASSERT_EQUAL(force.getReferencePositions().size(), force2.getReferencePositions().size());
    for (int i = 0; i < force.getReferencePositions().size(); i++)
        ASSERT_EQUAL_VEC(force.getReferencePositions()[i], force2.getReferencePositions()[i], 0.0);
    ASSERT_EQUAL(force.getGroup(0).size(), force2.getGroup(0).size());
    for (int i = 0; i < force.getGroup(0).size(); i++)
        ASSERT_EQUAL(force.getGroup(0)[i], force2.getGroup(0)[i]);
}

int main() {
    try {
        testSerialization();
    }
    catch(const exception& e) {
        cout << "exception: " << e.what() << endl;
        return 1;
    }
    cout << "Done" << endl;
    return 0;
}
