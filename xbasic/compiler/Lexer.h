//
// Created by Vivek Dagar on 01/03/26.
//

#ifndef SIMPLELANG_COMPILER_LEXER_H
#define SIMPLELANG_COMPILER_LEXER_H
/**
 * @file Lexer.h
 * @brief SimpleLang Lexer for tokenizing source code strings.
 */

#pragma once
#include "Token.h"
#include <string>
#include <vector>

/**
 * @brief Lexer class responsible for character-by-character scanning.
 */
class Lexer {
public:
  /**
   * @brief Construct a new Lexer object.
   * @param source The full source code string to tokenize.
   */
  Lexer(const std::string &source);

  Token getNextToken();

private:
  std::string source;
  size_t position;
  int line;

  char peek() const;
  char advance();
  void skipWhitespace();

  // Helper function for text and numbers
  Token identifierOrKeyword();
  Token number();
};

#endif // SIMPLELANG_COMPILER_LEXER_H