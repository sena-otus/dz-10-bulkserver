#pragma once

#include <string>
#include <ctime>
#include <queue>
#include <condition_variable>
#include <thread>

/**
 * @brief Очередь заданий на вывод с поддержкой многопоточности
 * */
class OutQueue
{
public:
    /** @brief Задание на вывод */
  struct Task
  {
    std::string m_str; ///<! строка с командами для вывода
    std::string m_fname; ///<! имя файла для вывода
    bool m_exit;       ///<! true - игнорировать содержимое и выйти
  };

    /** @brief добавить задание в очередь */
  void put(Task && task);

    /** @brief взять задание из очереди, может блокировать исполнение если очередь пуста */
  Task fetch();

    /**
     * @brief взвести флаг на выход
     * @see m_exitFlag */
  void putExit();

private:
  std::condition_variable m_event;
  std::mutex m_mutex;
  std::queue<Task> m_oq; ///<! очередь заданий
  bool m_exitFlag{false}; ///<! флаг сигнализирует что надо завершиться
};

using OutQueueSP = std::shared_ptr<OutQueue>;
