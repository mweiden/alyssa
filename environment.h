//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <stdexcept>
#include "symbol_table.h"
#include "ast.h"

using std::string;
using std::map;

class Environment {
public:
    explicit Environment(string _name) { name = _name; }
    ~Environment() { env.clear(); }

    string getName() { return name; }

    // symbol-based interface
    void setVariable(Symbol name, const SExpr &value) { env[name] = value; }
    SExpr getVariable(Symbol name) {
        auto it = env.find(name);
        if (it == env.end()) {
            throw std::out_of_range("Variable '" + symbolToString(name) + "' not found in environment '" + this->name + "'");
        }
        return it->second;
    }

    // string convenience wrappers
    void setVariable(const string &name, const SExpr &value) { setVariable(stringToSymbol(name), value); }
    SExpr getVariable(const string &name) { return getVariable(stringToSymbol(name)); }

    std::map<Symbol,SExpr>::iterator begin() { return env.begin(); }
    std::map<Symbol,SExpr>::iterator end() { return env.end(); }

private:
    map<Symbol, SExpr> env;
    string name;
};

#endif //ENVIRONMENT_H
