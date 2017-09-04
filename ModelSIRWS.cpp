class ModelSIRWS {

public:
    void setupModel(MarkovChain &rChain) {
        double beta = 260;
        double gamma = 17;
        double mu = 1.0/80;
        double nu = 5;
        double kappa = 0.1;

        rChain.addState("S");
        rChain.addState("I");
        rChain.addState("R");
        rChain.addState("W");

        rChain.addTransition(Transition("S", "I", beta, Transition::TRANSITION_TYPE_MASS_ACTION));
        rChain.addTransition(Transition("I","R", gamma, Transition::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition("R","W", 2*kappa, Transition::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition("W","R", nu*beta, Transition::TRANSITION_TYPE_MASS_ACTION, {"I"}));
        rChain.addTransition(Transition("R","S", 2*kappa, Transition::TRANSITION_TYPE_INDIVIDUAL));
    }
}