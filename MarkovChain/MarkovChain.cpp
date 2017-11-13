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
#include <boost/numeric/odeint.hpp>
#include <boost/operators.hpp>
#include "StateValues.h"
#include "Transitions.cpp"
#include "Serialiser.cpp"
#include <functional>

namespace pl = std::placeholders;

namespace Deterministic {
    
        class State : boost::additive1<State,
                          boost::additive2<State, double, 
                              boost::multiplicative2<State, double> > >
        {
          public:
            using Map = std::map<std::string, double>;
            State(Map const& map) : mMap(map) {}
            State() = default;
            State(const State &p) = default;
            State &operator=(State const&a) = default;
            
            void addToKey(std::string key, double a)
            {
                mMap[key] += a;
            }

            State &operator+=(const State &p) {
                for (auto& p : p.mMap) mMap[p.first] += p.second;
                return *this;
            }
    
            State &operator+=(double a) {
                for (auto& p : mMap)
                    p.second += a;
                return *this;
            }
    
            State &operator*=(double f) {
                for (auto& p : mMap) mMap[p.first] *= f;
                return *this;
            }
    
            friend State abs(const State &p) {
                using std::abs;
                auto map = p.mMap;
    
                for(auto& e : map)
                    e.second = abs(e.second);
    
                return map;
            }
    
            friend State operator/(const State &p1, const State &p2) {
                auto map = p1.mMap;
    
                for(auto& e : map)
                    e.second /= p2.mMap.at(e.first);
    
                return map;
            }
    
            friend double vector_space_norm_inf_impl(State const& p) {
                double max = 0;
                using std::abs;
                for (auto& el : p.mMap)
                    max = std::max(abs(el.second), max);
                return max;
            }
    
            size_t size() const { return mMap.size(); }
    
            void resize(State const& other) {
                for (auto& el : other.mMap)
                    mMap[el.first] += 0; // inserts if non-existent
            }

            Map getMap() const
            {
                return (mMap);
            }
    
          private:
            Map mMap;
        };
    }
    
using DeterministicStateType = Deterministic::State;

namespace boost { namespace numeric { namespace odeint {
    template <> struct vector_space_norm_inf<DeterministicStateType> {
        typedef double result_type;
        double operator()(const DeterministicStateType &p) const { return vector_space_norm_inf_impl(p); }
    };

    template <> struct is_resizeable<DeterministicStateType> {
        typedef boost::true_type type;
        const static bool value = type::value;
    };

    template <> struct same_size_impl<DeterministicStateType, DeterministicStateType> {
        static bool same_size(const DeterministicStateType &v1, const DeterministicStateType &v2) {
            return v1.size() == v2.size();
        }
    };

    template <> struct resize_impl<DeterministicStateType, DeterministicStateType> {
        static void resize(DeterministicStateType &v1, const DeterministicStateType &v2) {
            v1.resize(v2);
        }
    };
} } }

using namespace boost::numeric::odeint;

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
    Serialiser<T> *mpSerialiser;

   

    void solveGillespie()
    {
        double t = 0;
        bool ended_infinite = false;
        
        int population_size = 0;
        for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
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

        while (t < T_MAX && !ended_infinite)
        {

            int actual_size = 0;
            for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
            {
                assert(it->second >= 0);
                actual_size += it->second;
            }
            //assert(actual_size == population_size);

            std::vector<double> rates(transitions.size());
            for (int i = 0; i < transitions.size(); i++)
            {
                rates[i] = transitions[i]->getRate(states);
                if (rates[i] < 0.0 || isnan(rates[i]))
                {
                    std::cout << "Transition from " << transitions[i]->getSourceState() << " to " << transitions[i]->getDestinationState() << " has rate " << rates[i] << std::endl;
                    exit(-1);
                }
                assert(rates[i] >= 0);
            }
            double rates_sum = std::accumulate(rates.begin(), rates.end(), (double)0.0);

            double event_time = -(1.0 / rates_sum) * log(runif());
            if (isinf(event_time))
            {
                ended_infinite = true;
                break;
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
            transitions[eventOccurred]->do_transition(states);
            mpSerialiser->serialise(t, states);
        }
        mpSerialiser->serialiseFinally(t, states);
    }

    
    void derivative(const DeterministicStateType p, DeterministicStateType &dpdt, const double t) 
    {
        for (int i = 0 ; i < transitions.size(); i++) 
        {
            dpdt.addToKey(transitions[i]->getSourceState(), -1*transitions[i]->getRate(p.getMap()));
            dpdt.addToKey(transitions[i]->getDestinationState(), transitions[i]->getRate(p.getMap()));
        }
    }

    void serialiserDeterministic(const DeterministicStateType &p, const double t)
    {
        mpSerialiser->serialise(t, p.getMap());
    }

    void solveDeterministic() 
    {
        DeterministicStateType x0(states);
        typedef runge_kutta_cash_karp54<DeterministicStateType, double, DeterministicStateType, double, vector_space_algebra>
            rkck54;

        //typedef controlled_runge_kutta< rkck54, double, DeterministicStateType, double, vector_space_algebra > ctrl_rkck54;
        mpSerialiser->serialiseHeader(x0.getMap());
        integrate_adaptive(make_controlled(1e-10, 1e-6, rkck54()), std::bind(&MarkovChain::derivative, *this , pl::_1 , pl::_2 , pl::_3 ), x0, 0.0,
                           T_MAX, 0.1,
                        std::bind(&MarkovChain::serialiserDeterministic, *this, pl::_1, pl::_2));

        mpSerialiser->serialiseFinally(T_MAX, x0.getMap());
    }

    void solveRK4()
    {
        DeterministicStateType y(states);
        mpSerialiser->serialiseHeader(y.getMap());
        double t = 0;
        double h = 1.0/120;
        if (T_MAX < 5)
            h = 1.0/(5000*T_MAX);
        while (t < T_MAX)
        {
            mpSerialiser->serialise(t, y.getMap());
            DeterministicStateType k_1;
            DeterministicStateType k_2;
            DeterministicStateType k_3;
            DeterministicStateType k_4;
            derivative(y, k_1, t);
            derivative(y+(h/2)*k_1, k_2, t+(h/2));
            derivative(y+(h/2)*k_2, k_3, t+(h/2));
            derivative(y+h*k_3, k_4, t+h);
            
            y += (h/6)*(k_1+2*k_2+2*k_3+k_4);
            t += h;
        }

        mpSerialiser->serialiseFinally(t, y.getMap());
    }

    void solveForwardEuler()
    {
        DeterministicStateType y0(states);

        double t = 0;
        double h = 1.0/120.0;
        mpSerialiser->serialiseHeader(y0.getMap());
        while (t < T_MAX)
        {
            mpSerialiser->serialise(t, y0.getMap());
            DeterministicStateType dpdt;
            derivative(y0, dpdt, t);
            //std::cout << y0.getMap()["S"] << std::endl;
            y0 += h * dpdt;

            t += h;
        }

        mpSerialiser->serialiseFinally(t, y0.getMap());
    }


  protected:
    state_values<T> states;

    std::vector<Transition<T>* > transitions;

  public:

    void setSerialiser(Serialiser<T> *serialiser) {
        mpSerialiser = serialiser;
    }

    const static int SOLVER_TYPE_GILLESPIE = 1;

    void addState(std::string state_name, T initial_value) {
        states[state_name] = initial_value;
    }

    void addTransition(Transition<T> *transition) {
        double two_decimals = round(100/transition->getSingleParameter())/100;
        std::cout << ("Adding transition from " + transition->getSourceState() + " to " 
        + transition->getDestinationState() + " at rate " + std::to_string(transition->getSingleParameter()) 
        + " (1/") << std::setprecision(2) << std::fixed << two_decimals << ")" << std::endl;
        transitions.push_back(transition);
    }

    void setMaxTime(double newMaxTime) {
        T_MAX = newMaxTime;
    }

    void solve(int solver_type) {
        if (solver_type == SOLVER_TYPE_GILLESPIE) {
            solveGillespie();
        } else
        {
            solveRK4();
        }
    }

    
};

