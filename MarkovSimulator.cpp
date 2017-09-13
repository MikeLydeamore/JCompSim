#include "MarkovChain/MarkovChain.cpp"
//#include "MarkovChain/SerialiserFile.cpp"
//#include "ModelSIR.cpp"
//#include "ModelGASScabies.cpp"
//#include "ModelSI.cpp"
//#include "ModelSIS.cpp"
#include "ModelSIRWS.cpp"


int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <filename>\n";
    return -1;
  }

  using T = double;

  MarkovChain<T> chain;
  //ModelSIR model;
  //ModelGASScabies model;
  //ModelSI model;
  //ModelSIS model;
  ModelSIRWS<T> model;
  model.setupModel(chain);
  chain.setMaxTime(50);
  
  SerialiserFile<T> serialiser(argv[1]);

  chain.setSerialiser(&serialiser);

  chain.solve(chain.SOLVER_TYPE_GILLESPIE);

  return 0;
}
