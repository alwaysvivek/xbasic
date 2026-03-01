/**
 * @file AST.h
 * @brief Definitions of Abstract Syntax Tree nodes for SimpleLang.
 */

#pragma once
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Enumeration of AST node types.
 */
enum AstNodeType {

  AST_PROGRAM,
  AST_DECLARATION,
  AST_ASSIGNMENT,
  AST_IF,
  AST_BINARY_OP,
  AST_NUMBER,
  AST_IDENTIFIER
};

struct AstNode {
  AstNodeType type;
  virtual ~AstNode() = default;
  AstNode(AstNodeType t) : type(t) {}
};

struct ProgramNode : AstNode {
  std::vector<std::unique_ptr<AstNode>> children;
  ProgramNode() : AstNode(AST_PROGRAM) {}
};

struct DeclarationNode : AstNode {
  std::string name;
  DeclarationNode(std::string n) : AstNode(AST_DECLARATION), name(n) {}
};

struct AssignmentNode : AstNode {
  std::string identifier;
  std::unique_ptr<AstNode> expression;
  AssignmentNode(std::string id, std::unique_ptr<AstNode> expr)
      : AstNode(AST_ASSIGNMENT), identifier(id), expression(std::move(expr)) {}
};

struct IfNode : AstNode {
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;
  std::vector<std::unique_ptr<AstNode>> body;
  IfNode(std::unique_ptr<AstNode> l, std::unique_ptr<AstNode> r)
      : AstNode(AST_IF), left(std::move(l)), right(std::move(r)) {}
};

struct BinaryOpNode : AstNode {
  char op; // '+' or '-'
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;
  BinaryOpNode(char o, std::unique_ptr<AstNode> l, std::unique_ptr<AstNode> r)
      : AstNode(AST_BINARY_OP), op(o), left(std::move(l)), right(std::move(r)) {
  }
};

struct NumberNode : AstNode {
  int value;
  NumberNode(int v) : AstNode(AST_NUMBER), value(v) {}
};

struct IdentifierNode : AstNode {
  std::string name;
  IdentifierNode(std::string n) : AstNode(AST_IDENTIFIER), name(n) {}
};
