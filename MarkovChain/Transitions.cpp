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
  
  Transition() {};
  
  Transition(std::string source_state, std::string destination_state, parameter_map parameters, double (*getActualRate)(state_values<T> pStates, parameter_map parameters), std::vector<std::string> governing_states = {}) :
    mSource_state(source_state), mDestination_state(destination_state), mParameters(parameters), mpGetActualRate(getActualRate), mGoverning_states(governing_states) 
    {
      if (mGoverning_states.size() == 0) {
        mGoverning_states = {mDestination_state};
      }
    }

    Transition(std::string source_state, std::string destination_state, double parameter, std::vector<std::string> governing_states = {}) :
    mSource_state(source_state), mDestination_state(destination_state), mGoverning_states(governing_states)
    {
      if (mGoverning_states.size() == 0) {
        mGoverning_states = {mDestination_state};
      }

      mParameters["parameter"] = parameter;
    }

    
  virtual void setStates(std::string source_state, std::string destination_state) {
    mSource_state = source_state;
    mDestination_state = destination_state;
  }

  virtual void do_transition (state_values<T> &rStates) = 0;

  virtual double getRate(state_values<T> states) = 0;

  virtual std::string getSourceState() const 
  {
    return (mSource_state);
  }

  virtual void setSourceState(std::string source_state) 
  {
    mSource_state = source_state;
  }

  virtual std::string getDestinationState() const 
  {
    return (mDestination_state);
  }

  virtual void setDestinationState(std::string destination_state) 
  {
    mDestination_state = destination_state;
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
class TransitionIndividual : public Transition<T> 
{
public:
  TransitionIndividual(std::string source_state, std::string destination_state, double parameter)
    : Transition<T>(source_state, destination_state, parameter, {})
    {}

  virtual double getRate(state_values<T> states)
  {
    return (this->mParameters["parameter"] * states[this->mSource_state]);
  }

  virtual void do_transition(state_values<T> &rStates) {
    rStates[this->mSource_state] -= 1;
    rStates[this->mDestination_state] += 1;
  }
};

template<class T>
class TransitionMassAction : public Transition<T>
{
public:
  TransitionMassAction(std::string source_state, std::string destination_state, double parameter, std::vector<std::string> governing_states = {})
    : Transition<T>(source_state, destination_state, parameter, governing_states)
    {}

  virtual double getRate(state_values<T> states)
  {
    double mass = 0;
    for (std::vector<std::string>::iterator it = this->mGoverning_states.begin() ; it != this->mGoverning_states.end() ; it++)
    {
      mass += states[*it];
    }
    return (this->mParameters["parameter"] * states[this->mSource_state] * mass);
  }

  virtual void do_transition(state_values<T> &rStates)
  {
    rStates[this->mSource_state] -= 1;
    rStates[this->mDestination_state] += 1;
  }
};

template<class T>
class TransitionIndividualToVoid : public TransitionIndividual<T>
{
public:
  TransitionIndividualToVoid(std::string source_state, double parameter)
    : TransitionIndividual<T>(source_state, "Void", parameter)
    {}
  
  virtual void do_transition(state_values<T> &rStates)
  {
    rStates[this->mSource_state] -= 1;
  }
};

template<class T>
class TransitionCustomFromVoid : public Transition<T>
{
public:
  TransitionCustomFromVoid(std::string destination_state, parameter_map parameters, double (*getActualRate)(state_values<T> pStates, parameter_map parameters))
    : Transition<T>("Void", destination_state, parameters, getActualRate)
    {}

  virtual double getRate(state_values<T> states)
  {
    return (this->mpGetActualRate(states, this->mParameters));
  }

  virtual void do_transition(state_values<T> &rStates)
  {
    rStates[this->mDestination_state] += 1;
  }
};

template<class T>
class TransitionMassActionByPopulation : public TransitionMassAction<T>
{
private:
  std::vector<std::string> mPopulationStates;
public:
  TransitionMassActionByPopulation(std::string source_state, std::string destination_state, double parameter, std::vector<std::string> population_states, std::vector<std::string> governing_states = {})
    : TransitionMassAction<T>(source_state, destination_state, parameter, governing_states), mPopulationStates(population_states)
    {}

  virtual double getRate(state_values<T> states)
  {
    double mass = 0;
    for (std::vector<std::string>::iterator it = this->mGoverning_states.begin() ; it != this->mGoverning_states.end() ; it++)
    {
      mass += states[*it];
    }

    T population_size = 0;
    for (std::vector<std::string>::iterator it = mPopulationStates.begin() ; it != mPopulationStates.end() ; it++)
    {
      population_size += states[*it];
    }
    if (population_size == 0)
    {
      return (0);
    }

    return ( (this->mParameters["parameter"] * states[this->mSource_state] * mass)/population_size );
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
