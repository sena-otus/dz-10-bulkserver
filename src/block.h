#pragma once

#include "abstractblock.h"
#include "outq.h"
#include <functional>
#include <list>
#include <ctime>
#include <memory>

/**
 * @brief Block накапливает команды во внутреннем буфере
 *  */
class Block : public AbstractBlock
{
public:
    /** @brief тип список очередей для вывода команд */
  using wlist_t = std::list<OutQueueSP>;

    /**
     * @brief Конструктор блока
     * @param puid uid потока команд
     * @param wlist список функций для вывода готовых блоков
     * @param getTime функция для получения времени в секундах,
     *   по умолчанию time(0), тест передаёт фиксированное число для воспроизводимости
     * */
  explicit Block(
    unsigned puid,
    wlist_t &&wlist,
    std::function<time_t()> getTime);

    /**
     * @brief Добавить строку в буфер
     * @param line строка для обновления
     * */
  void append(const std::string_view &line) override;


  [[nodiscard]] unsigned cmdnum() const override;

  /**
   * Вывести накопленные данные и очистить буфер
   * */
  void flush() override;

private:
  unsigned m_puid; //!< uid потока команд
  wlist_t m_wlist; //!< список писателей для вывода блока команд
  std::string m_str; //!< буфер для накопления команд
  unsigned m_cmdnum{0}; //!< количество накопленных команд
  time_t m_timestamp{0}; //!< временная метка
  int m_buid{0}; //!< uid блоков с одинаковой временной меткой
  std::function<time_t()> m_getTime;
};
