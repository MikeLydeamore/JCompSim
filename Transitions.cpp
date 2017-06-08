#include <iostream>
#include <utility>
#include "StateValues.h"

class Transition {

protected:
  std::string source_state;
  std::string destination_state;
  double rate;
  int transition_type;

public:
  const static int TRANSITION_TYPE_INDIVIDUAL = 0;
  const static int TRANSITION_TYPE_MASS_ACTION = 1;
  Transition(std::string pSource_state, std::string pDestination_state, double pRate, int pTransition_type) {
    source_state = pSource_state;
    destination_state = pDestination_state;
    rate = pRate;
    transition_type = pTransition_type;
  }

  double getActualRate(state_values pStates) {
    switch (transition_type) {
      case 0 : return (rate * pStates[source_state]);
      case 1: return (rate * pStates[source_state]*pStates[destination_state]);
      default: return 0;
    }
  }
  void setRate(double pRate) {
    rate = pRate;
  }
  void setStates(std::string pSource_state, std::string pDestination_state) {
    source_state = pSource_state;
    destination_state = pDestination_state;
  }

  void do_transition (state_values &pStates) {
    pStates[source_state]--;
    pStates[destination_state]++;
  }

  void setTransitionType(int pTransition_type) {
    transition_type = pTransition_type;
  }

};
