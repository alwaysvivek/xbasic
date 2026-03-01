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
    if (currentToken.type == TOKEN_INT) {
      program->children.push_back(parseDeclaration());
    } else {
      program->children.push_back(parseStatement());
    }
  }
  return program;
}

std::unique_ptr<AstNode> Parser::parseDeclaration() {
  expect(TOKEN_INT);
  std::string name = currentToken.text;
  expect(TOKEN_IDENTIFIER);

  expect(TOKEN_SEMICOLON);
  return std::make_unique<DeclarationNode>(name);
}

std::unique_ptr<AstNode> Parser::parseStatement() {
  if (currentToken.type == TOKEN_IF) {
    return parseIf();
  } else {
    return parseAssignment();
  }
}

std::unique_ptr<AstNode> Parser::parseAssignment() {
  std::string name = currentToken.text;
  expect(TOKEN_IDENTIFIER);

  expect(TOKEN_ASSIGN);
  auto expr = parseExpression();
  expect(TOKEN_SEMICOLON);
  return std::make_unique<AssignmentNode>(name, std::move(expr));
}

std::unique_ptr<AstNode> Parser::parseIf() {
  expect(TOKEN_IF);
  expect(TOKEN_LPAREN);
  auto left = parseExpression();
  expect(TOKEN_EQUAL_EQUAL);
  auto right = parseExpression();
  expect(TOKEN_RPAREN);
  expect(TOKEN_LBRACE);

  auto ifNode = std::make_unique<IfNode>(std::move(left), std::move(right));
  while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
    ifNode->body.push_back(parseStatement());
  }
  expect(TOKEN_RBRACE);
  return ifNode;
}

std::unique_ptr<AstNode> Parser::parseExpression() {
  auto node = parseTerm();
  while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS) {
    char op = (currentToken.type == TOKEN_PLUS) ? '+' : '-';
    advance();
    node = std::make_unique<BinaryOpNode>(op, std::move(node), parseTerm());
  }
  return node;
}

std::unique_ptr<AstNode> Parser::parseTerm() { return parseFactor(); }

std::unique_ptr<AstNode> Parser::parseFactor() {
  if (currentToken.type == TOKEN_NUMBER) {
    int val = std::stoi(currentToken.text);
    advance();
    return std::make_unique<NumberNode>(val);
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
