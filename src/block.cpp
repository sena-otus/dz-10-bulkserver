#include "block.h"
#include "outq.h"
#include <boost/format.hpp>
#include <algorithm>
#include <string>

Block::Block(
  unsigned puid, wlist_t &&wlist,
  std::function<time_t()> getTime)
  : m_puid(puid),
    m_wlist(std::move(wlist)),
    m_getTime(std::move(getTime))
{}

void Block::append(const std::string_view &line) {
  if (m_cmdnum == 0) {
    m_str = line;
    const time_t newts = m_getTime();
    if (newts == m_timestamp) { // same timestamp like in previous file
      m_buid++;                 // increase suffix id
    } else {                    // no suffix
      m_buid = 0;
    }
    m_timestamp = newts;
  } else {
    m_str += ", " + std::string(line);
  }
  m_cmdnum++;
}

[[nodiscard]] unsigned Block::cmdnum() const { return m_cmdnum; }

void Block::flush() {
  if (m_cmdnum > 0) {
    std::string fname;
    fname = str(boost::format("bulk%d-%03d-%03d.log"     ) % m_timestamp % m_puid % m_buid);

    OutQueue::Task task{m_str, fname, false};

    std::for_each(m_wlist.begin(), m_wlist.end(),
                  [&](OutQueueSP &outqptr){outqptr->put(task);});
    m_cmdnum = 0;
  }
}
