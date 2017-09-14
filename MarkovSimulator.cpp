#include "MarkovChain/MarkovChain.cpp"
#include <boost/program_options.hpp>
#include <json.hpp>
#include <iostream>
#include <fstream>
//#include "MarkovChain/SerialiserFile.cpp"
#include "ModelSIR.cpp"
//#include "ModelGASScabies.cpp"
//#include "ModelSI.cpp"
//#include "ModelSIS.cpp"
//#include "ModelSIRWS.cpp"

namespace po = boost::program_options;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
  
  std::string filename;
  std::string statesFilename;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("filename,f", po::value<std::string>(&filename), "output file name")
      ("states,s", po::value<std::string>(&statesFilename), "states file name")
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
  }

  if (vm.count("states")) {
    std::ifstream states_file(statesFilename);
    json states_json;
    states_file >> states_json;
    std::cout << states_json << std::endl;
  }

  using T = double;

  MarkovChain<T> chain;
  ModelSIR<T> model;
  //ModelGASScabies model;
  //ModelSI model;
  //ModelSIS model;
  //ModelSIRWS<T> model;
  model.setupModel(chain);
  chain.setMaxTime(50);
  


  state_values<int> states;
  states["S"]=0;
  states["I"]=0;
  json j;
  for (typename state_values<int>::iterator it = states.begin(); it != states.end(); it++) {
    j[it->first] = it->second;
  }

  std::ofstream o("pretty.json");
  o << std::setw(4) << j << std::endl;

  SerialiserFile<T> serialiser(filename);

  chain.setSerialiser(&serialiser);

  chain.solve(chain.SOLVER_TYPE_GILLESPIE);

  return 0;
}
