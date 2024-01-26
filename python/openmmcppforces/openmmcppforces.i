
%module openmmcppforces

%import(module="openmm") "swig/OpenMMSwigHeaders.i"
%include "swig/typemaps.i"
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>

namespace std {
  %template(vectord) vector<double>;
  %template(vectorstring) vector<string>;
  %template(mapstringstring) map<string,string>;
  %template(mapstringdouble) map<string,double>;
};

%{
#define SWIG_PYTHON_CAST_MODE
#include "ConcertedRMSDForce.h"
#include "OpenMM.h"
#include "OpenMMAmoeba.h"
#include "OpenMMDrude.h"
#include "openmm/RPMDIntegrator.h"
#include "openmm/RPMDMonteCarloBarostat.h"
%}

%pythoncode %{
__version__ = "@CMAKE_PROJECT_VERSION@"
%}

/*
 * Convert C++ exceptions to Python exceptions.
*/

%exception {
    try {
        $action
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
        return NULL;
    }
}

namespace OpenMMCPPForces {

/**
 * This is a force whose energy equals a special type of root mean squared deviation
 * (RMSD) between the current coordinates and a reference structure.  It is intended for
 * use with CustomCVForce.  You will not normally want a force that exactly equals
 * the RMSD, but there are many situations where it is useful to have a restraining
 * or biasing force that depends on the RMSD in some way.
 *
 * The force is computed by first aligning the particle positions to the reference
 * structure, then computing the RMSD between the aligned positions and the reference.
 * The computation can optionally be done based on only a subset of the particles
 * in the system.
 *
 * Parameters
 * ----------
 * referencePositions
 *     the reference positions to compute the deviation from. The length of this vector
 *     must equal the number of particles in the system, even if not all particles are
 *     used in computing the Concerted RMSD.
 */
class ConcertedRMSDForce : public OpenMM::Force {
public:
    explicit ConcertedRMSDForce(const std::vector<OpenMM::Vec3>& referencePositions);
    /**
     * Get the reference positions to compute the deviation from.
     */
    const std::vector<OpenMM::Vec3>& getReferencePositions() const;
    /**
     * Set the reference positions to compute the deviation from.
     *
     * Parameters
     * ----------
     * positions
     *     the reference positions to compute the deviation from. The length of this
     *     vector must equal the number of particles in the system, even if not all
     *     particles are used in computing the concerted RMSD.
     */
    void setReferencePositions(const std::vector<OpenMM::Vec3>& positions);
    /**
     * Add a group of particles to be included in the concerted RMSD calculation.
     *
     * Parameters
     * ----------
     * particles
     *     the indices of the particles to include
     *
     * Returns
     * -------
     * int
     *     the index of the group that was added
     */
    int addGroup(const std::vector<int>& particles);
    /**
     * Get the number of particle groups included in the concerted RMSD calculation.
     */
    int getNumGroups() const;
    /**
     * Get the particles of a group included in the concerted RMSD calculation.
     *
     * Parameters
     * ----------
     * index
     *     the index of the group whose particles are to be retrieved
     *
     * Returns
     * -------
     * List[int]
     *     the indices of the particles in the group
     */
    const std::vector<int>& getGroup(int index) const;
    /**
     * Set the particles of a group included in the concerted RMSD calculation.
     *
     * Parameters
     * ----------
     * index
     *    the index of the group whose particles are to be set
     * particles
     *   the indices of the particles to include
     */
    void setGroup(int index, const std::vector<int>& particles);
    /**
     * Update the reference positions and particle groups in a Context to match those stored
     * in this Force object.  This method provides an efficient way to update these parameters
     * in an existing Context without needing to reinitialize it.  Simply call setReferencePositions()
     * and setGroup() to modify this object's parameters, then call updateParametersInContext()
     * to copy them over to the Context.
     */
    void updateParametersInContext(OpenMM::Context& context);
    /**
     * Returns whether or not this force makes use of periodic boundary
     * conditions.
     *
     * @returns true if force uses PBC and false otherwise
     */
    bool usesPeriodicBoundaryConditions();
    /*
     * Add methods for casting a Force to an ExtendedCustomCVForce.
    */
    %extend {
        static OpenMMCPPForces::ConcertedRMSDForce& cast(OpenMM::Force& force) {
            return dynamic_cast<OpenMMCPPForces::ConcertedRMSDForce&>(force);
        }

        static bool isinstance(OpenMM::Force& force) {
            return (dynamic_cast<OpenMMCPPForces::ConcertedRMSDForce*>(&force) != NULL);
        }
    }
};

}
