
class ModelSIS {
    
    public:
        void setupModel(MarkovChain &chain) {
            int initial_infected = 100;
            double lambda = 1.0/60;
            double gamma = 1.0/20;
                
            parameter_map parametersSI;
            parametersSI["lambda"] = lambda;
                
            parameter_map parametersIS;
            parametersIS["gamma"] = gamma;
                
            chain.addState("S", 1);
            chain.addState("I", 0);
         
            Transition transitionSI = Transition("S", "I", parametersSI, NULL, Transition::TRANSITION_TYPE_INDIVIDUAL);
            chain.addTransition(transitionSI);
    
            Transition transitionIS = Transition("I","S", parametersIS, NULL, Transition::TRANSITION_TYPE_INDIVIDUAL);
            chain.addTransition(transitionIS);
        }
};