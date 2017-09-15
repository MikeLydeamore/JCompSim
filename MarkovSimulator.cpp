#include "MarkovChain/MarkovChain.cpp"
#include <boost/program_options.hpp>
#include <json.hpp>
#include <iostream>
#include <fstream>
//#include "MarkovChain/SerialiserFile.cpp"
//#include "ModelSIR.cpp"
#include "MarkovChain/json/ModelJson.cpp"
//#include "ModelGASScabies.cpp"
//#include "ModelSI.cpp"
//#include "ModelSIS.cpp"
//#include "ModelSIRWS.cpp"

namespace po = boost::program_options;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
  
  std::string filename;
  std::string statesFilename;
  std::string transitionsFilename;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("filename,f", po::value<std::string>(&filename), "output file name")
      ("states,s", po::value<std::string>(&statesFilename), "states file name")
      ("transitions,t", po::value<std::string>(&transitionsFilename), "transitions file name")
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
    chain.setMaxTime(50);
    SerialiserFile<T> serialiser(filename);
    chain.setSerialiser(&serialiser);

    ModelJson<T> model(states_json, transitions_json);
    model.setupModel(chain);
    
    chain.solve(chain.SOLVER_TYPE_GILLESPIE);
    
    return 0;
  }

  return 0;
}
