%module openmmcppforces

%{
#define SWIG_PYTHON_CAST_MODE
#include "CompositeRMSDForce.h"
#include "OpenMM.h"
#include "OpenMMAmoeba.h"
#include "OpenMMDrude.h"
#include "openmm/RPMDIntegrator.h"
#include "openmm/RPMDMonteCarloBarostat.h"
#include "openmm/Force.h"
#include "openmm/Vec3.h"
#include "openmm/Context.h"
#include <numpy/ndarrayobject.h>

using namespace OpenMM;
%}

%import(module="openmm") "swig/OpenMMSwigHeaders.i"
%include "swig/typemaps.i"
%include "header.i"
%include "std_vector.i"

namespace std {
  %template(vectori) vector<int>;
};

%pythoncode %{
from openmm import unit
%}

/*
Add units to function outputs.
*/

%pythonappend OpenMMCPPForces::CompositeRMSDForce::getReferencePositions() const %{
    val= unit.nanometers
%}

/*
Convert C++ exceptions to Python exceptions.
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

%feature("docstring") %{
This is a force whose energy equals the root mean squared deviation (RMSD) between the
current coordinates of multiple particle groups and reference structures defined for
each group. The composite RMSD is computed after aligning every particle group to its
reference while keeping the relative orientations of all groups fixed. Therefore, it is
not a mere composition of independent RMSD calculations done in the usual way.

Consider `m` particle groups, with each group :math:`{\bf g}_j` containing
:math:`n_j` particles. The centroid of each group is given by

.. math::

   {\bf c}_j = \frac{1}{n_j} \sum_{k \in {\bf g}_j} {\bf r}_k

where :math:`{\bf r}_k` is the position of particle :math:`k` and the sum is over
all particles in the group. Analogously, :math:`{\bf c}_j^{\rm ref}` is the
centroid of the reference structure for group :math:`j`, whose each particle
:math:`k` is located at :math:`{\bf r}_k^{\rm ref}`.

The composite RMSD is then defined as

.. math::

    d_{\rm crms}({\bf r}) = \sqrt{
        \frac{1}{n} \min_{
            \bf q \in \mathbb{R}^4 \atop \|{\bf q}\| = 1
        } \sum_{j=1}^m \sum_{k \in {\bf g}_j} \left\|
            {\bf A}({\bf q})\left({\bf r}_k - {\bf c}_j\right) -
                {\bf r}_k^{\rm ref} + {\bf c}_j^{\rm ref}
        \right\|^2
    }

where :math:`n = \sum_{j=1}^m n_j` is the total number of particles in all groups,
:math:`{\bf A}(\bf q)` is the rotation matrix corresponding to a unit quaternion
:math:`{\bf q}`. Note that a single rotation matrix is used to align all groups,
thus keeping their relative orientations fixed.

This force is intended for use with :OpenMM:`CustomCVForce`.

Parameters
----------
referencePositions
    the reference positions to compute the deviation from. The length of this vector
    must equal the number of particles in the system, even if not all particles are
    used in computing the Composite RMSD.

Examples
--------
    >>> import openmmcppforces as mmcpp
    >>> from openmmtools import testsystems
    >>> model = testsystems.HostGuestVacuum()
    >>> host_atoms, guest_atoms = (
    ...     [a.index for a in r.atoms()]
    ...     for r in model.topology.residues()
    ... )
    >>> force = mmcpp.CompositeRMSDForce(model.positions)
    >>> host_group, guest_group = (
    ...     force.addGroup(atoms)
    ...     for atoms in (host_atoms, guest_atoms)
    ... )
    >>> force.setForceGroup(1)
    >>> model.system.addForce(force)
    5
    >>> context = mm.Context(
    ...     model.system,
    ...     mm.VerletIntegrator(1.0 * unit.femtoseconds),
    ...     mm.Platform.getPlatformByName('Reference'),
    ... )
    >>> context.setPositions(model.positions)
    >>> context.getState(getEnergy=True, groups={1}).getPotentialEnergy()
%}
class CompositeRMSDForce : public OpenMM::Force {
public:
    explicit CompositeRMSDForce(const std::vector<Vec3>& referencePositions);

    %feature("docstring") %{
    Get the reference positions to compute the deviation from.
    %}
    const std::vector<Vec3>& getReferencePositions() const;

    %feature("docstring") %{
    Set the reference positions to compute the deviation from.

    Parameters
    ----------
    positions
        the reference positions to compute the deviation from. The length of this
        vector must equal the number of particles in the system, even if not all
        particles are used in computing the composite RMSD.
    %}
    void setReferencePositions(const std::vector<Vec3>& positions);

    %feature("docstring") %{
    Add a group of particles to be included in the composite RMSD calculation.

    Parameters
    ----------
    particles
        the indices of the particles to include

    Returns
    -------
    int
        the index of the group that was added
    %}
    int addGroup(const std::vector<int>& particles);

    %feature("docstring") %{
    Get the number of particle groups included in the composite RMSD calculation.
    %}
    int getNumGroups() const;

    %feature("docstring") %{
    Get the particles of a group included in the composite RMSD calculation.

    Parameters
    ----------
    index
        the index of the group whose particles are to be retrieved

    Returns
    -------
    List[int]
        the indices of the particles in the group
    %}
    const std::vector<int>& getGroup(int index) const;

    %feature("docstring") %{
    Set the particles of a group included in the composite RMSD calculation.

    Parameters
    ----------
    index
       the index of the group whose particles are to be set
    particles
      the indices of the particles to include
    %}
    void setGroup(int index, const std::vector<int>& particles);

    %feature("docstring") %{
    Update the reference positions and particle groups in a Context to match those stored
    in this OpenMM::Force object.  This method provides an efficient way to update these parameters
    in an existing Context without needing to reinitialize it.  Simply call setReferencePositions()
    and setGroup() to modify this object's parameters, then call updateParametersInContext()
    to copy them over to the Context.
    %}
    void updateParametersInContext(OpenMM::Context& context);

    %feature("docstring") %{
    Returns whether or not this force makes use of periodic boundary
    conditions.

    Returns
    -------
    bool
        Whether force uses PBC
    %}
    bool usesPeriodicBoundaryConditions();

    %extend {
        static OpenMMCPPForces::CompositeRMSDForce& cast(OpenMM::Force& force) {
            return dynamic_cast<OpenMMCPPForces::CompositeRMSDForce&>(force);
        }

        static bool isinstance(OpenMM::Force& force) {
            return (dynamic_cast<OpenMMCPPForces::CompositeRMSDForce*>(&force) != NULL);
        }
    }
};

}
