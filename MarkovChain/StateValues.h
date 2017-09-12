#ifndef STATEVALUES_DISCRETE_H_
#define STATEVALUES_DISCRETE_H_

#include <string>
#include <map>

typedef std::map<std::string, int> state_values_discrete;

#endif

#ifndef STATEVALUES_CONTINUOUS_H_
#define STATEVALUES_CONTINUOUS_H_

typedef std::map<std::string, double> state_values_continuous;

#endif

#ifndef PARAMETER_MAP_H_
#define PARAMETERS_MAP_H_

#include <string>
#include <map>
typedef std::map<std::string, double> parameter_map;

#endif
