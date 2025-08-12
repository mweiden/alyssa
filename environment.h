//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <stdexcept>
#include "symbol_table.h"

using std::string;
using std::map;


class Environment {
public:
    // Construct an environment optionally linked to an outer environment.
    Environment(string _name, Environment *_outer = nullptr) : env{}, name(_name), outer(_outer) {}

    ~Environment() {
        env.clear();
    }

    string getName() {
        return name;
    }

    // symbol-based interface
    void setVariable(Symbol name, const string &value) {
        env[name] = value;
    }

    string getVariable(Symbol name) {
        auto it = env.find(name);
        if (it != env.end()) {
            return it->second;
        }
        if (outer) {
            return outer->getVariable(name);
        }
        throw std::out_of_range("Variable '" + symbolToString(name) + "' not found in environment '" + this->name + "'");
    }

    // string convenience wrappers
    void setVariable(const string &name, const string &value) {
        setVariable(stringToSymbol(name), value);
    }

    string getVariable(const string &name) {
        return getVariable(stringToSymbol(name));
    }

    std::map<Symbol,string>::iterator begin() {
        return env.begin();
    };

    std::map<Symbol,string>::iterator end() {
        return env.end();
    };

private:
    map<Symbol, string> env;
    string name;
    Environment *outer; // link to outer environment (not owned)
};

#endif //ENVIRONMENT_H
