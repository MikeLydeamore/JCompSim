#include "MarkovChain/MarkovChain.cpp"
#include "ModelSIR.cpp"
#include "ModelGASScabies.cpp"
#include "ModelSI.cpp"
#include "ModelSIS.cpp"
#include "ModelSIRWS.cpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <filename>\n";
    return -1;
  }
  MarkovChain chain;
  //ModelSIR model;
  //ModelGASScabies model;
  //ModelSI model;
  //ModelSIS model;
  ModelSIRWS model;
  model.setupModel(chain);
  chain.setMaxTime(50);
  chain.setOutputFile(argv[1]);
  chain.solve(MarkovChain::SOLVER_TYPE_GILLESPIE);

  return 0;
}
