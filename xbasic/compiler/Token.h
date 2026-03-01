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
  TOKEN_INT, // Deprecated in favor of TOKEN_NUM
  TOKEN_NUM, // XBasic 'num'
  TOKEN_IF,  // XBasic 'IF'
  TOKEN_THEN,
  TOKEN_ELSE,
  TOKEN_FOR,
  TOKEN_TO,
  TOKEN_NEXT,
  TOKEN_WHILE,
  TOKEN_END,
  TOKEN_ELIF,
  TOKEN_FN,
  TOKEN_RETURN,
  TOKEN_LIST,
  TOKEN_BANG_EQUAL, // !=
  TOKEN_PRINT,
  TOKEN_TEXT, // XBasic 'text'
  TOKEN_STRING,
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_ASSIGN,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_MULTIPLY,
  TOKEN_DIVIDE,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER,
  TOKEN_LESS,
  TOKEN_GREATER_EQUAL,
  TOKEN_LESS_EQUAL,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_COMMA,
  TOKEN_SEMICOLON,
  TOKEN_EOF
};

struct Token {
  TokenType type;
  std::string text;
  int line;
};

#endif // SIMPLELANG_COMPILER_TOKEN_H