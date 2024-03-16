#include "filewriter.h"
#include "outq.h"

#include <boost/format.hpp>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

  // NOLINTNEXTLINE(performance-unnecessary-value-param)
void filewriter(OutQueueSP oq) {
  while (true) {
    try {
      const OutQueue::Task task = oq->fetch();
      if (task.m_exit) {
        return;
      }

      // std::ofstream не позволяет создать файл экслюзивно (до c++23)
      // поэтому сначала создаём файл с помощью std::fopen(name,"wx")
      // ::open будет работать тоже, но его нет в стандарте c++
      {
        auto fp = std::unique_ptr<FILE, decltype(&fclose)>(std::fopen(task.m_fname.c_str(), "wx"), &fclose);
        if (!fp) throw std::runtime_error("ошибка создания файла '" + task.m_fname + "'");
      }
      {
        std::ofstream ofs(task.m_fname);
        if (!ofs.is_open()) throw std::runtime_error("ошибка открытия файла " + task.m_fname);
        ofs << "bulk: " << task.m_str << "\n";
        ofs.close(); // чтобы проверить ошибки записи
        if (!ofs) throw std::runtime_error("ошибка записи в файл " + task.m_fname);
      }
    } catch (std::exception &se) {
        // просто вывести в стандартный поток ошибок и продолжить
      std::cerr << se.what() << std::endl;
    }
  }
}
