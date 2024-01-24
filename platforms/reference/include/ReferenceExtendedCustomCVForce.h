/* -------------------------------------------------------------------------- *
 *                          OpenMM Custom CPP Forces                          *
 *                          ========================                          *
 *                                                                            *
 *  A plugin for distributing OpenMM CustomCPPForce instances                 *
 *                                                                            *
 *  Copyright (c) 2024 Charlles Abreu                                         *
 *  https://github.com/craabreu/customcppforces                               *
 * -------------------------------------------------------------------------- */

#ifndef __ReferenceExtendedCustomCVForce_H__
#define __ReferenceExtendedCustomCVForce_H__

#include "ExtendedCustomCVForce.h"
#include "openmm/internal/ContextImpl.h"
#include "lepton/ExpressionProgram.h"
#include <map>
#include <string>
#include <vector>

namespace CustomCPPForces {

class ReferenceExtendedCustomCVForce {
private:
    Lepton::ExpressionProgram energyExpression;
    std::vector<std::string> variableNames, paramDerivNames;
    std::vector<Lepton::ExpressionProgram> variableDerivExpressions;
    std::vector<Lepton::ExpressionProgram> paramDerivExpressions;

public:
    /**
     * Constructor
     */
    ReferenceExtendedCustomCVForce(const ExtendedCustomCVForce& force);

    /**
     * Destructor
     */
    ~ReferenceExtendedCustomCVForce();

    /**
     * Update any tabulated functions used by the force.  This is called when the user calls
     * updateParametersInContext().
     */
    void updateTabulatedFunctions(const ExtendedCustomCVForce& force);

    /**
     * Calculate the interaction.
     *
     * @param innerContext       the context created by the force for evaluating collective variables
     * @param atomCoordinates    atom coordinates
     * @param globalParameters   the values of global parameters
     * @param forces             the forces are added to this
     * @param totalEnergy        the energy is added to this
     * @param energyParamDerivs  parameter derivatives are added to this
     */
   void calculateIxn(ContextImpl& innerContext, std::vector<OpenMM::Vec3>& atomCoordinates,
                     const std::map<std::string, double>& globalParameters,
                     std::vector<OpenMM::Vec3>& forces, double* totalEnergy, std::map<std::string, double>& energyParamDerivs) const;
};

} // namespace CustomCPPForces

#endif // __ReferenceExtendedCustomCVForce_H__
