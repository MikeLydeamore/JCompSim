#include <json.hpp>
#include "../MarkovChain.hpp"

using json = nlohmann::json;


class TransitionJson {
private:
    std::string mSource_state;
    std::string mDestination_state;
    std::vector<std::string> mGoverning_states;
    std::vector<std::string> mCounters;

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

    std::vector<std::string> getCounters() const
    {
        return (mCounters);
    }

    void setCounters(std::vector<std::string> counters)
    {
        mCounters = counters;
    }
};


void to_json(json &j, const TransitionJson &transition) 
{
    if (transition.getTransitionType() == "mass-action")
    {
        j = json{{"source_state", transition.getSourceState()},
        {"destination_state", transition.getDestinationState()},
        {"transition_type", transition.getTransitionType()},
        {"parameter", transition.getSingleParameter()},
        {"governing_states", transition.getGoverningStates()},
        {"counters", transition.getCounters()}};
    } else
    {
        j = json{{"source_state", transition.getSourceState()},
        {"destination_state", transition.getDestinationState()},
        {"transition_type", transition.getTransitionType()},
        {"parameter", transition.getSingleParameter()},
        {"counteres", transition.getCounters()}};
    }
  
}


void from_json(const json &j, TransitionJson &transition) 
{
  transition.setSourceState(j.at("source_state").get<std::string>());
  transition.setDestinationState(j.at("destination_state").get<std::string>());
  transition.setTransitionType(j.at("transition_type").get<std::string>());
  transition.setSingleParameter(j.at("parameter").get<double>());
  transition.setCounters(j.at("counters").get<std::vector<std::string> >());
  
  if (transition.getTransitionType() == "mass-action") {
    transition.setGoverningStates(j.at("governing_states").get<std::vector<std::string> >());
  }
}


class ModelJson {

private:
    std::map<std::string, double> states;
    std::vector<TransitionJson > transitions;

    void addTransitionJson(MarkovChain &rChain, TransitionJson transition)
    {
        if (transition.getTransitionType() == "mass-action")
        {
            TransitionMassAction* t = new TransitionMassAction(transition.getSourceState(), 
                                                             transition.getDestinationState(),
                                                             transition.getSingleParameter(),
                                                             transition.getGoverningStates());
            if (transition.getCounters().size() > 0)
            {
                for (std::string counter : transition.getCounters())
                    t->addCounter(counter);
            }
            rChain.addTransition(t);
        } else if (transition.getTransitionType() == "individual")
        {
            TransitionIndividual* t = new TransitionIndividual(transition.getSourceState(), 
                                                             transition.getDestinationState(),
                                                             transition.getSingleParameter());
            
            if (transition.getCounters().size() > 0)
            {
                for (std::string counter : transition.getCounters())
                    t->addCounter(counter);
            }
            rChain.addTransition(t);
        } else if (transition.getTransitionType() == "constant")
        {
            TransitionConstant* t = new TransitionConstant(transition.getSourceState(),
                                                           transition.getDestinationState(),
                                                           transition.getSingleParameter());
            if (transition.getCounters().size() > 0)
            {
                for (std::string counter : transition.getCounters())
                    t->addCounter(counter);
            }
            rChain.addTransition(t);
        }
    }

public:
    ModelJson(json states_json, json transitions_json) {
        for (json::iterator it = states_json.begin() ; it != states_json.end() ; it++) {
            states[it.key()] = it.value();
        }

        transitions = transitions_json.get<std::vector<TransitionJson > >();
    }

    void setupModel(MarkovChain &rChain) {
        for(typename std::map<std::string, double>::iterator it = states.begin() ; it != states.end() ; it++) {
            rChain.addState(it->first, it->second);
        }

        for(typename std::vector<TransitionJson >::iterator it = transitions.begin() ; it != transitions.end() ; it++) {
            addTransitionJson(rChain, *it);
        }
    }
};