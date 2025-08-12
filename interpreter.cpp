#include "interpreter.h"
#include <sstream>
#include <stdexcept>
#include <cmath>

LispInterpreter::LispInterpreter(Environment *_env) : globalEnv(_env) {
    primitives[stringToSymbol("+")] = [](const std::vector<SExpr> &args){
        double acc = 0; for(const auto &a: args) acc += a.number; return makeNumber(acc); };
    primitives[stringToSymbol("-")] = [](const std::vector<SExpr> &args){
        if(args.empty()) return makeNumber(0);
        double acc = args[0].number; for(size_t i=1;i<args.size();++i) acc -= args[i].number; return makeNumber(acc); };
    primitives[stringToSymbol("*")] = [](const std::vector<SExpr> &args){
        double acc = 1; for(const auto &a: args) acc *= a.number; return makeNumber(acc); };
    primitives[stringToSymbol("/")] = [](const std::vector<SExpr> &args){
        if(args.empty()) return makeNumber(1);
        double acc = args[0].number; for(size_t i=1;i<args.size();++i) acc /= args[i].number; return makeNumber(acc); };
}

LispInterpreter::~LispInterpreter() {
}

SExpr LispInterpreter::eval(const std::string &source){
    SExpr ast = parse(source);
    return eval(ast, globalEnv);
}

SExpr LispInterpreter::eval(const SExpr &expression){
    return eval(expression, globalEnv);
}

SExpr LispInterpreter::eval(const SExpr &expression, Environment *env){
    switch(expression.type){
        case SExpr::Type::NUMBER:
            return expression;
        case SExpr::Type::SYMBOL:
            return env->getVariable(expression.sym);
        case SExpr::Type::LIST:{
            if(expression.list.empty()) return makeNil();
            const SExpr &first = expression.list[0];
            if(first.type != SExpr::Type::SYMBOL)
                throw std::runtime_error("procedure must be symbol");
            Symbol sym = first.sym;
            std::vector<SExpr> args;
            if(symbolToString(sym) == "quote"){
                return expression.list[1];
            } else if(symbolToString(sym) == "define"){
                const SExpr &name = expression.list[1];
                const SExpr &valExpr = expression.list[2];
                SExpr val = eval(valExpr, env);
                env->setVariable(name.sym, val);
                return val;
            } else {
                for(size_t i=1;i<expression.list.size();++i){
                    args.push_back(eval(expression.list[i], env));
                }
                return apply(first, args);
            }
        }
        case SExpr::Type::NIL:
        default:
            return expression;
    }
}

SExpr LispInterpreter::apply(const SExpr &procedure, const std::vector<SExpr> &arguments){
    if(procedure.type != SExpr::Type::SYMBOL)
        throw std::runtime_error("apply: procedure not symbol");
    auto it = primitives.find(procedure.sym);
    if(it == primitives.end())
        throw std::runtime_error("unknown primitive: "+symbolToString(procedure.sym));
    return it->second(arguments);
}
