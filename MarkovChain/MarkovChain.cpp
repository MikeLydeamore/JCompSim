#include <iostream>
#include <utility>
#include <vector>
#include <assert.h>
#include <math.h>
#include <random>
#include <numeric>
#include <ctime>
#include <iostream>
#include <fstream>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random.hpp>
#include "StateValues.h"
#include "Transitions.cpp"
#include "Serialiser.cpp"

template<class T>
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
    double T_MAX = 500;
    std::string filename;
    //void (*mpSerialise) (double t, state_values_discrete<T> states);
    Serialiser<T> *mpSerialiser;

    void solveGillespie()
    {
        std::ofstream outputfile;
        outputfile.open(filename);

        double t = 0;
        
        int population_size = 0;
        for (typename state_values_discrete<T>::iterator it = states.begin(); it != states.end(); it++)
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

        mpSerialiser->serialiseHeader(states);

        mpSerialiser->serialise(t, states);

        while (t < T_MAX)
        {

            int actual_size = 0;
            for (typename state_values_discrete<T>::iterator it = states.begin(); it != states.end(); it++)
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

            mpSerialiser->serialise(t, states);
        }
        outputfile.close();
    }
    
    /*void solveDeterministic() {
        

        //Contains the State keys in order. state_mappings[index] will give the index in the array of states.
        std::map<int, std::string> state_mappings;
        
        int i = 0;
        for (Iterator it = states.begin(), it != states.end(); ++it) {
            state_mappings[i] = it->first;
            i++;
        }
        
    }*/

    /*void derivative(const state_array p, state_array &dpdt, const double t) {
        std::map<std::string, double> dpdt_map;
        
        int i = 0;
        for (Iterator it = p.begin(); it != p.end(); ++it) {
            //Update states with current position.
            states[state_mappings[i]] = it;

            //Initialize map to have all the relevant keys.
            dpdt_map[state_mappings[i]] = 0;

            i++;
        }

        for (Transition transition : transitions) {
            dpdt_map[transition.getSourceState()] -= transition.getRate(states);
            dpdt_map[transition.getDestinationState()] += transition.getRate(states);
        }

        //convert map back into state_array
        i = 0;
        for (Iterator it = dpdt_map.begin(); it != dpdt_map.end(); ++it) {
            dpdt[i] = it->second;
            i++;
        }

    }*/


  protected:
    state_values_discrete<T> states;

    std::vector<Transition<T> > transitions;

  public:

    void setSerialiser(Serialiser<T> *serialiser) {
        mpSerialiser = serialiser;
    }

    const static int SOLVER_TYPE_GILLESPIE = 1;

    void addState(std::string state_name, int initial_value) {
        states[state_name] = initial_value;
    }

    void addTransition(Transition<T> transition) {
        transitions.push_back(transition);
    }

    void setMaxTime(double newMaxTime) {
        T_MAX = newMaxTime;
    }

    void setOutputFile(std::string newfilename) {
        filename = newfilename;
    }

    void solve(int solver_type) {
        if (solver_type == SOLVER_TYPE_GILLESPIE) {
            solveGillespie();
        }
    }

    
};