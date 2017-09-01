
class ModelSIS {
    
    public:
        void setupModel(MarkovChain &rChain) {
            int initial_infected = 100;
            double lambda = 1.0/60;
            double gamma = 1.0/20;
                
            parameter_map parametersSI;
            parametersSI["lambda"] = lambda;
                
            parameter_map parametersIS;
            parametersIS["gamma"] = gamma;
                
            rChain.addState("S", 1);
            rChain.addState("I", 0);
         
            Transition transitionSI = Transition("S", "I", parametersSI, NULL, Transition::TRANSITION_TYPE_INDIVIDUAL);
            rChain.addTransition(transitionSI);
    
            Transition transitionIS = Transition("I","S", parametersIS, NULL, Transition::TRANSITION_TYPE_INDIVIDUAL);
            rChain.addTransition(transitionIS);
        }
};