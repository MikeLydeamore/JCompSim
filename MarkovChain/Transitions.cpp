#include <iostream>
#include <utility>
#include "StateValues.h"

class Transition {

protected:
  std::string mSource_state;
  std::string mDestination_state;
  parameter_map mParameters;
  double (*mGetActualRate)(state_values pStates, parameter_map parameters);
  int mTransition_type = 0;

public:
  
  const static int TRANSITION_TYPE_CUSTOM = -1;
  const static int TRANSITION_TYPE_INDIVIDUAL = 0;
  const static int TRANSITION_TYPE_MASS_ACTION = 1;
  
  Transition(std::string source_state, std::string destination_state, parameter_map parameters, double (*getActualRate)(state_values pStates, parameter_map parameters), int transition_type = -1) :
    mSource_state(source_state), mDestination_state(destination_state), mParameters(parameters), mGetActualRate(getActualRate), mTransition_type(transition_type) {};

  void setStates(std::string source_state, std::string destination_state) {
    mSource_state = source_state;
    mDestination_state = destination_state;
  }

  void do_transition (state_values &states) {
    states[mSource_state]--;
    states[mDestination_state]++;
  }

  double getRate(state_values states)
  {
    if (mTransition_type == TRANSITION_TYPE_CUSTOM)
    {
      return mGetActualRate(states, mParameters);
    }
    if (mTransition_type == TRANSITION_TYPE_INDIVIDUAL) 
    {
      return ( mParameters[0] * (states[mSource_state]) );
    }
    if (mTransition_type == TRANSITION_TYPE_MASS_ACTION)
    {
      return (mParameters[0] * states[mSource_state] * states[mDestination_state]);
    }

    return (0);
  }

};
