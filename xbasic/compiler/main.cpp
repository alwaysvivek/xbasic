#include "Codegen.h"
/**
 * @file main.cpp
 * @brief Entry point for the SimpleLang Compiler.
 */
#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * @brief Main compiler driver.
 */
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./compiler <input_file>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << argv[1] << std::endl;
    return 1;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();

  Lexer lexer(source);
  Parser parser(lexer);
  auto ast = parser.parseProgram();

  SymbolTable symbolTable;
  Codegen codegen(symbolTable);
  codegen.generate(ast.get());

  return 0;
}
