# JCompSim
JCompSim is a JSON-powered C++ developed tool for rapidly prototyping and simulating compartmental mathematical models.

JCompSim allows the user to specify their model using JSON, and then supply those models to JCompSim for solving in either a stochastic or deteministic context without the need to write or change any C++ code.

For more information about implementing your own models, check the [Wiki](https://github.com/MikeLydeamore/JCompSim/wiki).

To download, head to the [Releases](https://github.com/MikeLydeamore/JCompSim/releases) page, or clone and compile using:
`clang++ -std=c++11 -v  -I./MarkovChain/ -L./staticlibs/ -lboost_program_options -o JCompSim MarkovSimulator.cpp`

Note that compiling from source requires an up-to-date install of Boost.