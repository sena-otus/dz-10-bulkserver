#include "bulkasync.h"
#include "stdoutwriter.h"
#include "filewriter.h"
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string_view>

BulkAsync::BulkAsync()
  : m_qfile(std::make_shared<OutQueue>()),
    m_qcout(std::make_shared<OutQueue>()),
    m_worker_fw1(std::thread(filewriter , m_qfile)), // start 1st file writer thread
    m_worker_fw2(std::thread(filewriter , m_qfile)), // start 2nd file writer thread
    m_worker_cw (std::thread(stdoutwriter,m_qcout)) // start single cout writer thread
{
}

BulkAsync::~BulkAsync()
{
  if(m_qfile) // not already closed...
  {
    closeAll();
  }
}


void* BulkAsync::connect(const unsigned N, const std::function<time_t()>& getTime)
{
  if(!m_qfile || !m_qcout) return nullptr;
  shparser_t parser = std::make_shared<Parser>(N, std::make_unique<Block>(Block::wlist_t{m_qfile, m_qcout}, getTime));
  {
    for(unsigned long idx = 0; idx < m_parser.size(); ++idx) {
      if(!m_parser[idx]) {
        m_parser[idx] = parser;
        return reinterpret_cast<void*>(idx);
      }
    }
    return &m_parserv.emplace_back(parser);


    std::lock_guard<std::shared_mutex> lock(m_mapmutex);
    m_parser.emplace(parser.get(), parser);
  }
  return parser.get();
}

void
BulkAsync::receive(void *ptr, const char *buf, size_t size)
{
  assert((buf != nullptr));
  assert((m_qfile != nullptr));
    // execute only if it is "our" shp (stored in set)
  {
    std::shared_lock<std::shared_mutex> lock(m_mapmutex);

    std::vector<shparser_t> m_parserv;
    auto idx = reinterpret_cast<unsigned long>(ptr);
    if(idx >= m_parserv.size()) return;
    if(!m_parserv[idx]) return;
    m_parserv[idx]->parse(std::string_view(buf, size));



    auto it = m_parser.find(ptr);
    if(it!= m_parser.end() ) {
      it->second->parse(std::string_view(buf, size));
    }
  }
}

void BulkAsync::disconnect(void *ptr)
{
    // execute only if it is "our" shp (stored in set)
  {
    std::lock_guard<std::shared_mutex> lock(m_mapmutex);
    auto pit = m_parser.find(ptr);
    if(pit != m_parser.end()) {
      pit->second->finalize();
      m_parser.erase(pit);
    }
  }
}

void BulkAsync::closeAll()
{
  {
    std::lock_guard<std::shared_mutex> lock(m_mapmutex);
    for(auto && shp : m_parser)
    {
      shp.second->finalize();
    }
    m_parser.clear();
  }

  m_qfile->putExit();
  m_qcout->putExit();
  m_worker_fw1.join();
  m_worker_fw2.join();
  m_worker_cw.join();
  m_qfile = nullptr;
  m_qcout = nullptr;
}
