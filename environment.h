//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <stdexcept>
#include "ast.h"

using std::string;
using std::map;

class Environment {
public:
    // Construct an environment optionally linked to an outer environment.
    Environment(string _name, Environment *_outer = nullptr) : env{}, name(_name), outer(_outer) {}

    ~Environment() {
        env.clear();
    }

    string getName() { return name; }

    void setVariable(const string &name, const SExpr &value) { env[name] = value; }

    // Update an existing variable in this environment chain.
    // Returns true if the variable was found and updated, false otherwise.
    bool updateVariable(const string &name, const SExpr &value) {
        auto it = env.find(name);
        if (it != env.end()) {
            it->second = value;
            return true;
        }
        if (outer) {
            return outer->updateVariable(name, value);
        }
        return false;
    }

    SExpr getVariable(const string &name) {
        auto it = env.find(name);
        if (it != env.end()) {
            return it->second;
        }
        if (outer) {
            return outer->getVariable(name);
        }
        throw std::out_of_range("Variable '" + name + "' not found in environment '" + this->name + "'");
    }

    map<string, SExpr>::iterator begin() { return env.begin(); }
    map<string, SExpr>::iterator end() { return env.end(); }

private:
    map<string, SExpr> env;
    string name;
    Environment *outer; // link to outer environment (not owned)
};

#endif //ENVIRONMENT_H
