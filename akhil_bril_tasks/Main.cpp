#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "CFG.h"


using namespace std;
using json = nlohmann::json;

/*#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

int main(int argc, char *argv[])
{

}*/

int main(int argc, char *argv[])
{
  if (argc < 2 ) {
    std::cout << "No input file provided " << std::endl;
  }

  std::ifstream inFile(argv[1]);
  json inputJson;
  //inFile >> inputJson;
  inputJson = json::parse(inFile);

  std::map<std::string, shared_ptr<BrilOpt::CFG>> funcCfgMap;

  for (auto it = inputJson["functions"].begin(); it != inputJson["functions"].end(); it++)
  {
    json function = *it;
    shared_ptr<BrilOpt::CFG> newCFG = std::make_shared<BrilOpt::CFG>(function);
    funcCfgMap[function["name"]] = newCFG;
    newCFG->CreateCFG();
  }

  //json functions = inputJson["functions"].at(0);
  //std::cout << "functions " << functions << "\n";

  //json instrs = functions["instrs"];
  //std::cout << instrs << "\n";


  /*for (auto& instr : instrs)
  {
    std::cout << instr;
    std::cout << endl;
  }

  BrilOpt::CFG cfg(inputJson);
  cfg.CreateCFG();*/

}
