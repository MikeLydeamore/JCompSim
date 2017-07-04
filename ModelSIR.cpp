
class ModelSIR {

    protected:
        static double getRateSToI(state_values states, parameter_map parameters) {

            return (parameters["beta"] * states["S"] * states["I"]);
        }

        static double getRateIToR(state_values states, parameter_map parameters) {
            return (parameters["gamma"] * states["I"]);
        }

    public:

        void setupModel(MarkovChain &chain) {
            int initial_infected = 100;
            double beta = 2;
            double gamma = 1;
            int population_size = 10000;
            parameter_map parametersSI;
            parametersSI["beta"] = beta/(population_size-1);
            
            parameter_map parametersIR;
            parametersIR["gamma"] = gamma;
            
            parametersIR["gamma"] = 1;
            
            chain.addState("S", population_size - initial_infected);
            chain.addState("I", initial_infected);
            chain.addState("R", 0);
     
            Transition transitionSI = Transition("S", "I", parametersSI, *getRateSToI);
            chain.addTransition(transitionSI);

            Transition transitionIR = Transition("I","R", parametersIR, *getRateIToR);
            chain.addTransition(transitionIR);
        }
};