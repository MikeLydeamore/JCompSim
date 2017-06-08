#include <iostream>
#include <utility>
#include <vector>
#include <assert.h>
#include <math.h>
#include <random>
#include <numeric>
#include <ctime>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random.hpp>
#include "StateValues.h"
#include "Transitions.cpp"

double mBeta = 2;
double mGamma = 1;
int population_size = 10000;
int initial_infected = 100;

state_values states;
std::vector<Transition> transitions;

double t = 0;

const double T_MAX = 1000; //Arbitrary really

void setupSIR() {
  states["S"] = population_size - initial_infected;
  states["I"] = initial_infected;
  states["R"] = 0;

  Transition transitionSI = Transition("S", "I", mBeta/(population_size-1), Transition::TRANSITION_TYPE_MASS_ACTION);
  transitions.push_back(transitionSI);

  Transition transitionIR = Transition("I","R", mGamma, Transition::TRANSITION_TYPE_INDIVIDUAL);
  transitions.push_back(transitionIR);
}

void serialise(double pT, state_values pStates) {
  std::cout << pT;

  std::map<std::string,int>::iterator it;

  for(it=pStates.begin(); it != pStates.end(); it++) {
    std::cout << "," << it->second;
  }
  std::cout << "\n";
}

int main() {
  setupSIR();

  typedef boost::uniform_real<> NumberDistribution; 
  typedef boost::mt19937 RandomNumberGenerator; 
  typedef boost::variate_generator<RandomNumberGenerator&, 
                                   NumberDistribution> Generator; 
 
  NumberDistribution distribution(0, 1); 
  RandomNumberGenerator generator; 
  Generator runif(generator, distribution); 
  generator.seed(std::time(0)); // seed with the current time 

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
    assert(actual_size == population_size);

    std::vector<double> rates(transitions.size());
    for (int i = 0 ; i < transitions.size() ; i++) {
      rates[i] = transitions[i].getActualRate(states);
    }
    double rates_sum = std::accumulate(rates.begin(), rates.end(), (double) 0.0);
    
    double event_time = -(1.0/rates_sum) * log(runif());
    if (isinf(event_time)) {
      return 0;
    }
    t += event_time;

    std::vector<double> rates_normalised(rates.size());
    rates_normalised[0] = rates[0] / rates_sum;

    for (int i = 1 ; i < rates.size() ; i++) {
      rates_normalised[i] = rates_normalised[i-1] + (rates[i] / rates_sum);
    }

    int eventOccurred = 0;
    double u = runif();
    
    while (u > rates_normalised[eventOccurred]) {
      eventOccurred++;
    }
    transitions[eventOccurred].do_transition(states);

    serialise(t, states);

  }

  return 0;
}
