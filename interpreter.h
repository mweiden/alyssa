//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include <string>
#include "environment.h"
#include "ast.h"

class Interpreter {
public:
    explicit Interpreter(Environment *env);

    // Evaluate a source string by parsing once into an AST
    SExpr eval(const std::string &source);

    // Evaluate a previously parsed AST node
    SExpr eval(const SExpr &expr, Environment *env);

    // Apply a procedure to arguments
    SExpr apply(const SExpr &procedure, const std::vector<SExpr> &arguments);

private:
    Environment *globalEnv;
};

#endif //INTERPRETER_H
