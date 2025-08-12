//
// Rewritten interpreter operating on AST nodes.
//

#include "interpreter.h"
#include <stdexcept>
#include <sstream>

namespace {
SExpr primitiveAdd(const std::vector<SExpr> &args) {
    double sum = 0;
    for (const auto &a : args) {
        if (!a.isNumber()) throw std::runtime_error("+ expects numbers");
        sum += std::get<double>(a.value);
    }
    return SExpr(sum);
}

SExpr primitiveSub(const std::vector<SExpr> &args) {
    if (args.empty()) throw std::runtime_error("- expects at least one arg");
    double result = std::get<double>(args[0].value);
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i].isNumber()) throw std::runtime_error("- expects numbers");
        result -= std::get<double>(args[i].value);
    }
    return SExpr(result);
}

SExpr primitiveMul(const std::vector<SExpr> &args) {
    double result = 1;
    for (const auto &a : args) {
        if (!a.isNumber()) throw std::runtime_error("* expects numbers");
        result *= std::get<double>(a.value);
    }
    return SExpr(result);
}

SExpr primitiveDiv(const std::vector<SExpr> &args) {
    if (args.empty()) throw std::runtime_error("/ expects at least one arg");
    double result = std::get<double>(args[0].value);
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i].isNumber()) throw std::runtime_error("/ expects numbers");
        result /= std::get<double>(args[i].value);
    }
    return SExpr(result);
}
} // namespace

Interpreter::Interpreter(Environment *env) : globalEnv(env) {
    // Install primitive procedures in the global environment
    globalEnv->setVariable("+", SExpr(SExpr::Primitive(primitiveAdd)));
    globalEnv->setVariable("-", SExpr(SExpr::Primitive(primitiveSub)));
    globalEnv->setVariable("*", SExpr(SExpr::Primitive(primitiveMul)));
    globalEnv->setVariable("/", SExpr(SExpr::Primitive(primitiveDiv)));
}

SExpr Interpreter::eval(const std::string &source) {
    SExpr ast = parse(source);
    return eval(ast, globalEnv);
}

SExpr Interpreter::eval(const SExpr &expr, Environment *env) {
    if (expr.isNumber() || expr.isPrimitive() || expr.isLambda()) {
        return expr;
    }
    if (expr.isSymbol()) {
        return env->getVariable(std::get<std::string>(expr.value));
    }
    if (expr.isList()) {
        const auto &list = std::get<SExpr::List>(expr.value);
        if (list.empty()) return expr; // nil
        const SExpr &first = list[0];
        if (first.isSymbol()) {
            const std::string &sym = std::get<std::string>(first.value);
            if (sym == "define") {
                if (list.size() != 3 || !list[1].isSymbol()) {
                    throw std::runtime_error("malformed define");
                }
                std::string var = std::get<std::string>(list[1].value);
                SExpr val = eval(list[2], env);
                env->setVariable(var, val);
                return val;
            } else if (sym == "lambda") {
                if (list.size() < 3 || !list[1].isList()) {
                    throw std::runtime_error("malformed lambda");
                }
                std::vector<std::string> params;
                for (const auto &p : std::get<SExpr::List>(list[1].value)) {
                    if (!p.isSymbol()) throw std::runtime_error("lambda param not symbol");
                    params.push_back(std::get<std::string>(p.value));
                }
                auto lam = std::make_shared<Lambda>();
                lam->params = params;
                lam->body = std::make_shared<SExpr>(list[2]);
                lam->env = env;
                return SExpr(lam);
            }
        }
        // application
        SExpr proc = eval(first, env);
        std::vector<SExpr> args;
        for (size_t i = 1; i < list.size(); ++i) {
            args.push_back(eval(list[i], env));
        }
        return apply(proc, args);
    }
    throw std::runtime_error("unknown expression");
}

SExpr Interpreter::apply(const SExpr &procedure, const std::vector<SExpr> &arguments) {
    if (procedure.isPrimitive()) {
        auto fn = std::get<SExpr::Primitive>(procedure.value);
        return fn(arguments);
    } else if (procedure.isLambda()) {
        auto lam = std::get<std::shared_ptr<Lambda>>(procedure.value);
        if (lam->params.size() != arguments.size()) {
            throw std::runtime_error("argument count mismatch");
        }
        auto newEnv = std::make_unique<Environment>("lambda", lam->env);
        for (size_t i = 0; i < lam->params.size(); ++i) {
            newEnv->setVariable(lam->params[i], arguments[i]);
        }
        return eval(*lam->body, newEnv.get());
    }
    throw std::runtime_error("attempt to call non-procedure");
}
