template<class T>
class ModelSIR {
    public:
        
        void setupModel(MarkovChain<T> &rChain) {
            int initial_infected = 5;
            double beta = 1.1;
            double gamma = 1;
            int population_size = 10000;
                 
            rChain.addState("S", population_size - initial_infected);
            rChain.addState("I", initial_infected);
            rChain.addState("R", 0);
     
            Transition<T> transitionSI = Transition<T>("S", "I", beta/(population_size - 1), Transition<T>::TRANSITION_TYPE_MASS_ACTION);
            rChain.addTransition(transitionSI);

            Transition<T> transitionIR = Transition<T>("I","R", gamma, Transition<T>::TRANSITION_TYPE_INDIVIDUAL);
            rChain.addTransition(transitionIR);
        }
};