#include "MarkovChain/MarkovChain.cpp"
#include "ModelSIR.cpp"

void setupCoupledModel(){
  /*
  //Index 1: GAS
  //Index 2: Scabies

  int initial_infected_gas = 100;
  int initial_infected_scabies = 100;
  states["SS"] = population_size - initial_infected_gas - initial_infected_scabies;
  states["SI"] = 0.5*initial_infected_scabies;
  states["IS"] = initial_infected - 0.5*initial_infected_gas;
  states["II"] = initial_infected - 0.5*initial_infected_scabies;*/
}
int main() {
  MarkovChain chain;
  ModelSIR model;
  model.setupModel(chain);
  chain.simulateChain();

  return 0;
}
