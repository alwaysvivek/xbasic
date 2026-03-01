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
      int addr = symbolTable.declare(decl->name);
      if (decl->initialValue) {
        generateExpression(decl->initialValue.get());
        std::cout << "sta " << addr << " ; init " << decl->name << std::endl;
      }
      break;
    }
    case AST_ASSIGNMENT: {
      auto assign = static_cast<AssignmentNode *>(node);
      generateExpression(assign->expression.get());
      int addr = symbolTable.getAddress(assign->identifier);
      std::cout << "sta " << addr << " ; store result to " << assign->identifier
                << std::endl;
      break;
    }
    case AST_IF: {
      auto ifNode = static_cast<IfNode *>(node);
      int labelId = ++labelCounter;
      generateExpression(ifNode->condition.get());

      if (ifNode->condition->type == AST_BINARY_OP) {
        auto bin = static_cast<BinaryOpNode *>(ifNode->condition.get());
        if (bin->op == "==")
          std::cout << "jne label_else_" << labelId << std::endl;
        else if (bin->op == "!=")
          std::cout << "je label_else_" << labelId << std::endl;
        else
          std::cout << "jz label_else_" << labelId << std::endl;
      } else {
        std::cout << "jz label_else_" << labelId << std::endl;
      }

      for (auto &stmt : ifNode->thenBody)
        generate(stmt.get());

      if (!ifNode->elseBody.empty()) {
        std::cout << "jmp %label_end_" << labelId << std::endl;
        std::cout << "label_else_" << labelId << ":" << std::endl;
        for (auto &stmt : ifNode->elseBody)
          generate(stmt.get());
        std::cout << "label_end_" << labelId << ":" << std::endl;
      } else {
        std::cout << "label_else_" << labelId << ":" << std::endl;
      }
      break;
    }
    case AST_WHILE: {
      auto whileNode = static_cast<WhileNode *>(node);
      int labelId = ++labelCounter;
      std::cout << "label_while_start_" << labelId << ":" << std::endl;
      generateExpression(whileNode->condition.get());
      // For now, only support == and != which set/unset Zero flag
      // If op was '==' and result of cmp is not equal, exit
      if (whileNode->condition->type == AST_BINARY_OP) {
        auto bin = static_cast<BinaryOpNode *>(whileNode->condition.get());
        if (bin->op == "==")
          std::cout << "jne label_while_end_" << labelId << std::endl;
        else if (bin->op == "!=")
          std::cout << "je label_while_end_" << labelId << std::endl;
        else
          std::cout << "jz label_while_end_" << labelId << " ; default exit"
                    << std::endl;
      } else {
        std::cout << "jz label_while_end_" << labelId << std::endl;
      }

      for (auto &stmt : whileNode->body)
        generate(stmt.get());
      std::cout << "jmp label_while_start_" << labelId << std::endl;
      std::cout << "label_while_end_" << labelId << ":" << std::endl;
      break;
    }
    case AST_FOR: {
      auto forNode = static_cast<ForNode *>(node);
      int labelId = ++labelCounter;
      int addr = symbolTable.isDeclared(forNode->varName)
                     ? symbolTable.getAddress(forNode->varName)
                     : symbolTable.declare(forNode->varName);

      // Init: var = startExpr
      generateExpression(forNode->startExpr.get());
      std::cout << "sta " << addr << std::endl;

      std::cout << "label_for_start_" << labelId << ":" << std::endl;
      for (auto &stmt : forNode->body)
        generate(stmt.get());

      // Increment: var = var + 1
      std::cout << "lda " << addr << std::endl;
      std::cout << "inc" << std::endl;
      std::cout << "sta " << addr << std::endl;

      // Condition: if end >= var loop back
      std::cout << "mov B A" << std::endl;        // B = var
      generateExpression(forNode->endExpr.get()); // A = end
      std::cout << "cmp ; end - var" << std::endl;
      std::cout << "jnc label_for_start_" << labelId << " ; loop if end >= var"
                << std::endl;
      break;
    }
    case AST_PRINT: {
      auto printNode = static_cast<PrintNode *>(node);
      generateExpression(printNode->expression.get());
      std::cout << "out 0 ; print A to primary I/O" << std::endl;
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

      if (bin->op == "+") {
        std::cout << "add" << std::endl;
      } else if (bin->op == "-") {
        std::cout << "sub" << std::endl;
      } else if (bin->op == "==") {
        std::cout << "cmp" << std::endl;
        // The IF/WHILE will handle the branch
      }
    }
  }
};
