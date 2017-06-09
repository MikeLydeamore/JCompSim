#include <iostream>
#include <utility>
#include "StateValues.h"

class Transition {

protected:
  std::string source_state;
  std::string destination_state;
  parameter_map parameters;
  double (*getActualRate)(state_values pStates, parameter_map parameters);

public:
  
  const static int TRANSITION_TYPE_INDIVIDUAL = 0;
  const static int TRANSITION_TYPE_MASS_ACTION = 1;
  
  Transition(std::string pSource_state, std::string pDestination_state, parameter_map _parameters, double (*pGetActualRate)(state_values pStates, parameter_map parameters)) {
    source_state = pSource_state;
    destination_state = pDestination_state;
    getActualRate = pGetActualRate;
    parameters = _parameters;
  }

  void setStates(std::string pSource_state, std::string pDestination_state) {
    source_state = pSource_state;
    destination_state = pDestination_state;
  }

  void do_transition (state_values &pStates) {
    pStates[source_state]--;
    pStates[destination_state]++;
  }

  double getRate(state_values pStates)
  {
    return getActualRate(pStates, parameters);
  }

};
