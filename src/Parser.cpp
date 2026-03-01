/**
 * @file Parser.cpp
 * @brief Implementation of the Recursive Descent Parser.
 */

#include "Parser.h"
#include <iostream>

/**
 * @brief Initialize parser and fetch the first token.
 */
Parser::Parser(Lexer &lexer) : lexer(lexer) { advance(); }

void Parser::advance() { currentToken = lexer.getNextToken(); }

void Parser::expect(TokenType type) {
  if (currentToken.type != type) {
    std::cerr << "Syntax Error: Expected token type " << type << " but found "
              << currentToken.type << " at line " << currentToken.line
              << std::endl;
    exit(1);
  }
  advance();
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
  auto program = std::make_unique<ProgramNode>();
  while (currentToken.type != TOKEN_EOF) {
    if (currentToken.type == TOKEN_NUM || currentToken.type == TOKEN_TEXT) {
      program->children.push_back(parseDeclaration());
    } else {
      program->children.push_back(parseStatement());
    }
  }
  return program;
}

std::unique_ptr<AstNode> Parser::parseDeclaration() {
  advance(); // consume 'num' or 'text'

  std::string name = currentToken.text;
  expect(TOKEN_IDENTIFIER);

  std::unique_ptr<AstNode> initialValue = nullptr;
  if (currentToken.type == TOKEN_ASSIGN) {
    advance();
    initialValue = parseExpression();
  }

  if (currentToken.type == TOKEN_SEMICOLON)
    advance();

  return std::make_unique<DeclarationNode>(name, std::move(initialValue));
}

std::unique_ptr<AstNode> Parser::parseStatement() {
  switch (currentToken.type) {
  case TOKEN_IF:
    return parseIf();
  case TOKEN_WHILE:
    return parseWhile();
  case TOKEN_FOR:
    return parseFor();
  case TOKEN_PRINT:
    return parsePrint();
  case TOKEN_IDENTIFIER:
    return parseAssignment();
  default:
    std::cerr << "Syntax Error: Unexpected token " << currentToken.text
              << " at line " << currentToken.line << std::endl;
    exit(1);
  }
}

std::unique_ptr<AstNode> Parser::parseAssignment() {
  std::string name = currentToken.text;
  expect(TOKEN_IDENTIFIER);
  expect(TOKEN_ASSIGN);
  auto expr = parseExpression();
  if (currentToken.type == TOKEN_SEMICOLON)
    advance();
  return std::make_unique<AssignmentNode>(name, std::move(expr));
}

std::unique_ptr<AstNode> Parser::parseIf() {
  expect(TOKEN_IF);
  auto condition = parseExpression();
  expect(TOKEN_THEN);

  auto ifNode = std::make_unique<IfNode>(std::move(condition));
  while (currentToken.type != TOKEN_ELSE && currentToken.type != TOKEN_END &&
         currentToken.type != TOKEN_EOF) {
    ifNode->thenBody.push_back(parseStatement());
  }

  if (currentToken.type == TOKEN_ELSE) {
    advance();
    while (currentToken.type != TOKEN_END && currentToken.type != TOKEN_EOF) {
      ifNode->elseBody.push_back(parseStatement());
    }
  }

  expect(TOKEN_END);
  return ifNode;
}

std::unique_ptr<AstNode> Parser::parseWhile() {
  expect(TOKEN_WHILE);
  auto condition = parseExpression();
  auto node = std::make_unique<WhileNode>(std::move(condition));
  while (currentToken.type != TOKEN_END && currentToken.type != TOKEN_EOF) {
    node->body.push_back(parseStatement());
  }
  expect(TOKEN_END);
  return node;
}

std::unique_ptr<AstNode> Parser::parseFor() {
  expect(TOKEN_FOR);
  std::string var = currentToken.text;
  expect(TOKEN_IDENTIFIER);
  expect(TOKEN_ASSIGN);
  auto start = parseExpression();
  expect(TOKEN_TO);
  auto end = parseExpression();

  auto node = std::make_unique<ForNode>(var, std::move(start), std::move(end));
  while (currentToken.type != TOKEN_NEXT && currentToken.type != TOKEN_EOF) {
    node->body.push_back(parseStatement());
  }
  expect(TOKEN_NEXT);
  return node;
}

std::unique_ptr<AstNode> Parser::parsePrint() {
  expect(TOKEN_PRINT);
  auto expr = parseExpression();
  return std::make_unique<PrintNode>(std::move(expr));
}

std::unique_ptr<AstNode> Parser::parseExpression() {
  auto node = parseTerm();
  while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS ||
         currentToken.type == TOKEN_EQUAL_EQUAL ||
         currentToken.type == TOKEN_BANG_EQUAL) {
    std::string op = currentToken.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node), parseTerm());
  }
  return node;
}

std::unique_ptr<AstNode> Parser::parseTerm() {
  auto node = parseFactor();
  while (currentToken.type == TOKEN_MULTIPLY ||
         currentToken.type == TOKEN_DIVIDE) {
    std::string op = currentToken.text;
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node), parseFactor());
  }
  return node;
}

std::unique_ptr<AstNode> Parser::parseFactor() {
  if (currentToken.type == TOKEN_NUMBER) {
    int val = std::stoi(currentToken.text);
    advance();
    return std::make_unique<NumberNode>(val);
  } else if (currentToken.type == TOKEN_STRING) {
    std::string val = currentToken.text;
    advance();
    return std::make_unique<StringNode>(val);
  } else if (currentToken.type == TOKEN_IDENTIFIER) {
    std::string name = currentToken.text;
    advance();
    return std::make_unique<IdentifierNode>(name);
  } else if (currentToken.type == TOKEN_LPAREN) {
    advance();
    auto node = parseExpression();
    expect(TOKEN_RPAREN);
    return node;
  }

  std::cerr << "Syntax Error: Unexpected token " << currentToken.text
            << " at line " << currentToken.line << std::endl;
  exit(1);
}
