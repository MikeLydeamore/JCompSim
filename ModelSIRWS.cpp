template<class T>
class ModelSIRWS {

public:
    void setupModel(MarkovChain<T> &rChain) {
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

        rChain.addTransition(Transition<T>("S", "I", beta, Transition<T>::TRANSITION_TYPE_MASS_ACTION));
        rChain.addTransition(Transition<T>("I", "R", gamma, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition<T>("R", "W", 2*kappa, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition<T>("W", "R", nu*beta, Transition<T>::TRANSITION_TYPE_MASS_ACTION, {"I"}));
        rChain.addTransition(Transition<T>("R", "S", 2*kappa, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));

        rChain.addTransition(Transition<T>("I", "S", mu, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition<T>("R", "S", mu, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
        rChain.addTransition(Transition<T>("W", "S", mu, Transition<T>::TRANSITION_TYPE_INDIVIDUAL));
    }
};