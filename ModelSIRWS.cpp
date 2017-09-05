class ModelSIRWS {

public:
    void setupModel(MarkovChain &rChain) {
        double beta = 260;
        double gamma = 17;
        double mu = 1.0/80;
        double nu = 5;
        double kappa = 1.0/10;

        int population_size = 10000;
        int initial_infected = 50;

        rChain.addState("S", population_size - initial_infected);
        rChain.addState("I", initial_infected);
        rChain.addState("R", 0);
        rChain.addState("W", 0);

        rChain.addTransition(Transition("S", "I", beta, Transition::TRANSITION_TYPE_MASS_ACTION));
        rChain.addTransition(Transition("I", "R", gamma, Transition::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition("R", "W", 2*kappa, Transition::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition("W", "R", nu*beta, Transition::TRANSITION_TYPE_MASS_ACTION, {"I"}));
        rChain.addTransition(Transition("R", "S", 2*kappa, Transition::TRANSITION_TYPE_INDIVIDUAL));

        rChain.addTransition(Transition("I", "S", mu, Transition::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition("R", "S", mu, Transition::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition("W", "S", mu, Transition::TRANSITION_TYPE_INDIVIDUAL));
    }
};