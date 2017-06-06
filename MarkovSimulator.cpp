#include <iostream>
#include <map>
#include <utility>

std::map<std::string, int> states;

double t = 0;

const double T_MAX = 1000; //Arbitrary really

void setup() {
  states["S"] = 99;
  states["I"] = 1;
  states["R"] = 5;
}

void serialise(int pT, std::map<std::string, int> pStates) {
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
  for(std::map<std::string,int>::iterator it=states.begin(); it != states.end(); it++) {
    std::cout << "," << it->first;
  }
  std::cout << "\n";

  serialise(t, states);

  while (t < T_MAX) {

  }
  return 0;
}
