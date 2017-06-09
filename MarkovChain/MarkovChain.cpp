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

class MarkovChain
{
  private:
    unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
        a=a-b;  a=a-c;  a=a^(c >> 13);
        b=b-c;  b=b-a;  b=b^(a << 8);
        c=c-a;  c=c-b;  c=c^(b >> 13);
        a=a-b;  a=a-c;  a=a^(c >> 12);
        b=b-c;  b=b-a;  b=b^(a << 16);
        c=c-a;  c=c-b;  c=c^(b >> 5);
        a=a-b;  a=a-c;  a=a^(c >> 3);
        b=b-c;  b=b-a;  b=b^(a << 10);
        c=c-a;  c=c-b;  c=c^(b >> 15);
        return c;
    }


  protected:
    void serialise(double pT, state_values pStates) {
        std::cout << pT;

        std::map<std::string,int>::iterator it;

        for(it=pStates.begin(); it != pStates.end(); it++) {
            std::cout << "," << it->second;
        }
        std::cout << "\n";
        }

    state_values states;
    std::vector<Transition> transitions;

  public:
    void addState(std::string state_name, int initial_value) {
        states[state_name] = initial_value;
    }

    void addTransition(Transition transition) {
        transitions.push_back(transition);
    }
    void simulateChain()
    {
        double t = 0;
        const double T_MAX = 1000; //Arbitrary really
        
        int population_size = 0;
        for (state_values::iterator it = states.begin(); it != states.end(); it++)
        {
            assert(it->second >= 0);
            population_size += it->second;
        }

        typedef boost::uniform_real<> NumberDistribution;
        typedef boost::mt19937 RandomNumberGenerator;
        typedef boost::variate_generator<RandomNumberGenerator &,
                                         NumberDistribution>
            Generator;

        NumberDistribution distribution(0, 1);
        RandomNumberGenerator generator;
        Generator runif(generator, distribution);
        unsigned long seed = mix(clock(), time(NULL), getpid());
        generator.seed(seed); // seed with the current time

        std::cout << "t";
        for (state_values::iterator it = states.begin(); it != states.end(); it++)
        {
            std::cout << "," << it->first;
        }
        std::cout << "\n";

        serialise(t, states);

        while (t < T_MAX)
        {

            int actual_size = 0;
            for (state_values::iterator it = states.begin(); it != states.end(); it++)
            {
                assert(it->second >= 0);
                actual_size += it->second;
            }
            assert(actual_size == population_size);

            std::vector<double> rates(transitions.size());
            for (int i = 0; i < transitions.size(); i++)
            {
                rates[i] = transitions[i].getRate(states);
            }
            double rates_sum = std::accumulate(rates.begin(), rates.end(), (double)0.0);

            double event_time = -(1.0 / rates_sum) * log(runif());
            if (isinf(event_time))
            {
                return;
            }
            t += event_time;

            std::vector<double> rates_normalised(rates.size());
            rates_normalised[0] = rates[0] / rates_sum;

            for (int i = 1; i < rates.size(); i++)
            {
                rates_normalised[i] = rates_normalised[i - 1] + (rates[i] / rates_sum);
            }

            int eventOccurred = 0;
            double u = runif();

            while (u > rates_normalised[eventOccurred])
            {
                eventOccurred++;
            }
            transitions[eventOccurred].do_transition(states);

            serialise(t, states);
        }
    }
};