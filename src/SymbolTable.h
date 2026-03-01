/**
 * @file SymbolTable.h
 * @brief Logic for variable address mapping and semantic validation.
 */

#pragma once
#include <iostream>
#include <map>
#include <string>

/**
 * @brief Manages variable name to memory address mappings.
 */
class SymbolTable {
public:
  /**
   * @brief Initialize symbol table (Variables start at 0x80).
   */
  SymbolTable() : nextAddress(0x80) {}

  bool declare(const std::string &name) {
    if (table.find(name) != table.end()) {
      std::cerr << "Semantic Error: Redeclaration of variable '" << name << "'"
                << std::endl;
      return false;
    }
    if (nextAddress > 0xFF) {
      std::cerr << "Semantic Error: Memory Exhausted" << std::endl;
      return false;
    }
    table[name] = nextAddress++;
    return true;
  }

  int getAddress(const std::string &name) {
    if (table.find(name) == table.end()) {
      std::cerr << "Semantic Error: Undeclared variable '" << name << "'"
                << std::endl;
      exit(1);
    }
    return table[name];
  }

private:
  std::map<std::string, int> table;
  int nextAddress;
};
