#include <iostream>
#include <utility>
#include <vector>
#include <assert.h>
#include "StateValues.h"
#include "Transitions.cpp"

double beta = 2;
double gamma = 1;
int population_size = 100;

state_values states;
std::vector<Transition> transitions;

double t = 0;

const double T_MAX = 1000; //Arbitrary really

void setupSIR() {
  states["S"] = 99;
  states["I"] = 1;
  states["R"] = 0;

  Transition transitionSI = Transition("S", "I", beta/population_size, 1);
  transitions.push_back(transitionSI);

  Transition transitionIR = Transition("I","R", gamma, 0);
  transitions.push_back(transitionIR);
}

void serialise(int pT, state_values pStates) {
  std::cout << pT;

  std::map<std::string,int>::iterator it;

  for(it=pStates.begin(); it != pStates.end(); it++) {
    std::cout << "," << it->second;
  }
  std::cout << "\n";
}

int main() {
  setupSIR();
  std::cout << "t";
  for(state_values::iterator it=states.begin(); it != states.end(); it++) {
    std::cout << "," << it->first;
  }
  std::cout << "\n";

  serialise(t, states);

  while (t < T_MAX) {
    
    int actual_size = 0;
    for (state_values::iterator it = states.begin(); it != states.end(); it++) {
      assert(it->second >= 0);
      actual_size += it->second;
    }
    assert(actual_size != population_size)

  }

  return 0;
}
