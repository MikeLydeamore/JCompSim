#include <json.hpp>

using json = nlohmann::json;

template<class T>
class TransitionJson {
private:
    std::string mSource_state;
    std::string mDestination_state;
    std::vector<std::string> mGoverning_states;

    double mParameter;

    std::string mTransition_type;

public:
    TransitionJson() {}

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
  
    double getSingleParameter() const 
    {
      return (mParameter);
    }
  
    void setSingleParameter(double parameter) 
    {
      mParameter = parameter;
    }
  
    std::vector<std::string> getGoverningStates() const 
    {
      return (mGoverning_states); 
    }
  
    void setGoverningStates(std::vector<std::string> governing_states) 
    {
      mGoverning_states = governing_states;
    }

    std::string getTransitionType() const
    {
        return (mTransition_type);
    }

    void setTransitionType(std::string transition_type)
    {
        mTransition_type = transition_type;
    }
};

template<class T>
void to_json(json &j, const TransitionJson<T> &transition) 
{
    if (transition.getTransitionType() == "mass-action")
    {
        j = json{{"source_state", transition.getSourceState()},
        {"destination_state", transition.getDestinationState()},
        {"transition_type", transition.getTransitionType()},
        {"parameter", transition.getSingleParameter()},
        {"governing_states", transition.getGoverningStates()}};
    } else
    {
        j = json{{"source_state", transition.getSourceState()},
        {"destination_state", transition.getDestinationState()},
        {"transition_type", transition.getTransitionType()},
        {"parameter", transition.getSingleParameter()}};
    }
  
}

template<class T>
void from_json(const json &j, TransitionJson<T> &transition) 
{
  transition.setSourceState(j.at("source_state").get<std::string>());
  transition.setDestinationState(j.at("destination_state").get<std::string>());
  transition.setTransitionType(j.at("transition_type").get<std::string>());
  transition.setSingleParameter(j.at("parameter").get<double>());
  
  if (transition.getTransitionType() == "mass-action") {
    transition.setGoverningStates(j.at("governing_states").get<std::vector<std::string> >());
  }
}

template<class T>
class ModelJson {

private:
    std::map<std::string, T> states;
    std::vector<TransitionJson<T> > transitions;

    void addTransitionJson(MarkovChain<T> &rChain, TransitionJson<T> transition)
    {
        if (transition.getTransitionType() == "mass-action")
        {
            rChain.addTransition(new TransitionMassAction<T>(transition.getSourceState(), 
                                                             transition.getDestinationState(),
                                                             transition.getSingleParameter(),
                                                             transition.getGoverningStates()));
        } else if (transition.getTransitionType() == "individual")
        {
            rChain.addTransition(new TransitionIndividual<T>(transition.getSourceState(), 
                                                             transition.getDestinationState(),
                                                             transition.getSingleParameter()));
        } else if (transition.getTransitionType() == "constant")
        {
            rChain.addTransition(new TransitionConstant<T>(transition.getSourceState(),
                                                           transition.getDestinationState(),
                                                           transition.getSingleParameter()));
        }
    }

public:
    ModelJson(json states_json, json transitions_json) {
        for (json::iterator it = states_json.begin() ; it != states_json.end() ; it++) {
            states[it.key()] = it.value();
        }

        transitions = transitions_json.get<std::vector<TransitionJson<T> > >();
    }

    void setupModel(MarkovChain<T> &rChain) {
        for(typename std::map<std::string, T>::iterator it = states.begin() ; it != states.end() ; it++) {
            rChain.addState(it->first, it->second);
        }

        for(typename std::vector<TransitionJson<T> >::iterator it = transitions.begin() ; it != transitions.end() ; it++) {
            addTransitionJson(rChain, *it);
        }
    }
};