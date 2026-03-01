/**
 * @file Codegen.h
 * @brief Code Generation for the 8-bit CPU targeting SimpleLang AST.
 */

#pragma once
#include "AST.h"
#include "SymbolTable.h"
#include <iostream>
#include <memory>

/**
 * @brief Code generator that visits AST nodes and prints assembly.
 */
class Codegen {
public:
  /**
   * @brief Construct a new Codegen object.
   * @param symbol_table Reference to the symbol table for address lookup.
   */
  Codegen(SymbolTable &symbol_table)
      : symbolTable(symbol_table), labelCounter(0) {}

  void generate(AstNode *node) {
    if (!node)
      return;

    switch (node->type) {
    case AST_PROGRAM: {
      auto prog = static_cast<ProgramNode *>(node);
      std::cout << ".text" << std::endl;
      for (auto &child : prog->children) {
        generate(child.get());
      }
      std::cout << "hlt" << std::endl;
      break;
    }
    case AST_DECLARATION: {
      auto decl = static_cast<DeclarationNode *>(node);
      symbolTable.declare(decl->name);
      break;
    }
    case AST_ASSIGNMENT: {
      auto assign = static_cast<AssignmentNode *>(node);
      generateExpression(assign->expression.get());
      int addr = symbolTable.getAddress(assign->identifier);
      std::cout << "sta " << addr << " ; store result from A to "
                << assign->identifier << std::endl;
      break;
    }
    case AST_IF: {
      auto ifNode = static_cast<IfNode *>(node);
      int labelId = ++labelCounter;

      // Evaluate right operand and put in B
      generateExpression(ifNode->right.get());
      std::cout << "mov B A" << std::endl;

      // Evaluate left operand and it stays in A
      generateExpression(ifNode->left.get());

      // Compare
      std::cout << "cmp" << std::endl;
      std::cout << "jne %label_end_" << labelId << std::endl;

      for (auto &stmt : ifNode->body) {
        generate(stmt.get());
      }

      std::cout << "label_end_" << labelId << ":" << std::endl;
      break;
    }
    default:
      break;
    }
  }

private:
  SymbolTable &symbolTable;
  int labelCounter;
  int nextReg = 2; // Use C, D, E, F, G as temporaries

  std::string getRegName(int r) {
    switch (r) {
    case 0:
      return "A";
    case 1:
      return "B";
    case 2:
      return "C";
    case 3:
      return "D";
    case 4:
      return "E";
    case 5:
      return "F";
    case 6:
      return "G";
    default:
      return "C";
    }
  }

  void generateExpression(AstNode *node) {
    if (node->type == AST_NUMBER) {
      auto num = static_cast<NumberNode *>(node);
      std::cout << "ldi A " << num->value << std::endl;
    } else if (node->type == AST_IDENTIFIER) {
      auto id = static_cast<IdentifierNode *>(node);
      int addr = symbolTable.getAddress(id->name);
      std::cout << "lda " << addr << " ; load " << id->name << " into A"
                << std::endl;
    } else if (node->type == AST_BINARY_OP) {
      auto bin = static_cast<BinaryOpNode *>(node);

      int tempReg = nextReg++;
      if (nextReg > 7) {
        std::cerr << "Semantic Error: Expression too complex (out of registers)"
                  << std::endl;
        exit(1);
      }

      // 1. Evaluate right side, result in A
      generateExpression(bin->right.get());
      // 2. Move result to temp register
      std::cout << "mov " << getRegName(tempReg) << " A" << std::endl;

      // 3. Evaluate left side, result in A
      generateExpression(bin->left.get());

      // 4. Move temp register to B
      std::cout << "mov B " << getRegName(tempReg) << std::endl;

      nextReg--;

      if (bin->op == '+') {
        std::cout << "add" << std::endl;
      } else if (bin->op == '-') {
        std::cout << "sub" << std::endl;
      }
    }
  }
};
