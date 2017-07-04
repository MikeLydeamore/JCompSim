#include "MarkovChain/MarkovChain.cpp"
#include "ModelSIR.cpp"
#include "ModelGASScabies.cpp"
#include "ModelSI.cpp"

int main() {
  MarkovChain chain;
  //ModelSIR model;
  ModelGASScabies model;
  //ModelSI model;
  model.setupModel(chain);
  chain.setMaxTime(300);
  chain.simulateChain();

  return 0;
}
