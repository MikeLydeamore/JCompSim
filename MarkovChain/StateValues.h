#ifndef STATEVALUES_H_
#define STATEVALUES_H_

#include <string>
#include <map>

template<class T>
using state_values = std::map<std::string, T>;

#endif

#ifndef PARAMETER_MAP_H_
#define PARAMETERS_MAP_H_

#include <string>
#include <map>
typedef std::map<std::string, double> parameter_map;

#endif
