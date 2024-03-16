#include "stdoutwriter.h"
#include <iostream>

  // NOLINTNEXTLINE(performance-unnecessary-value-param)
void stdoutwriter(OutQueueSP oq)
{
  while (true) {
    const OutQueue::Task task = oq->fetch();
    if(task.m_exit)
    {
      return;
    }
    std::cout << "bulk: " << task.m_str
              << std::endl; // std::endl to flush the buffer
  }
}
