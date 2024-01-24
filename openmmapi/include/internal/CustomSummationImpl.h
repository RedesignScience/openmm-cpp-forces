#ifndef CUSTOMCPPFORCES_CUSTOMSUMMATIONIMPL_H_
#define CUSTOMCPPFORCES_CUSTOMSUMMATIONIMPL_H_

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
#include "openmm/Context.h"
#include "openmm/CustomCompoundBondForce.h"
#include "openmm/Platform.h"
#include "openmm/internal/ContextImpl.h"

#include <map>
#include <string>
#include <vector>

using namespace OpenMM;
using namespace std;

namespace CustomCPPForces {

class CustomSummationImpl {
public:
    CustomSummationImpl(
        int numArgs,
        string expression,
        map<string, double> overallParameters,
        vector<string> perTermParameters,
        Platform &platform,
        map<string, string> platformProperties
    );
    ~CustomSummationImpl();
    double evaluate(const vector<double> &arguments);
    vector<double> evaluateDerivatives(const vector<double> &arguments);
    void update(const vector<vector<double>> &parameters);
    void setParameter(const string &name, double value);
private:
    void setPositions(const vector<double> &arguments);
    int numArgs;
    Context *context;
    bool contextIsUnchanged;
    CustomCompoundBondForce *force;
    vector<int> particles;
    vector<Vec3> positions;
    vector<double> latestArguments;
    double value;
    bool valueIsDirty;
    vector<double> derivatives;
    bool derivativesAreDirty;
};

} // namespace CustomCPPForces

#endif /*CUSTOMCPPFORCES_CUSTOMSUMMATIONIMPL_H_*/
