#pragma once

#include <string_view>

/** @brief Чисто абстрактный класс - интерфейс блока */

class AbstractBlock {
public:
    /** @defgroup vdtor требуется объявить из-за вирт. деструктора
     * @{ */
  AbstractBlock() = default;
  AbstractBlock(const AbstractBlock &other) = default;
  AbstractBlock(AbstractBlock &&other) = default;
  AbstractBlock& operator=(const AbstractBlock &other) = default;
  AbstractBlock& operator=(AbstractBlock &&other) = default;
    /** @} */
    /** в базовом классе нужен виртуальный деструктор */
  virtual ~AbstractBlock() = default;

    /** @brief Добавить строку в буфер
     * @param line строка для обновления */
  virtual void append(const std::string_view &line) = 0;

    /** @brief количество накопленных команд */
  [[nodiscard]] virtual unsigned cmdnum() const = 0;

    /** @brief вывести накопленные данные и очистить буфер */
  virtual void flush() = 0;
};
