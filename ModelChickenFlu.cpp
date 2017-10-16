#include <math.h>

template<class T>
class ModelChickenFlu {

private:   
    static double eggLayingRate(state_values<T> states, parameter_map parameters) {
        T population_size = 0;
        for (typename state_values<T>::iterator it = states.begin() ; it != states.end() ; it++) {
            if (it->first != "E" || it->first != "ES") {
                population_size = population_size + it->second;
            }
        }
        if (population_size == 0)
        {
            return (0);
        }
        double n_egg = 8.9;
        double q = 4;
        double f = ((double) states["He-S"])/population_size;
        double mu = (f * population_size * n_egg * q) / population_size;
        double b = log(mu + 1);
        
        double mu_bar = b * population_size * (1-( ((double) population_size)/parameters["K"]));
        if (parameters["ES"]>0) {
            return ( parameters["w"] * mu_bar);
        }
        else {
            return (mu_bar);
        }
    }
    
public:
    void setupModel(MarkovChain<T> &rChain) {
        
        //States!
        rChain.addState("E", 0);
        rChain.addState("ES", 0);

        const std::vector<std::string> disease_states = {"S", "E", "I"};
        const std::vector<std::string> demographic_states = {"Ch","eG","lG","He","Rs"};
        std::vector<std::string> population_states;

        for (std::vector<std::string>::const_iterator iterator_demographics = demographic_states.begin() ; 
        iterator_demographics != demographic_states.end() ; iterator_demographics++) {
            for (std::vector<std::string>::const_iterator iterator_disease = disease_states.begin() ;
            iterator_disease != disease_states.end() ; iterator_disease++) {
                std::string state_name = *iterator_demographics + "-" + *iterator_disease;
                T initial_population = 0;
                if (*iterator_disease == "S")
                {
                    initial_population = 200;
                }
                if (state_name == "He-I" || state_name == "He-S")
                {
                    initial_population = 100;
                }

                rChain.addState(state_name, initial_population);

                population_states.push_back(state_name);
            }
        }

        //Aging transitions
        std::map<std::string, double> ageing_rates;
        ageing_rates["E"] = 1.0/21;
        ageing_rates["Ch"] = 1.0/28;
        ageing_rates["eG"] = 2.0/70;
        ageing_rates["lG"] = 2.0/70;
        double hen_proportion = 0.83;

        std::map<std::string, double> h;
        h["lG"] = 0.13;
        h["He"] = 0.304;
        h["Rs"] = 0.296;

        std::map<std::string, double> alpha;
        alpha["lG"] = 0.545;
        alpha["He"] = 0.009;
        alpha["Rs"] = 0.245;

        rChain.addTransition(new TransitionIndividual<T>("E", "Ch-S", ageing_rates["E"]));
        rChain.addTransition(new TransitionIndividualToVoid<T>("E", 1.0/47.05));
        
        for (std::vector<std::string>::const_iterator disease_iterator = disease_states.begin() ;
        disease_iterator != disease_states.end() ; disease_iterator++) 
        {
            //Ageing
            rChain.addTransition(new TransitionIndividual<T>("Ch-"+*disease_iterator, "eG-"+*disease_iterator, ageing_rates["Ch"]));
            rChain.addTransition(new TransitionIndividual<T>("eG-"+*disease_iterator, "lG-"+*disease_iterator, ageing_rates["eG"]));
            rChain.addTransition(new TransitionIndividual<T>("lG-"+*disease_iterator, "He-"+*disease_iterator, hen_proportion*ageing_rates["lG"]));
            rChain.addTransition(new TransitionIndividual<T>("lG-"+*disease_iterator, "Rs-"+*disease_iterator, (1-hen_proportion)*ageing_rates["lG"]));

            //Death
            rChain.addTransition(new TransitionIndividualToVoid<T>("Ch-"+*disease_iterator, 1.0/25));
            rChain.addTransition(new TransitionIndividualToVoid<T>("eG-"+*disease_iterator, 1.0/268));
            rChain.addTransition(new TransitionIndividualToVoid<T>("lG-"+*disease_iterator, h["lG"]*alpha["lG"]));
            rChain.addTransition(new TransitionIndividualToVoid<T>("lG-"+*disease_iterator, (1-h["lG"]*alpha["lG"])*(1.0/268)));
            rChain.addTransition(new TransitionIndividualToVoid<T>("He-"+*disease_iterator, h["He"]*alpha["He"]));
            rChain.addTransition(new TransitionIndividualToVoid<T>("He-"+*disease_iterator, (1-h["He"]*alpha["He"])*(1.0/10918)));
            rChain.addTransition(new TransitionIndividualToVoid<T>("Rs-"+*disease_iterator, h["Rs"]*alpha["Rs"]));
            rChain.addTransition(new TransitionIndividualToVoid<T>("Rs-"+*disease_iterator, (1-h["Rs"]*alpha["Rs"])*(1.0/25011)));
            

        }

        parameter_map eggParameters;
        eggParameters["w"] = 1.0/32.1;
        eggParameters["K"] = 1100;
        rChain.addTransition(new TransitionCustomFromVoid<T>("E", eggParameters, *eggLayingRate));

        double beta = 0.54;
        double sigma = 0.5;
        double gamma = 0.5;
        std::vector<std::string> infected_states = {"Ch-I", "eG-I", "lG-I", "He-I", "Rs-I"};

        for (std::vector<std::string>::const_iterator iterator = demographic_states.begin() ; iterator != demographic_states.end(); iterator++)
        {
            rChain.addTransition(new TransitionMassActionByPopulation<T>(*iterator+"-S", *iterator+"-E", beta, population_states, infected_states));
            rChain.addTransition(new TransitionIndividual<T>(*iterator+"-E", *iterator+"-I", sigma));
            rChain.addTransition(new TransitionIndividualToVoid<T>(*iterator+"-I", gamma));
        }
    }
};