#pragma once

#include "outq.h"

/**
 * @brief bulk вывод в std::cout
 * @see Block::m_oqlist
 * @param oq output queue with  tasks
 * */

extern void stdoutwriter(OutQueueSP oq);
