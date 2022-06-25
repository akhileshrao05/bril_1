
#include <fstream>
#include <iostream>


#include "CFG.h"

using namespace std;

namespace BrilOpt {

  void BasicBlock::AddInstr(json instr) {
        std::cout << "In AddInstr instr: " << instr << std::endl;

        std::shared_ptr<Instr> newInstr = std::make_shared<Instr>(instr);

        m_instrs.push_back(newInstr);
  }

  void BasicBlock::AddPred(std::shared_ptr<BasicBlock> pred) {
    m_predecessors.push_back(pred);
  }

  void BasicBlock::AddSuccessor(std::shared_ptr<BasicBlock> succ) {
    m_successors.push_back(succ);
  }

  void BasicBlock::SetThenSuccessor(std::shared_ptr<BasicBlock> succ) {
    m_thenSucc = succ;
  }

  void BasicBlock::SetElseSuccessor(std::shared_ptr<BasicBlock> succ) {
    m_elseSucc = succ;
  }

  void BasicBlock::print() {
    for (auto instr : m_instrs){
      instr->print();
    }
  }


  void CFG::CreateCFG()
  {
    std::cout << "In CreateCFG" << std::endl;

    std::shared_ptr<BasicBlock> curBlock;
    /*for (auto function : m_inputJson["functions"])
    {
      std::cout << "function:\n";
      std::cout << function << std::endl;
      CreateBlocks(function);
    }*/

    CreateBlocks(m_function);
    LinkBlocks();

    std::cout << "####### Printing Blocks ###########\n";
    for ( auto const x : m_labelToBlockMap)
    {
      auto block = x.second;
      block->print();
    }
  }

  bool IsTerminatorInstr(json instr) {
    //Weird error in nlohman json.
    //When an instr with no "label" is passed to this function,
    // "label" field becomes defined and is = nullptr
    //Hence the weird label field comparison.

    std::cout << "IsTerminatorInstr instr " << instr << std::endl;
    if (instr["label"]==nullptr)
      std::cout << "Instr is not terminator\n";

    if (instr["op"] == "jmp" || instr["op"] == "br" || instr["op"] == "ret" || instr["label"]!=nullptr ) {
      std::cout << "IsTerminatorInstr return true \n";
      return true;
    }

    std::cout << "IsTerminatorInstr return false \n ";

    return false;
  }

  bool IsConditionalBranch(json instr) {
    if (instr["opcode"] == "br")
      return true;

    return false;
  }

  bool IsLabel(json instr) {
    if (instr["label"] != "")
      return true;

    return false;
  }


  void CFG::CreateBlocks(json function)
  {
    std::shared_ptr<BasicBlock> curBlock = std::make_shared<BasicBlock>();
    m_basicBlocks.push_back(curBlock);
    std::string label;
    if (function["instrs"][0].contains("label"))
      label = function["instrs"][0]["label"];
    else
      label = "b." + std::to_string(m_basicBlocks.size());

    m_labelToBlockMap[label] = curBlock;


    for (auto it = function["instrs"].begin(); it != function["instrs"].end(); it++)
    {
      auto instr = *it;

      std::cout << "Processing instr: " << instr << std::endl;
      if(instr.contains("label"))
        std::cout << "Instr contains label\n";
      else
        std::cout << "Instr DOES NOT contains label\n";

      //Done with function, return
      if (instr["op"] == "ret")
      {
        curBlock->AddInstr(instr);
        return;
      }

      if(IsTerminatorInstr(instr))
      {
        std::shared_ptr<BasicBlock> prevBlock;
        prevBlock = curBlock;
        curBlock = std::make_shared<BasicBlock>();
        m_basicBlocks.push_back(curBlock);

          if (!instr.contains("label"))
          {
            //if current bloc ends with a terminator instr
            //i.e not a fall through
            prevBlock->AddInstr(instr);
            std::cout << "Adding instr: " << instr << " to block: " << label << std::endl;
            // Assuming all blocks start with a label unless it is the first block of a function
            it++;
            instr = *it;
            if (instr.contains("label"))
              label = instr["label"];
            else
              label = "b." + std::to_string(m_basicBlocks.size());

          }
          else
          {
            //if current block ends with a fall through
            label = instr["label"];
            prevBlock->AddSuccessor(curBlock);
            //curBlock->AddInstr(instr);
            //std::cout << "Adding instr: " << instr << " to block: " << curBlock << std::endl;
          }
          m_labelToBlockMap[label] = curBlock;
        }
        else
        {
          std::cout << "Adding instr: " << instr << " to block: " << curBlock << std::endl;
          curBlock->AddInstr(instr);
        }
      }


    }

  bool CFG::LinkBlocks()
  {
    std::cout << "LinkBlocks\n";

    for ( auto const x : m_labelToBlockMap)
    {
      std::string label = x.first;
      std::shared_ptr<BasicBlock> curBlock = x.second;
      std::cout << "Processing block with label:" << label << std::endl;
      std::cout << "IsEmpty: " << curBlock->isBlockEmpty() << std::endl;
      if (curBlock->isBlockEmpty())
        continue;
      json lastInstr = (curBlock->GetLastInstr())->m_instrBody;
      std::string opcode = lastInstr["op"];
      std::cout << opcode << std::endl;

      if (opcode == "br")
      {
        std::string thenLabel = lastInstr["labels"][0];
        std::string elseLabel = lastInstr["labels"][1];

        if (m_labelToBlockMap.find(thenLabel) == m_labelToBlockMap.end()) {
          std::cout << "########## ERROR: label " << thenLabel << " not found in block map\n";
          return false;
        }
        if (m_labelToBlockMap.find(elseLabel) == m_labelToBlockMap.end()) {
          std::cout << "########## ERROR: label " << elseLabel << " not found in block map\n";
          return false;
        }

        auto thenSucc = m_labelToBlockMap[thenLabel];
        auto elseSucc = m_labelToBlockMap[elseLabel];

        std::cout << "Found thenSucc block with label:" << thenLabel << std::endl;
        std::cout << "Found elseSucc block with label:" << elseLabel << std::endl;

        curBlock->SetThenSuccessor(thenSucc);
        curBlock->SetElseSuccessor(elseSucc);

        curBlock->AddSuccessor(thenSucc);
        curBlock->AddSuccessor(elseSucc);

        thenSucc->AddPred(curBlock);
        elseSucc->AddPred(curBlock);
      }
      else if (opcode == "jmp")
      {
        std::string jmpLabel = lastInstr["labels"][0];
        std::cout << jmpLabel << std::endl;

        if (m_labelToBlockMap.find(jmpLabel) == m_labelToBlockMap.end()) {
          std::cout << "########## ERROR: label " << jmpLabel << " not found in block map\n";
          return false;
        }

        std::cout << "Found jmpSucc block with label:" << jmpLabel << std::endl;
        auto jmpSucc = m_labelToBlockMap[jmpLabel];
        curBlock->AddSuccessor(jmpSucc);

        jmpSucc->AddPred(curBlock);

      }
    }
    return true;
  }






  /*bool CFG::_CreateCFG(std::shared_ptr<BasicBlock> curBlock, json::iterator& it)
  {

    if (it == m_inputJson.end())
      return true;

    if (curBlock == nullptr) {
      std::cout << "######## CreateCFG creating new block\n";
      curBlock = std::make_shared<BasicBlock>();
    }

    m_basicBlocks.push_back(curBlock);

    while (it != m_inputJson.end())
    {
      json instr = *it;
      std::cout << "In _CreateCFG instr: " << *(it) << std::endl;
      std::cout << "In _CreateCFG curBlock " << curBlock << std::endl;
      curBlock->AddInstr(instr);

      if (IsTerminatorInstr(instr))
      {

          if (IsConditionalBranch(instr)) {
            curBlock->m_isTerminatorCondBranch = true;

            std::shared_ptr<BasicBlock> thenSucc = std::make_shared<BasicBlock>();
            std::shared_ptr<BasicBlock> elseSucc = std::make_shared<BasicBlock>();

            curBlock->AddSuccessor(thenSucc);
            curBlock->AddSuccessor(elseSucc);

            curBlock->SetThenSuccessor(thenSucc);
            curBlock->SetElseSuccessor(elseSucc);

            it++;
            if (_CreateCFG(thenSucc, it))
              return true;
            it++;
            if (_CreateCFG(elseSucc, it))
              return true;

          }
          else {
            std::shared_ptr<BasicBlock> newBlock = std::make_shared<BasicBlock>();
            curBlock->AddSuccessor(newBlock);

            // A label shows up as an instr in the json
            // So skip the label to continue with the next actual instr
            if (!IsLabel(instr)) {
              it++;
              json nextInstr = *it;
              newBlock->AddInstr(nextInstr);
            }

            it++;
            if (_CreateCFG(newBlock, it))
              return true;
          }
        }
        it++;

      }
      std::cout << "returning \n";
      return true;

    }*/

};
