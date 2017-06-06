#include <iostream>
#include <utility>
#include <vector>
#include "StateValues.h"
#include "Transitions.cpp"

double beta = 2;
double gamma = 1;

state_values states;
std::vector<Transition> transitions;

double t = 0;

const double T_MAX = 1000; //Arbitrary really

void setup() {
  states["S"] = 99;
  states["I"] = 1;
  states["R"] = 5;

  TransitionMassAction transitionSI;
  transitionSI.setStates("S","I");
  transitionSI.setRate(beta);
  transitions.push_back(transitionSI);
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
  setup();
  std::cout << "t";
  for(state_values::iterator it=states.begin(); it != states.end(); it++) {
    std::cout << "," << it->first;
  }
  std::cout << "\n";

  serialise(t, states);

  std::cout << transitions[0].getActualRate(states);

  return 0;
}
