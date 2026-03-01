//
// Created by Vivek Dagar on 01/03/26.
//

#ifndef SIMPLELANG_COMPILER_TOKEN_H
#define SIMPLELANG_COMPILER_TOKEN_H
/**
 * @file Token.h
 * @brief Definition of Token types and the Token structure for SimpleLang.
 */

#pragma once
#include <string>

/**
 * @brief Enum of all possible tokens in SimpleLang.
 * No unknown token, as that results in a fatal error.
 */
enum TokenType {
  TOKEN_INT,
  TOKEN_IF,
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_ASSIGN,      // =
  TOKEN_PLUS,        // +
  TOKEN_MINUS,       // -
  TOKEN_EQUAL_EQUAL, // ==
  TOKEN_LPAREN,      // (
  TOKEN_RPAREN,      // )
  TOKEN_LBRACE,      // {
  TOKEN_RBRACE,      // }
  TOKEN_SEMICOLON,   // ;
  TOKEN_EOF
};

struct Token {
  TokenType type;
  std::string text;
  int line;
};

#endif // SIMPLELANG_COMPILER_TOKEN_H