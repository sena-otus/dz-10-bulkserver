#include "outq.h"

using std::lock_guard;

void OutQueue::put(Task &&task) {
  {
    const lock_guard<std::mutex> lg(m_mutex);
    m_oq.emplace(task);
  }
  m_event.notify_one();
}

void OutQueue::putExit()
{
  m_exitFlag = true;
  m_event.notify_all();
}


OutQueue::Task OutQueue::fetch()
{
  std::unique_lock<std::mutex> ul(m_mutex);
  m_event.wait(ul, [this]{return !m_oq.empty() || m_exitFlag;});
  if(m_oq.empty()) {
    return Task{"", "", true};
  }
  auto task = m_oq.front();
  m_oq.pop();
  return task;
}
