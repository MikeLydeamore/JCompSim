template<class T>
class ModelSIR {

    protected:
        static double getRateSToI(state_values_discrete<double> states, parameter_map parameters) {

            return (parameters["beta"] * states["S"] * states["I"]);
        }

        static double getRateIToR(state_values_discrete<double> states, parameter_map parameters) {
            return (parameters["gamma"] * states["I"]);
        }

    public:
        
        void setupModel(MarkovChain<T> &rChain) {
            int initial_infected = 100;
            double beta = 2;
            double gamma = 1;
            int population_size = 10000;
                 
            rChain.addState("S", population_size - initial_infected);
            rChain.addState("I", initial_infected);
            rChain.addState("R", 0);
     
            Transition transitionSI = Transition("S", "I", beta/(population_size - 1), Transition::TRANSITION_TYPE_MASS_ACTION);
            rChain.addTransition(transitionSI);

            Transition transitionIR = Transition("I","R", gamma, Transition::TRANSITION_TYPE_INDIVIDUAL);
            rChain.addTransition(transitionIR);
        }
};