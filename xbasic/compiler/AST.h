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
  AST_ELIF,
  AST_WHILE,
  AST_FOR,
  AST_PRINT,
  AST_BINARY_OP,
  AST_NUMBER,
  AST_STRING,
  AST_IDENTIFIER,
  AST_FUNCTION,
  AST_RETURN,
  AST_LIST,
  AST_CALL
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
  std::unique_ptr<AstNode> initialValue;
  DeclarationNode(std::string n, std::unique_ptr<AstNode> val = nullptr)
      : AstNode(AST_DECLARATION), name(n), initialValue(std::move(val)) {}
};

struct AssignmentNode : AstNode {
  std::string identifier;
  std::unique_ptr<AstNode> expression;
  AssignmentNode(std::string id, std::unique_ptr<AstNode> expr)
      : AstNode(AST_ASSIGNMENT), identifier(id), expression(std::move(expr)) {}
};

struct ElifBlock {
  std::unique_ptr<AstNode> condition;
  std::vector<std::unique_ptr<AstNode>> body;
};

struct IfNode : AstNode {
  std::unique_ptr<AstNode> condition;
  std::vector<std::unique_ptr<AstNode>> thenBody;
  std::vector<ElifBlock> elifBlocks;
  std::vector<std::unique_ptr<AstNode>> elseBody;
  IfNode(std::unique_ptr<AstNode> cond)
      : AstNode(AST_IF), condition(std::move(cond)) {}
};

struct WhileNode : AstNode {
  std::unique_ptr<AstNode> condition;
  std::vector<std::unique_ptr<AstNode>> body;
  WhileNode(std::unique_ptr<AstNode> cond)
      : AstNode(AST_WHILE), condition(std::move(cond)) {}
};

struct ForNode : AstNode {
  std::string varName;
  std::unique_ptr<AstNode> startExpr;
  std::unique_ptr<AstNode> endExpr;
  std::vector<std::unique_ptr<AstNode>> body;
  ForNode(std::string name, std::unique_ptr<AstNode> start,
          std::unique_ptr<AstNode> end)
      : AstNode(AST_FOR), varName(name), startExpr(std::move(start)),
        endExpr(std::move(end)) {}
};

struct PrintNode : AstNode {
  std::unique_ptr<AstNode> expression;
  PrintNode(std::unique_ptr<AstNode> expr)
      : AstNode(AST_PRINT), expression(std::move(expr)) {}
};

struct StringNode : AstNode {
  std::string value;
  StringNode(std::string v) : AstNode(AST_STRING), value(v) {}
};

struct BinaryOpNode : AstNode {
  std::string op; // '+', '-', '*', '/', '==', '!=', '>', '<', '>=', '<='
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;
  BinaryOpNode(std::string o, std::unique_ptr<AstNode> l,
               std::unique_ptr<AstNode> r)
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

struct FunctionNode : AstNode {
  std::string name;
  std::vector<std::string> params;
  std::vector<std::unique_ptr<AstNode>> body;
  FunctionNode(std::string n, std::vector<std::string> p)
      : AstNode(AST_FUNCTION), name(n), params(std::move(p)) {}
};

struct ReturnNode : AstNode {
  std::unique_ptr<AstNode> expression;
  ReturnNode(std::unique_ptr<AstNode> expr)
      : AstNode(AST_RETURN), expression(std::move(expr)) {}
};

struct ListNode : AstNode {
  std::vector<std::unique_ptr<AstNode>> elements;
  ListNode() : AstNode(AST_LIST) {}
};

struct CallNode : AstNode {
  std::string name;
  std::vector<std::unique_ptr<AstNode>> args;
  CallNode(std::string n) : AstNode(AST_CALL), name(n) {}
};
