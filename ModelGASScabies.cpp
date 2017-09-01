class ModelGASScabies {
    /*
        First index = GAS
        Second index = Scabies
    */

    protected:
        static double rateSStoIS(state_values states, parameter_map parameters) { //Infection with GAS, no scabies
            return (parameters["beta_gas"] * states["SS"] * (states["IS"] + states["II"]));
        }

        static double rateIStoSS(state_values states, parameter_map parameters) { //Recovery from GAS, no scabies
            return (parameters["gamma_gas"] * states["IS"]);
        }

        static double rateSItoII(state_values states, parameter_map parameters) { //Infection with GAS, WITH scabies
            return (parameters["beta_gas"] * states["SI"] * (states["IS"] + states["II"]));
        }

        static double rateIItoSI(state_values states, parameter_map parameters) { //Recovery from GAS, WITH scabies
            return (parameters["gamma_gas"] * states["II"]);
        }

        static double rateSStoSI(state_values states, parameter_map parameters) { //Infection with scabies, no GAS
            return (parameters["beta_scabies"] * states["SS"] * (states["SI"] + states["II"]));
        }

        static double rateSItoSS(state_values states, parameter_map parameters) { //Recovery from scabies, no GAS
            return (parameters["gamma_scabies"] * states["SI"]);
        }

        static double rateIStoII(state_values states, parameter_map parameters) { //Infection with scabies, WITH GAS
            return (parameters["beta_scabies"] * states["IS"] * (states["SI"] + states["II"]));
        }

        static double rateIItoIS(state_values states, parameter_map parameters) { //Recovery from scabies, WITH GAS
            return (parameters["gamma_scabies"] * states["II"]);
        }

    public:
        void setupModel(MarkovChain &rChain) {
            double beta_gas = 1.0/5.0;
            double gamma_gas = 1.0/7.0;
            
            double beta_scabies = beta_gas;
            double gamma_scabies = gamma_gas;

            double enhanced_susc = 1;

            int population_size = 10000;
            int initial_infected_scabies = 100;
            int initial_infected_gas = 100;

            parameter_map parameters;
            parameters["beta_gas"] = beta_gas / (population_size-1);

            parameters["gamma_gas"] = gamma_gas;
            parameters["beta_scabies"] = beta_scabies / (population_size-1);
            parameters["gamma_scabies"] = gamma_scabies;
            parameters["enhanced_susc"] = enhanced_susc;

            rChain.addState("SS", population_size-initial_infected_gas-initial_infected_scabies);
            rChain.addState("SI", initial_infected_scabies);
            rChain.addState("IS", initial_infected_gas);
            rChain.addState("II", 0);

            rChain.addTransition(Transition("SS", "SI", parameters, *rateSStoSI));
            rChain.addTransition(Transition("SI", "SS", parameters, *rateSItoSS));
            rChain.addTransition(Transition("SS", "IS", parameters, *rateSStoIS));
            rChain.addTransition(Transition("IS", "SS", parameters, *rateIStoSS));
            rChain.addTransition(Transition("SI", "II", parameters, *rateSItoII));
            rChain.addTransition(Transition("II", "SI", parameters, *rateIItoSI));
            rChain.addTransition(Transition("IS", "II", parameters, *rateIStoII));
            rChain.addTransition(Transition("II", "IS", parameters, *rateIItoIS));
        }
};