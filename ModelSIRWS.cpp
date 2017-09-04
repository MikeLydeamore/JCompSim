class ModelSIRWS {

public:
    void setupModel(MarkovChain &rChain) {
        double beta = 260;
        double gamma = 17;
        double mu = 1.0/80;
        double nu = 5;
        double kappa = 0.1;

        parameter_map paramSI; paramSI["beta"] = beta;
        parameter_map paramIR; paramIR["gamma"] = gamma;
        parameter_map paramRW; paramRW["kappa"] = 2*kappa;
        parameter_map paramWR; paramWR["nubeta"] = nu * beta;
        parameter_map paramRS; paramRS["kappa"] = 2*kappa;
    }
}