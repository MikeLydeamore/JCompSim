#include <iostream>
#include <utility>
#include "StateValues.h"

class Transition {

public:
  double getActualRate(state_values pStates) {
    return 0;
  }
  void setRate(double pRate) {
    rate = pRate;
  }
  void setStates(std::string pSource_state, std::string pDestination_state) {
    source_state = pSource_state;
    destination_state = pDestination_state;
  }

  void transition (state_values &pStates) {
    pStates[source_state]--;
    pStates[destination_state]++;
  }

protected:
  std::string source_state;
  std::string destination_state;
  double rate;

};

class TransitionIndividual : public Transition {
public:
  double getActualRate(state_values pStates) {
    return (rate * pStates[source_state]);
  }
};

class TransitionMassAction : public Transition {
public:
  double getActualRate(state_values pStates) {
    return (rate * pStates[source_state] * pStates[destination_state]);
  }
};
