#pragma once
/**
 * @file Parser.h
 * @brief Recursive Descent Parser for SimpleLang.
 */
#include "AST.h"
#include "Lexer.h"
#include <memory>
#include <vector>

/**
 * @brief Parser class that transforms tokens into an AST.
 */
class Parser {
public:
  /**
   * @brief Construct a new Parser object.
   * @param lexer Reference to the Lexer providing tokens.
   */
  Parser(Lexer &lexer);
  std::unique_ptr<ProgramNode> parseProgram();

private:
  Lexer &lexer;
  Token currentToken;

  void advance();
  void expect(TokenType type);

  std::unique_ptr<AstNode> parseDeclaration();
  std::unique_ptr<AstNode> parseStatement();
  std::unique_ptr<AstNode> parseAssignment();
  std::unique_ptr<AstNode> parseIf();
  std::unique_ptr<AstNode> parseExpression();
  std::unique_ptr<AstNode> parseTerm();
  std::unique_ptr<AstNode> parseFactor();
};
