
#include <fstream>
#include <iostream>
#include <cstdio>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace BrilOpt {

class Instr{

  public:
  /*std::string m_dest;
  std::string m_opcode;
  std::string m_dataType;
  int m_value;
  std::vector<std::string> m_args;
  std::vector<std::string> m_labels;
  bool m_isBinaryOp;*/

  json m_instrBody;



  Instr(json instr):m_instrBody(instr) {}

  /*Instr(std::string opcode, std::string dest, std::string dataType, int value, std::vector<std::string> args, std::vector<std::string> labels): m_opcode(opcode),
  m_dest(dest), m_dataType(dataType), m_value(value), m_args(args), m_labels(labels){};

  Instr(const Instr& other): m_opcode(other.m_opcode), m_dest(other.m_dest), m_dataType(other.m_dataType), m_value(other.m_value),
  m_args(other.m_args), m_labels(other.m_labels){};*/

  bool IsTerminatorInstr()
  {
    if ( m_instrBody["op"]== "br" || m_instrBody["op"] == "jmp")
      return true;
    return false;
  }

  void print() { std::cout << m_instrBody << std::endl ; }

};

class BasicBlock {
  private:
  std::string m_label;
  std::vector<std::shared_ptr<Instr>> m_instrs;
  std::vector<std::shared_ptr<BasicBlock>> m_successors;
  std::vector<std::shared_ptr<BasicBlock>> m_predecessors;


  std::shared_ptr<BasicBlock> m_thenSucc;
  std::shared_ptr<BasicBlock> m_elseSucc;

  public:

  BasicBlock() {
    m_label = "";
  }

  bool m_isTerminatorCondBranch;
  void AddInstr(json instr);
  void AddPred(std::shared_ptr<BasicBlock> pred);
  void AddSuccessor(std::shared_ptr<BasicBlock> succ);
  std::shared_ptr<Instr> GetLastInstr() { return m_instrs[m_instrs.size()-1];}
  void SetThenSuccessor(std::shared_ptr<BasicBlock> succ);
  void SetElseSuccessor(std::shared_ptr<BasicBlock> succ);
  bool isBlockEmpty() { return m_instrs.empty(); }
  void print();


};

class CFG {

  private:
  json m_function;
  std::vector<std::shared_ptr<BasicBlock>> m_basicBlocks;

  bool _CreateCFG(std::shared_ptr<BasicBlock> curBlock, json::iterator& it);
  std::map<std::string, std::shared_ptr<BasicBlock>> m_labelToBlockMap;

  public:
  CFG(json function): m_function(function) {}
  void CreateCFG();
  void CreateBlocks(json function);
  bool LinkBlocks();
};


};
