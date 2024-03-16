#pragma once

#include "outq.h"
#include "block.h"
#include "parser.h"

#include <shared_mutex>
#include <thread>
#include <map>

/** @brief Интерфейс к библиотеке async */
class BulkAsync final
{
public:
  using parserptr_t = std::unique_ptr<Parser>;
  explicit BulkAsync();
  ~BulkAsync();

    /** \defgroup m1 методы необходимые из-за объявления деструктора
     *  @{ */
  BulkAsync(BulkAsync &&) = default;
  BulkAsync(const BulkAsync &) = delete;
  BulkAsync& operator=(const BulkAsync&) = delete;
  BulkAsync& operator=(BulkAsync&&) = default;
    /** @}*/

    /**
     * @brief Создаёт контекст для обработки потока команд. Может вызываться
     * несколько раз.
     * Потокобезопасность: может безопасно вызываться из разных потоков.
     * @param N размер блока команд
     * @param getTime функция для генерации временных меток
     * @return контекст потока команд
     * */
  void* connect(unsigned N, const std::function<time_t()> &getTime = [](){ return std::time(nullptr);});


    /**
     * @brief Обработка очередной команды.
     * Ради скорости никакой проверки на корректность указателя ptr не
     * делается. @see receive_checked() для варианта с проверками.
     * Потокобезопасность: может безопасно вызываться из разных потоков.
     * @param void контекст потока команд
     * @param buf указатель на буфер с командой
     * @param size размер буфера
     * */
  void receive(void *ptr, const char *buf, size_t size);

    /**
     * @brief Обработка очередной команды
     * Этот вариант receive() проверяет что ptr указывает на
     * существующий контекст. Если конекст уже разрушен или указатель
     * неверен, то вызов будет проигнорирован. Проверка осуществляется с
     * помощью поиска по map m_parser. Другой вариант быстрого вызова
     * с проверкой это использовать vector<parserptr_t> вместо map и
     * кастить индекс вектора в void *ptr.
     * Потокобезопасность: может безопасно вызываться из разных потоков.
     * @param ptr контекст потока команд
     * @param buf указатель на буфер с командой
     * @param size размер буфера
     * */
  void receive_checked(void *ptr, const char *buf, size_t size);


    /**
     * @brief Обработка очередной команды
     * В конце команды должен стоять символ перевода строки, иначе
     * строка будет соединена со следующей.
     * Потокобезопасность: может безопасно вызываться из разных потоков.
     * @param ptr контекст потока команд
     * @param str строка  с командой
     * */
  void receive(void *ptr, const std::string &str) {receive(ptr, str.data(), str.size());}

    /**
     * @brief Разрушить контекст потока команд.
     * Потокобезопасность: может безопасно вызываться из разных потоков.
     * @param ptr контекст потока команд
     * */
  void disconnect(void *ptr);

    /**
     * @brief Разрушить все контексты потоков.
     * Потокобезопасность: может безопасно вызываться из разных потоков.
     * */
  void closeAll();

private:
  OutQueueSP m_qfile; ///!< очередь на запись в файл
  OutQueueSP m_qcout; ///!< очередь на вывод на экран
  std::thread m_worker_fw1; ///!< первый тред записи в файл
  std::thread m_worker_fw2; ///!< второй тред записи в файл
  std::thread m_worker_cw;  ///!< тред вывода на экран
  unsigned m_nextuid; ///!< uid для следующего контекста
  std::shared_mutex m_mapmutex; ///!< доступ к мапе
  std::map<void *, std::unique_ptr<Parser>> m_parser; ///!< контексты потоков команд
};
