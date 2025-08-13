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

SExpr primitiveList(const std::vector<SExpr> &args) {
    return SExpr(SExpr::List(args.begin(), args.end()));
}

SExpr primitiveCar(const std::vector<SExpr> &args) {
    if (args.size() != 1 || !args[0].isList()) throw std::runtime_error("car expects one list");
    const auto &lst = std::get<SExpr::List>(args[0].value);
    if (lst.empty()) throw std::runtime_error("car of empty list");
    return lst[0];
}

SExpr primitiveCdr(const std::vector<SExpr> &args) {
    if (args.size() != 1 || !args[0].isList()) throw std::runtime_error("cdr expects one list");
    const auto &lst = std::get<SExpr::List>(args[0].value);
    if (lst.empty()) return SExpr(SExpr::List{});
    SExpr::List rest(lst.begin() + 1, lst.end());
    return SExpr(rest);
}

SExpr primitiveCons(const std::vector<SExpr> &args) {
    if (args.size() != 2) throw std::runtime_error("cons expects two args");
    SExpr::List result;
    result.push_back(args[0]);
    if (args[1].isList()) {
        const auto &lst = std::get<SExpr::List>(args[1].value);
        result.insert(result.end(), lst.begin(), lst.end());
    } else {
        result.push_back(args[1]);
    }
    return SExpr(result);
}

SExpr primitiveEq(const std::vector<SExpr> &args) {
    if (args.size() != 2) throw std::runtime_error("eq? expects two args");
    const SExpr &a = args[0];
    const SExpr &b = args[1];
    if (a.isNumber() && b.isNumber()) {
        return std::get<double>(a.value) == std::get<double>(b.value) ? SExpr("true") : SExpr("false");
    }
    if (a.isSymbol() && b.isSymbol()) {
        return std::get<std::string>(a.value) == std::get<std::string>(b.value) ? SExpr("true") : SExpr("false");
    }
    return SExpr("false");
}

SExpr primitiveNull(const std::vector<SExpr> &args) {
    if (args.size() != 1) throw std::runtime_error("null? expects one arg");
    if (args[0].isList() && std::get<SExpr::List>(args[0].value).empty()) {
        return SExpr("true");
    }
    return SExpr("false");
}

SExpr primitiveAnd(const std::vector<SExpr> &args) {
    for (const auto &a : args) {
        if (a.isSymbol() && std::get<std::string>(a.value) == "false") {
            return SExpr("false");
        }
    }
    if (!args.empty()) return args.back();
    return SExpr("true");
}

SExpr primitiveOr(const std::vector<SExpr> &args) {
    for (const auto &a : args) {
        if (!(a.isSymbol() && std::get<std::string>(a.value) == "false")) {
            return a;
        }
    }
    return SExpr("false");
}
} // namespace

Interpreter::Interpreter(Environment *env) : globalEnv(env) {
    // Install primitive procedures in the global environment
    globalEnv->setVariable("+", SExpr(SExpr::Primitive(primitiveAdd)));
    globalEnv->setVariable("-", SExpr(SExpr::Primitive(primitiveSub)));
    globalEnv->setVariable("*", SExpr(SExpr::Primitive(primitiveMul)));
    globalEnv->setVariable("/", SExpr(SExpr::Primitive(primitiveDiv)));
    globalEnv->setVariable("list", SExpr(SExpr::Primitive(primitiveList)));
    globalEnv->setVariable("car", SExpr(SExpr::Primitive(primitiveCar)));
    globalEnv->setVariable("cdr", SExpr(SExpr::Primitive(primitiveCdr)));
    globalEnv->setVariable("cons", SExpr(SExpr::Primitive(primitiveCons)));
    globalEnv->setVariable("eq?", SExpr(SExpr::Primitive(primitiveEq)));
    globalEnv->setVariable("null?", SExpr(SExpr::Primitive(primitiveNull)));
    globalEnv->setVariable("and", SExpr(SExpr::Primitive(primitiveAnd)));
    globalEnv->setVariable("or", SExpr(SExpr::Primitive(primitiveOr)));
    // predefined booleans and NIL
    globalEnv->setVariable("true", SExpr("true"));
    globalEnv->setVariable("false", SExpr("false"));
    globalEnv->setVariable("NIL", SExpr(SExpr::List{}));
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
            } else if (sym == "set!") {
                if (list.size() != 3 || !list[1].isSymbol()) {
                    throw std::runtime_error("malformed set!");
                }
                std::string var = std::get<std::string>(list[1].value);
                SExpr val = eval(list[2], env);
                if (!env->updateVariable(var, val)) {
                    env->setVariable(var, val);
                }
                return val;
            } else if (sym == "if") {
                if (list.size() != 4) throw std::runtime_error("malformed if");
                SExpr test = eval(list[1], env);
                bool cond = !(test.isSymbol() && std::get<std::string>(test.value) == "false");
                return cond ? eval(list[2], env) : eval(list[3], env);
            } else if (sym == "quote") {
                if (list.size() != 2) throw std::runtime_error("malformed quote");
                return list[1];
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
