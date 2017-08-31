#include "MarkovChain/MarkovChain.cpp"
#include "ModelSIR.cpp"
#include "ModelGASScabies.cpp"
#include "ModelSI.cpp"
#include "ModelSIS.cpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <filename>\n";
    return -1;
  }
  MarkovChain chain;
  //ModelSIR model;
  //ModelGASScabies model;
  //ModelSI model;
  ModelSIS model;
  model.setupModel(chain);
  chain.setMaxTime(5*365);
  chain.setOutputFile(argv[1]);
  chain.simulateChain();

  return 0;
}
