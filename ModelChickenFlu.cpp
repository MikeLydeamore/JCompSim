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

        const std::vector<std::string> disease_states = {"S", "E" "I"};
        const std::vector<std::string> demographic_states = {"Ch","eG","lG","He","Ro"};

        for (std::vector<std::string>const_iterator iterator_demographics = demographic_states.begin() ; 
        iterator_demographics != demographic_states.end() ; iterator_demographics++) {
            for (std::vector<std::string>const_iterator iterator_disease = disease_states.begin() ;
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
        ageing_rates["eG"] = 1.0/10;
        ageing_rates["lG"] = 1.0/10;

        rChain.addTransition(Transition<T>("E", "Ch-S", 1.0/3, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        for (std::vector<std::string>const_iterator iterator_disease = disease_states.begin() ;
        iterator_disease != disease_states.end() ; iterator_disease++) {
            for (std::vector<std::string>const_iterator iterator_demographics = demographic_states.begin() ;
            iterator_demographics != demographic_states.end() ; iterator_demographics++) {
                if ( (iterator_demographics + 1) == demographic_states.end()) {
                    break;
                }
                std::string source_state = *iterator_demographics + "-" + *iterator_disease;
                std::string destination_state = *(iterator_demographics + 1) + "-" + *iterator_disease;

                rChain.addTransition(Transition<T>(source_state, destination_state, ageing_rates[*iterator_demographics], Transition<T>::TRANSITION_TYPE_INDIVIDUAL))
            }
        }


    }
};