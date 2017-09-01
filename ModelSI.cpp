class ModelSI {

    public:
        void setupModel(MarkovChain &chain) {

            parameter_map parameterBeta;
            parameterBeta["beta"] = 0.2/10000;

            parameter_map parameterGamma;
            parameterGamma["gamma"] = 0.1429;

            chain.addState("S", 9900);
            chain.addState("I", 100);
            
            chain.addTransition(Transition("S","I", parameterBeta, NULL, Transition::TRANSITION_TYPE_MASS_ACTION));
            chain.addTransition(Transition("I","S", parameterGamma, NULL, Transition::TRANSITION_TYPE_INDIVIDUAL));
        }
};