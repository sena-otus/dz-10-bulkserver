#include "parser.h"
#include "abstractblock.h"
#include <stdexcept>
#include <iostream>
#include <string_view>
#include <utility>


Parser::Parser(unsigned N, std::unique_ptr<AbstractBlock> &&block, std::shared_ptr<AbstractBlock> globalBlock)
  : m_block(std::move(block)), m_globalBlock(std::move(globalBlock)), m_N(N)
{}



void Parser::parse(const std::string_view& line)
{
  const std::lock_guard<std::mutex> guard(m_protectparse);
  m_acculine.addNewInput(line);
  std::optional<std::string> cmd;
  while((cmd = m_acculine.getNextCmd())) {
    if(*cmd == "{") {
      m_extendedModeLevel++;
      if(m_extendedModeLevel > 1) continue;
      m_globalBlock->flush();
      continue;
    }
    if(*cmd == "}") {
      m_extendedModeLevel--;
      if(m_extendedModeLevel > 0) continue;
      if(m_extendedModeLevel < 0) {
        std::cerr << "игнорируется неожиданная скобка '}'" << std::endl;
        m_extendedModeLevel = 0;
      }
      m_block->flush();
      continue;
    }
    if(m_extendedModeLevel > 0) {
      m_block->append(*cmd);
    } else if (m_globalBlock->cmdnum() < m_N) {
      m_globalBlock->append(*cmd);
    }
    if(m_globalBlock->cmdnum() == m_N && m_extendedModeLevel == 0) m_globalBlock->flush();
  }
}




void Parser::finalize()
{
  const std::lock_guard<std::mutex> guard(m_protectparse);
  if(m_extendedModeLevel==0) {
    m_globalBlock->flush();
  }
}
