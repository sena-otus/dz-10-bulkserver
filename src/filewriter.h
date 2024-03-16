#pragma once

#include "outq.h"

/**
 * @brief bulk вывод в файл
 * @see Block::oqlist
 * @param oq output queue with  tasks
 * */
extern void filewriter(OutQueueSP oq);
