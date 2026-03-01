//
// Created by Vivek Dagar on 01/03/26.
//

#include "Lexer.h"
/**
 * @file Lexer.cpp
 * @brief Implementation of the Lexical Analyzer.
 */

#include "Token.h"
#include <cctype>
#include <iostream>

// Constructor
Lexer::Lexer(const std::string &source)
    : source(source), position(0), line(1) {}

/**
 * @brief Extracts legal tokens from the source string.
 * @return Token The next recognized token.
 */
Token Lexer::getNextToken() {
  this->skipWhitespace();

  char c = peek();

  if (c == '\0')
    return {TOKEN_EOF, "", line};

  if (std::isalpha(c)) {
    std::string result;
    while (std::isalnum(peek())) {
      result += advance();
    }
    // Case-insensitive check for XBasic keywords
    std::string upper = result;
    for (auto &ch : upper)
      ch = std::toupper(ch);

    if (result == "num")
      return {TOKEN_NUM, result, line};
    if (result == "text")
      return {TOKEN_TEXT, result, line};
    if (upper == "IF")
      return {TOKEN_IF, result, line};
    if (upper == "THEN")
      return {TOKEN_THEN, result, line};
    if (upper == "ELSE")
      return {TOKEN_ELSE, result, line};
    if (upper == "FOR")
      return {TOKEN_FOR, result, line};
    if (upper == "TO")
      return {TOKEN_TO, result, line};
    if (upper == "NEXT")
      return {TOKEN_NEXT, result, line};
    if (upper == "WHILE")
      return {TOKEN_WHILE, result, line};
    if (upper == "END")
      return {TOKEN_END, result, line};
    if (upper == "PRINT")
      return {TOKEN_PRINT, result, line};

    // Compatibility
    if (result == "int")
      return {TOKEN_NUM, result, line};

    return {TOKEN_IDENTIFIER, result, line};
  }

  if (std::isdigit(c)) {
    std::string result;
    while (std::isdigit(peek())) {
      result += advance();
    }
    return {TOKEN_NUMBER, result, line};
  }

  if (c == '"') {
    advance(); // skip opening quote
    std::string result;
    while (peek() != '"' && peek() != '\0') {
      result += advance();
    }
    if (peek() == '"')
      advance(); // skip closing quote
    return {TOKEN_STRING, result, line};
  }

  if (c == '!') {
    advance();
    if (peek() == '=') {
      advance();
      return {TOKEN_BANG_EQUAL, "!=", line};
    }
    std::cerr << "Lexical Error: Unexpected character '!' at line " << line
              << std::endl;
    exit(1);
  }

  if (c == '=') {
    advance();
    if (peek() == '=') {
      advance();
      return {TOKEN_EQUAL_EQUAL, "==", line};
    }
    return {TOKEN_ASSIGN, "=", line};
  }

  if (c == '+') {
    advance();
    return {TOKEN_PLUS, "+", line};
  }
  if (c == '-') {
    advance();
    return {TOKEN_MINUS, "-", line};
  }
  if (c == '*') {
    advance();
    return {TOKEN_MULTIPLY, "*", line};
  }
  if (c == '/') {
    advance();
    return {TOKEN_DIVIDE, "/", line};
  }
  if (c == '(') {
    advance();
    return {TOKEN_LPAREN, "(", line};
  }
  if (c == ')') {
    advance();
    return {TOKEN_RPAREN, ")", line};
  }
  if (c == '{') {
    advance();
    return {TOKEN_LBRACE, "{", line};
  }
  if (c == '}') {
    advance();
    return {TOKEN_RBRACE, "}", line};
  }
  if (c == ';') {
    advance();
    return {TOKEN_SEMICOLON, ";", line};
  }

  std::cerr << "Lexical Error: Unknown character '" << c << "' at line " << line
            << std::endl;
  exit(1);
}

// Returns current char without actually consuming, null if at end
char Lexer::peek() const {
  if (position >= source.length())
    return '\0';
  return source[position];
}

char Lexer::advance() {
  if (position >= source.length())
    return '\0';
  return source[position++];
}

void Lexer::skipWhitespace() {
  while (position < source.length()) {
    char c = peek();
    if (c == ' ' || c == '\t' || c == '\r') {
      this->advance();
    } else if (c == '\n') {
      this->line++;
      this->advance();
    } else
      break;
  }
}
