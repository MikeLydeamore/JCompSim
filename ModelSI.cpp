class ModelSI {

    protected:
        static double getInfectionRate(state_values states, parameter_map parameters) {
            return (parameters["beta"] * states["S"] * states["I"]);
        }

        static double getRecoveryRate(state_values states, parameter_map parameters) {
            return (parameters["gamma"] * states["I"]);
        }

    public:
        void setupModel(MarkovChain &chain) {

            parameter_map parameterBeta;
            parameterBeta["beta"] = 0.2/10000;

            parameter_map parameterGamma;
            parameterGamma["gamma"] = 0.1429;

            chain.addState("S", 9900);
            chain.addState("I", 100);
            
            chain.addTransition(Transition("S","I", parameterBeta, *getInfectionRate));
            chain.addTransition(Transition("I","S", parameterGamma, *getRecoveryRate));
        }
};