#pragma once

#include "abstractblock.h"
#include "acculine.h"
#include <memory>
#include <string_view>

/**
 * @brief Parser разбирает входные строки на блоки и выводит их
 * */
class Parser final {
public:
  /**
   * @param N размер блока
   * @param block буфер для команд
   * */
  explicit Parser(unsigned N, std::unique_ptr<AbstractBlock> &&block);

  /**
   *  @brief Разобрать строку и вывести блок по готовности
   *  @param line строка для разбора
   *  */
  void parse(const std::string_view &line);

  /**
   * @brief Вывести остатки данных в буфере(должно быть вызвано в конце)
   * */
  void finalize();

private:
  std::unique_ptr<AbstractBlock> m_block;
  int m_lineno{0};
  int m_extendedModeLevel{0};
  unsigned m_N;
  AccuLine m_acculine;
  std::mutex m_protectparse;
};
