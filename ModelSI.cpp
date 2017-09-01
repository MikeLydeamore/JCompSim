class ModelSI {

    public:
        void setupModel(MarkovChain &rChain) {

            parameter_map parameterBeta;
            parameterBeta["beta"] = 0.2/10000;

            parameter_map parameterGamma;
            parameterGamma["gamma"] = 0.1429;

            rChain.addState("S", 9900);
            rChain.addState("I", 100);
            
            rChain.addTransition(Transition("S","I", parameterBeta, NULL, Transition::TRANSITION_TYPE_MASS_ACTION));
            rChain.addTransition(Transition("I","S", parameterGamma, NULL, Transition::TRANSITION_TYPE_INDIVIDUAL));
        }
};