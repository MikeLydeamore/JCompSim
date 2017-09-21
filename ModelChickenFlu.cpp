template<class T>
class ModelChickenFlu {

private:   
    static double eggLayingRate(state_values<T> states, parameter_map parameters) {
        T population_size = 0;
        for (typename state_values<T>::iterator it = states.begin() ; it != states.end() ; it++) {
            population_size = population_size + it->second;
        }

        double mu_bar = parameters["b"] * population_size * (1-( ((double) population_size)/parameters["K"]));
        if (parameters["ES"]>0) {
            return ( (1-parameters["w"]) * mu_bar);
        }
        else {
            return (parameters["w"] * mu_bar);
        }
    }
    
public:
    void setupModel(MarkovChain<T> &rChain) {
        
        //States!
        rChain.addState("E", 1000);
        rChain.addState("ES", 0);

        const std::vector<std::string> disease_states = {"S", "E", "I"};
        const std::vector<std::string> demographic_states = {"Ch","eG","lG","He","Ro"};

        for (std::vector<std::string>::const_iterator iterator_demographics = demographic_states.begin() ; 
        iterator_demographics != demographic_states.end() ; iterator_demographics++) {
            for (std::vector<std::string>::const_iterator iterator_disease = disease_states.begin() ;
            iterator_disease != disease_states.end() ; iterator_disease++) {
                std::string state_name = *iterator_demographics + "-" + *iterator_disease;
                T initial_population = (*iterator_disease == "S") ? 20 : 0;

                rChain.addState(state_name, initial_population);
            }
        }

        //Aging transitions
        std::map<std::string, double> ageing_rates;
        ageing_rates["E"] = 1.0/3;
        ageing_rates["Ch"] = 1.0/4;
        ageing_rates["eG"] = 0.5/10;
        ageing_rates["lG"] = 0.5/10;
        double hen_proportion = 0.7;

        rChain.addTransition(Transition<T>("E", "Ch-S", ageing_rates["E"], Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        
        for (std::vector<std::string>::const_iterator disease_iterator = disease_states.begin() ;
        disease_iterator != disease_states.end() ; disease_iterator++) {
            rChain.addTransition(Transition<T>("Ch-"+*disease_iterator, "Eg-"+*disease_iterator, ageing_rates["Ch"], Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
            rChain.addTransition(Transition<T>("eG-"+*disease_iterator, "lG-"+*disease_iterator, ageing_rates["eG"], Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
            rChain.addTransition(Transition<T>("lG-"+*disease_iterator, "He-"+*disease_iterator, 2*hen_proportion*ageing_rates["lG"], Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
            rChain.addTransition(Transition<T>("lG-"+*disease_iterator, "Ro-"+*disease_iterator, 2*(1-hen_proportion)*ageing_rates["lG"], Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        }


    }
};