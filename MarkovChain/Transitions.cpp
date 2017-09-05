#include <iostream>
#include <utility>
#include "StateValues.h"

class Transition {

protected:
  std::string mSource_state;
  std::string mDestination_state;
  std::vector<std::string> mGoverning_states;
  parameter_map mParameters;
  double (*mpGetActualRate)(state_values pStates, parameter_map parameters);
  int mTransition_type = 0;

public:
  
  const static int TRANSITION_TYPE_CUSTOM = -1;
  const static int TRANSITION_TYPE_INDIVIDUAL = 0;
  const static int TRANSITION_TYPE_MASS_ACTION = 1;
  
  Transition(std::string source_state, std::string destination_state, parameter_map parameters, double (*getActualRate)(state_values pStates, parameter_map parameters), int transition_type = -1, std::vector<std::string> governing_states = {}) :
    mSource_state(source_state), mDestination_state(destination_state), mParameters(parameters), mpGetActualRate(getActualRate), mTransition_type(transition_type), mGoverning_states(governing_states) 
    {
      if (mGoverning_states.size() == 0) {
        mGoverning_states = {mDestination_state};
      }
    };

    Transition(std::string source_state, std::string destination_state, double parameter, int transition_type = -1, std::vector<std::string> governing_states = {}) :
    mSource_state(source_state), mDestination_state(destination_state), mTransition_type(transition_type), mGoverning_states(governing_states)
    {
      if (mGoverning_states.size() == 0) {
        mGoverning_states = {mDestination_state};
      }

      mParameters["parameter"] = parameter;
    };

  void setStates(std::string source_state, std::string destination_state) {
    mSource_state = source_state;
    mDestination_state = destination_state;
  }

  void do_transition (state_values &rStates) {
    rStates[mSource_state]--;
    rStates[mDestination_state]++;
  }

  double getRate(state_values states)
  {
    if (mTransition_type == TRANSITION_TYPE_CUSTOM)
    {
      return mpGetActualRate(states, mParameters);
    }
    if (mTransition_type == TRANSITION_TYPE_INDIVIDUAL) 
    {
      return (mParameters.begin()->second * states[mSource_state]);
    }
    if (mTransition_type == TRANSITION_TYPE_MASS_ACTION)
    {
      double governing_rate = 0;
      for (std::string governing_state : mGoverning_states)
      {
        governing_rate += states[governing_state];
      }
      return (mParameters.begin()->second * states[mSource_state] * governing_rate);
    }

    return (0);
  }

};
