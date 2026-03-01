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
    if (currentToken.type == TOKEN_NUM || currentToken.type == TOKEN_TEXT ||
        currentToken.type == TOKEN_LIST) {
      program->children.push_back(parseDeclaration());
    } else if (currentToken.type == TOKEN_FN) {
      program->children.push_back(parseFunction());
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
  case TOKEN_RETURN:
    return parseReturn();
  case TOKEN_IDENTIFIER: {
    // Check if it's a call or assignment
    // (peek doesn't exist in Parser, but currentToken is identifier)
    // Actually, parseAssignment will handle it if we add lookahead or just try
    // both. For now, if NEXT is '(', it's a call. But our Lexer doesn't have
    // lookahead readily available in Parser. Let's just use a simple heuristic
    // or update Lexer. For now, let's just assume it's assignment unless it's a
    // known built-in.
    return parseAssignment();
  }
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
  while (currentToken.type != TOKEN_ELIF && currentToken.type != TOKEN_ELSE &&
         currentToken.type != TOKEN_END && currentToken.type != TOKEN_EOF) {
    ifNode->thenBody.push_back(parseStatement());
  }

  while (currentToken.type == TOKEN_ELIF) {
    advance();
    auto elifCond = parseExpression();
    expect(TOKEN_THEN);
    ElifBlock block;
    block.condition = std::move(elifCond);
    while (currentToken.type != TOKEN_ELIF && currentToken.type != TOKEN_ELSE &&
           currentToken.type != TOKEN_END && currentToken.type != TOKEN_EOF) {
      block.body.push_back(parseStatement());
    }
    ifNode->elifBlocks.push_back(std::move(block));
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

  if (currentToken.type == TOKEN_THEN)
    advance();

  auto node = std::make_unique<ForNode>(var, std::move(start), std::move(end));
  while (currentToken.type != TOKEN_NEXT && currentToken.type != TOKEN_END &&
         currentToken.type != TOKEN_EOF) {
    node->body.push_back(parseStatement());
  }
  if (currentToken.type == TOKEN_NEXT)
    advance();
  else
    expect(TOKEN_END);

  return node;
}

std::unique_ptr<AstNode> Parser::parsePrint() {
  expect(TOKEN_PRINT);
  bool hasParen = (currentToken.type == TOKEN_LPAREN);
  if (hasParen)
    advance();

  auto expr = parseExpression();

  if (hasParen)
    expect(TOKEN_RPAREN);

  return std::make_unique<PrintNode>(std::move(expr));
}

std::unique_ptr<AstNode> Parser::parseFunction() {
  expect(TOKEN_FN);
  std::string name = currentToken.text;
  expect(TOKEN_IDENTIFIER);
  expect(TOKEN_LPAREN);

  std::vector<std::string> params;
  if (currentToken.type != TOKEN_RPAREN) {
    params.push_back(currentToken.text);
    expect(TOKEN_IDENTIFIER);
    while (currentToken.type == TOKEN_COMMA) {
      advance();
      params.push_back(currentToken.text);
      expect(TOKEN_IDENTIFIER);
    }
  }
  expect(TOKEN_RPAREN);

  auto fn = std::make_unique<FunctionNode>(name, std::move(params));
  while (currentToken.type != TOKEN_END && currentToken.type != TOKEN_EOF) {
    fn->body.push_back(parseStatement());
  }
  expect(TOKEN_END);
  return fn;
}

std::unique_ptr<AstNode> Parser::parseReturn() {
  expect(TOKEN_RETURN);
  auto expr = parseExpression();
  return std::make_unique<ReturnNode>(std::move(expr));
}

std::unique_ptr<AstNode> Parser::parseList() {
  expect(TOKEN_LBRACKET);
  auto list = std::make_unique<ListNode>();
  if (currentToken.type != TOKEN_RBRACKET) {
    list->elements.push_back(parseExpression());
    while (currentToken.type == TOKEN_COMMA) {
      advance();
      list->elements.push_back(parseExpression());
    }
  }
  expect(TOKEN_RBRACKET);
  return list;
}

std::unique_ptr<AstNode> Parser::parseCall() {
  std::string name = currentToken.text;
  expect(TOKEN_IDENTIFIER);
  expect(TOKEN_LPAREN);

  auto call = std::make_unique<CallNode>(name);
  if (currentToken.type != TOKEN_RPAREN) {
    call->args.push_back(parseExpression());
    while (currentToken.type == TOKEN_COMMA) {
      advance();
      call->args.push_back(parseExpression());
    }
  }
  expect(TOKEN_RPAREN);
  return call;
}

std::unique_ptr<AstNode> Parser::parseExpression() {
  auto node = parseTerm();
  while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS ||
         currentToken.type == TOKEN_EQUAL_EQUAL ||
         currentToken.type == TOKEN_BANG_EQUAL ||
         currentToken.type == TOKEN_GREATER ||
         currentToken.type == TOKEN_LESS ||
         currentToken.type == TOKEN_GREATER_EQUAL ||
         currentToken.type == TOKEN_LESS_EQUAL) {
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
    // Lookahead for '(' to distinguish between identifier and call
    // Since we don't have peek, we'll use a hack or just try parseCall
    // Actually, we can check if currentToken is followed by '(' if we update
    // Lexer For now, let's assume if it's a known function or followed by '('
    // Wait, I can't check 'followed by' without advance.
    // I'll just change the caller to handle this or use a simple lookahead in
    // Parser.
    std::string name = currentToken.text;
    advance();
    if (currentToken.type == TOKEN_LPAREN) {
      // It's a call!
      auto call = std::make_unique<CallNode>(name);
      advance(); // consume '('
      if (currentToken.type != TOKEN_RPAREN) {
        call->args.push_back(parseExpression());
        while (currentToken.type == TOKEN_COMMA) {
          advance();
          call->args.push_back(parseExpression());
        }
      }
      expect(TOKEN_RPAREN);
      return call;
    }
    return std::make_unique<IdentifierNode>(name);
  } else if (currentToken.type == TOKEN_LPAREN) {
    advance();
    auto node = parseExpression();
    expect(TOKEN_RPAREN);
    return node;
  } else if (currentToken.type == TOKEN_LBRACKET) {
    return parseList();
  }

  std::cerr << "Syntax Error: Unexpected token " << currentToken.text
            << " at line " << currentToken.line << std::endl;
  exit(1);
}
