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
      int endLabel = labelId; // label_end_L

      // Evaluate condition
      generateExpression(ifNode->condition.get());

      // Branch to local elif or else or end if false
      std::string nextLabel =
          ifNode->elifBlocks.empty()
              ? (ifNode->elseBody.empty() ? "label_end_" : "label_else_")
              : "label_elif_0_";

      generateBranchOnFalse(ifNode->condition.get(),
                            nextLabel + std::to_string(labelId));

      // Then Body
      for (auto &stmt : ifNode->thenBody)
        generate(stmt.get());
      std::cout << "jmp label_end_" << endLabel << std::endl;

      // ELIF Blocks
      for (size_t i = 0; i < ifNode->elifBlocks.size(); ++i) {
        std::cout << "label_elif_" << i << "_" << labelId << ":" << std::endl;
        generateExpression(ifNode->elifBlocks[i].condition.get());

        std::string failLabel =
            (i + 1 < ifNode->elifBlocks.size())
                ? ("label_elif_" + std::to_string(i + 1) + "_")
                : (ifNode->elseBody.empty() ? "label_end_" : "label_else_");

        generateBranchOnFalse(ifNode->elifBlocks[i].condition.get(),
                              failLabel + std::to_string(labelId));

        for (auto &stmt : ifNode->elifBlocks[i].body)
          generate(stmt.get());
        std::cout << "jmp label_end_" << endLabel << std::endl;
      }

      // ELSE Body
      if (!ifNode->elseBody.empty()) {
        std::cout << "label_else_" << labelId << ":" << std::endl;
        for (auto &stmt : ifNode->elseBody)
          generate(stmt.get());
      }

      std::cout << "label_end_" << endLabel << ":" << std::endl;
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
    case AST_FUNCTION: {
      auto fn = static_cast<FunctionNode *>(node);
      std::cout << "jmp label_after_fn_" << fn->name << " ; skip fn body"
                << std::endl;
      std::cout << fn->name << ":" << std::endl;
      // Parameters: Map to global variables for now
      // This is limited (no recursion) but works for basic XBasic
      for (size_t i = 0; i < fn->params.size(); ++i) {
        int addr = symbolTable.isDeclared(fn->params[i])
                       ? symbolTable.getAddress(fn->params[i])
                       : symbolTable.declare(fn->params[i]);
        // Caller puts args in registers or memory?
        // For simplicity, let's say A is arg0, B is arg1...
        if (i < 2) {
          std::cout << "sta " << addr << " ; param " << fn->params[i]
                    << " from " << (i == 0 ? "A" : "B") << std::endl;
        }
      }
      for (auto &stmt : fn->body)
        generate(stmt.get());
      std::cout << "ret" << std::endl;
      std::cout << "label_after_fn_" << fn->name << ":" << std::endl;
      break;
    }
    case AST_RETURN: {
      auto ret = static_cast<ReturnNode *>(node);
      if (ret->expression)
        generateExpression(ret->expression.get());
      std::cout << "ret" << std::endl;
      break;
    }
    case AST_CALL: {
      auto call = static_cast<CallNode *>(node);
      // Simple built-in check
      if (call->name == "append") {
        std::cout << "; built-in append called" << std::endl;
        break;
      }
      if (call->args.size() > 0) {
        generateExpression(call->args[0].get());
      }
      if (call->args.size() > 1) {
        std::cout << "mov C A" << std::endl;
        generateExpression(call->args[1].get());
        std::cout << "mov B A" << std::endl;
        std::cout << "mov A C" << std::endl;
      }
      std::cout << "call " << call->name << std::endl;
      break;
    }
    case AST_LIST: {
      // Just a placeholder
      std::cout << "ldi A 0 ; list placeholder" << std::endl;
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
      } else if (bin->op == "*" || bin->op == "/") {
        // Warning: these are not natively supported by the 8-bit CPU!
        std::cerr << "Warning: '" << bin->op
                  << "' not implemented in hardware, result may be wrong"
                  << std::endl;
      } else if (bin->op == "==" || bin->op == "!=" || bin->op == ">" ||
                 bin->op == "<" || bin->op == ">=" || bin->op == "<=") {
        std::cout << "cmp" << std::endl;
      }
    } else if (node->type == AST_CALL) {
      generate(node);
    }
  }

  void generateBranchOnFalse(AstNode *condition, const std::string &target) {
    if (condition->type == AST_BINARY_OP) {
      auto bin = static_cast<BinaryOpNode *>(condition);
      if (bin->op == "==")
        std::cout << "jne " << target << " ; branch if not equal" << std::endl;
      else if (bin->op == "!=")
        std::cout << "je " << target << " ; branch if equal" << std::endl;
      else if (bin->op == "<")
        std::cout << "jnc " << target << " ; branch if >= (Carry=0)"
                  << std::endl;
      else if (bin->op == ">=")
        std::cout << "jc " << target << " ; branch if < (Carry=1)" << std::endl;
      else if (bin->op == ">") {
        // A > B <-> A != B AND A >= B
        // Skip if A == B OR A < B
        std::cout << "je " << target << std::endl;
        std::cout << "jc " << target << std::endl;
      } else if (bin->op == "<=") {
        // A <= B <-> A == B OR A < B
        // This is harder with single jumps.
        // Skip if A > B (A != B AND A >= B)
        int okLabel = ++labelCounter;
        std::cout << "je label_ok_" << okLabel << std::endl;
        std::cout << "jc label_ok_" << okLabel << std::endl;
        std::cout << "jmp " << target << std::endl;
        std::cout << "label_ok_" << okLabel << ":" << std::endl;
      } else {
        std::cout << "jz " << target << std::endl;
      }
    } else {
      std::cout << "jz " << target << std::endl;
    }
  }
};
