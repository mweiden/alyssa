//
// Simplified Lisp interpreter operating on AST nodes
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>
#include <functional>
#include "environment.h"
#include "ast.h"

class Interpreter {
public:
    virtual SExpr eval(const SExpr &expression) = 0;
    virtual SExpr apply(const SExpr &procedure, const std::vector<SExpr> &arguments) = 0;
    virtual ~Interpreter() = default;
};

class LispInterpreter : public Interpreter {
public:
    explicit LispInterpreter(Environment *_env);
    ~LispInterpreter();

    // convenience: parse source then eval
    SExpr eval(const std::string &source);

    // Interpreter interface
    SExpr eval(const SExpr &expression) override;
    SExpr apply(const SExpr &procedure, const std::vector<SExpr> &arguments) override;

    // internal
    SExpr eval(const SExpr &expression, Environment *env);

private:
    Environment *globalEnv;
    std::map<Symbol, std::function<SExpr(const std::vector<SExpr>&)>> primitives;
};

#endif // INTERPRETER_H
