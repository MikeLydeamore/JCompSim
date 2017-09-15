#include <json.hpp>

using json = nlohmann::json;

template<class T>
class ModelJson {

private:
    std::map<std::string, T> states;
    std::vector<Transition<T> > transitions;

public:
    ModelJson(json states_json, json transitions_json) {
        for (json::iterator it = states_json.begin() ; it != states_json.end() ; it++) {
            states[it.key()] = it.value();
        }

        transitions = transitions_json.get<std::vector<Transition<T> > >();
    }

    void setupModel(MarkovChain<T> &rChain) {
        for(typename std::map<std::string, T>::iterator it = states.begin() ; it != states.end() ; it++) {
            rChain.addState(it->first, it->second);
        }

        for(typename std::vector<Transition<T> >::iterator it = transitions.begin() ; it != transitions.end() ; it++) {
            rChain.addTransition(*it);
        }
    }
};