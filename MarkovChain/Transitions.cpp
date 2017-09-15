#include <iostream>
#include <utility>
#include "StateValues.h"
#include <json.hpp>

using json = nlohmann::json;

template<class T>
class Transition {

protected:
  std::string mSource_state;
  std::string mDestination_state;
  std::vector<std::string> mGoverning_states;
  parameter_map mParameters;

  double (*mpGetActualRate)(state_values<T> pStates, parameter_map parameters);

  int mTransition_type = 0;

public:
  
  const static int TRANSITION_TYPE_CUSTOM = -1;
  const static int TRANSITION_TYPE_INDIVIDUAL = 0;
  const static int TRANSITION_TYPE_MASS_ACTION = 1;

  Transition() {};
  
  Transition(std::string source_state, std::string destination_state, parameter_map parameters, double (*getActualRate)(state_values<T> pStates, parameter_map parameters), int transition_type = -1, std::vector<std::string> governing_states = {}) :
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

  void do_transition (state_values<T> &rStates) {
    rStates[mSource_state] -= 1;
    rStates[mDestination_state] += 1;
  }

  double getRate(state_values<T> states)
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

  std::string getSourceState() const {
    return (mSource_state);
  }

  void setSourceState(std::string source_state) {
    mSource_state = source_state;
  }

  std::string getDestinationState() const {
    return (mDestination_state);
  }

  void setDestinationState(std::string destination_state) {
    mDestination_state = destination_state;
  }

  int getTransitionType() const {
    return (mTransition_type);
  }

  void setTransitionType(int transition_type) {
    mTransition_type = transition_type;
  }

  double getSingleParameter() const {
    return (mParameters.begin()->second);
  }

  void setSingleParameter(double parameter) {
    mParameters["parameter"] = parameter;
  }

  std::vector<std::string> getGoverningStates() const {
    return (mGoverning_states); 
  }

  void setGoverningStates(std::vector<std::string> governing_states) {
    mGoverning_states = governing_states;
  }

};

template<class T>
void to_json(json &j, const Transition<T> &transition) {
  j = json{{"source_state", transition.getSourceState()},
           {"destination_state", transition.getDestinationState()},
           {"transition_type", transition.getTransitionType()},
           {"parameter", transition.getSingleParameter()},
           {"governing_states", transition.getGoverningStates()}};
}

template<class T>
void from_json(const json &j, Transition<T> &transition) {
  transition.setSourceState(j.at("source_state").get<std::string>());
  transition.setDestinationState(j.at("destination_state").get<std::string>());
  transition.setTransitionType(j.at("transition_type").get<int>());
  transition.setSingleParameter(j.at("parameter").get<double>());
  
  if (transition.getTransitionType() == 1) {
    transition.setGoverningStates(j.at("governing_states").get<std::vector<std::string> >());
  }
}
