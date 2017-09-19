#include "MarkovChain/MarkovChain.cpp"
#include <boost/program_options.hpp>
#include <json.hpp>
#include <iostream>
#include <fstream>
#include "MarkovChain/json/ModelJson.cpp"
#include "ModelSIRWS.cpp"

namespace po = boost::program_options;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
  
  std::string filename;
  std::string statesFilename;
  std::string transitionsFilename;
  std::string serialiserType;
  double max_t = 50;
  double dt = 0.001;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("filename,f", po::value<std::string>(&filename), "output file name")
      ("states,s", po::value<std::string>(&statesFilename), "states file name")
      ("transitions,t", po::value<std::string>(&transitionsFilename), "transitions file name")
      ("serialiser", po::value<std::string>(&serialiserType), "serialiser type")
      ("maxt", po::value<double>(&max_t), "maximum simulation time")
      ("dt", po::value<double>(&dt), "time step for serialising")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
  }

  if (!vm.count("filename")) {
    std::cout << "filename must be provided" << std::endl;
    return (-1);
  }

  using T = int;

  if (vm.count("states") && vm.count("transitions")) {
    std::ifstream states_file(statesFilename);
    json states_json;
    states_file >> states_json;

    std::ifstream transitions_file(transitionsFilename);
    json transitions_json;
    transitions_file >> transitions_json;

    
    MarkovChain<T> chain;
    chain.setMaxTime(max_t);

    SerialiserFile<T> serialiser(filename);
    SerialiserFileFinalState<T> serialiserFileFinal(filename);

    std::vector<double> serialiser_times(max_t/dt + 1);
    
    double n = {-1*dt};
    std::generate(serialiser_times.begin(), serialiser_times.end(), [&n, dt]{ return n += dt; });
    SerialiserPredefinedTimesFile<T> serialiserPredefinedTimesFile(serialiser_times, filename);
    if (serialiserType == "full") {
      chain.setSerialiser(&serialiser);
    }
    else if (serialiserType == "final") {
      chain.setSerialiser(&serialiserFileFinal);
    }
    else if (serialiserType == "points") {
      chain.setSerialiser(&serialiserPredefinedTimesFile);
    }
    else {
      chain.setSerialiser(&serialiser);
    }

    ModelJson<T> model(states_json, transitions_json);
    model.setupModel(chain);
    
    chain.solve(chain.SOLVER_TYPE_GILLESPIE);
    return 0;
  }
  
  MarkovChain<T> chain;
  ModelSIRWS<T> model;
  SerialiserFile<T> serialiser2(filename);
  model.setupModel(chain);
  chain.setMaxTime(100);
  chain.setSerialiser(&serialiser2);

  chain.solve(chain.SOLVER_TYPE_GILLESPIE);

  return 0;
}
