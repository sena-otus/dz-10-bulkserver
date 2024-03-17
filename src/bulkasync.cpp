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
    m_worker_cw (std::thread(stdoutwriter,m_qcout)), // start single cout writer thread
    m_nextuid(0)
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
  const std::lock_guard<std::shared_mutex> lock(m_mapmutex);
  auto blockptr = std::make_unique<Block>(m_nextuid,
                                          Block::wlist_t{m_qfile, m_qcout},
                                          getTime);
  m_nextuid++;
  parserptr_t parser = std::make_unique<Parser>(N, std::move(blockptr));
  auto *rawptr = parser.get();
  m_parser.emplace(rawptr, std::move(parser));
  return rawptr;
}

void
BulkAsync::receive(void *ptr, const char *buf, size_t size)
{
    // залочить мютекс чтобы другой поток не мог вызвать disconnect(ptr)
    // но использовать shared_lock чтобы разрешить вызов других receive()
  const std::shared_lock<std::shared_mutex> lock(m_mapmutex);
  auto *parser = static_cast<Parser*>(ptr);
  parser->parse(std::string_view(buf, size));
}

void
BulkAsync::receive_checked(void *ptr, const char *buf, size_t size)
{
  const std::shared_lock<std::shared_mutex> lock(m_mapmutex);
    // исполнить только если ptr это "наш" ptr, сохранённый в m_parser
  auto it = m_parser.find(ptr);
  if(it!= m_parser.end() ) {
    it->second->parse(std::string_view(buf, size));
  }
}

void BulkAsync::disconnect(void *ptr)
{
  const std::lock_guard<std::shared_mutex> lock(m_mapmutex);
  auto pit = m_parser.find(ptr);
  if(pit != m_parser.end()) {
    pit->second->finalize();
    m_parser.erase(pit);
  }
}

void BulkAsync::closeAll()
{
  {
    const std::lock_guard<std::shared_mutex> lock(m_mapmutex);
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
