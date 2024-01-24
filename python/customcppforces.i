%module customcppforces

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

namespace CustomCPPForces {

/**
 * This is a force whose energy equals the root mean squared deviation (RMSD)
 * between the current coordinates and a reference structure.  It is intended for
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
 *     the reference positions to compute the deviation
 *     from.  The length of this vector must equal the
 *     number of particles in the system, even if not
 *     all particles are used in computing the RMSD.
 * particles
 *     the indices of the particles to use when computing
 *     the RMSD.  If this is empty (the default), all
 *     particles in the system will be used.
 */
class ConcertedRMSDForce : public OpenMM::Force {
public:
    explicit ConcertedRMSDForce(
        const std::vector<OpenMM::Vec3>& referencePositions,
        const std::vector<int>& particles=std::vector<int>()
    );
    /**
     * Get the reference positions to compute the deviation from.
     */
    const std::vector<OpenMM::Vec3>& getReferencePositions() const;
    /**
     * Set the reference positions to compute the deviation from.
     */
    void setReferencePositions(const std::vector<OpenMM::Vec3>& positions);
    /**
     * Get the indices of the particles to use when computing the RMSD.  If this
     * is empty, all particles in the system will be used.
     */
    const std::vector<int>& getGroup(0) const;
    /**
     * Set the indices of the particles to use when computing the RMSD.  If this
     * is empty, all particles in the system will be used.
     */
    void setGroup(0, const std::vector<int>& particles);
    /**
     * Update the reference positions and particle indices in a Context to match those stored
     * in this Force object.  This method provides an efficient method to update certain parameters
     * in an existing Context without needing to reinitialize it.  Simply call setReferencePositions()
     * and setParticles() to modify this object's parameters, then call updateParametersInContext()
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
        static CustomCPPForces::ConcertedRMSDForce& cast(OpenMM::Force& force) {
            return dynamic_cast<CustomCPPForces::ConcertedRMSDForce&>(force);
        }

        static bool isinstance(OpenMM::Force& force) {
            return (dynamic_cast<CustomCPPForces::ConcertedRMSDForce*>(&force) != NULL);
        }
    }
};

}
