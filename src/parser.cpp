#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <string_view>


Parser::Parser(unsigned N, std::unique_ptr<AbstractBlock> &&block)
  : m_block(std::move(block)), m_N(N)
{}



void Parser::parse(const std::string_view& line)
{
  const std::lock_guard<std::mutex> guard(m_protectparse);
  m_acculine.addNewInput(line);
  std::optional<std::string> cmd;
  while((cmd = m_acculine.getNextCmd()))
  {
    if(*cmd == "{")
    {
      m_extendedModeLevel++;
      if(m_extendedModeLevel > 1) continue;
      m_block->flush();
      continue;
    }
    if(*cmd == "}")
    {
      m_extendedModeLevel--;
      if(m_extendedModeLevel > 0) continue;
      if(m_extendedModeLevel < 0) throw std::runtime_error("лишняя скобка } в строке " + std::to_string(m_lineno));
      m_block->flush();
      continue;
    }
    if(m_block->cmdnum() < m_N || m_extendedModeLevel > 0) m_block->append(*cmd);
    if(m_block->cmdnum() == m_N && m_extendedModeLevel == 0) m_block->flush();
  }
}




void Parser::finalize()
{
    // вообще здесь не нужна защита, потому что finalize() никогда не
    // вызывается параллельно с parse()
  const std::lock_guard<std::mutex> guard(m_protectparse);
  if(m_extendedModeLevel==0)
  {
    m_block->flush();
  }
}
