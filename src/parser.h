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
  explicit Parser(unsigned N, std::unique_ptr<AbstractBlock> &&block, std::shared_ptr<AbstractBlock> globalBlock);

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
  std::unique_ptr<AbstractBlock> m_block; ///!< динамический блок индивидуальный для данного потока команд
  std::shared_ptr<AbstractBlock> m_globalBlock; ///!< глобальный статический блок общий для всех потоков команд
  int m_extendedModeLevel{0}; ///!< уровень вложенности, минимум 0 - статический блок
  unsigned m_N; ///!< размер статического блока
  AccuLine m_acculine; ///!< аккумулятор входных символов выдаёт команды по мере готовности
  std::mutex m_protectparse; ///!< мьютекс позволяет вызывать парсер из нескольки потоков
};
