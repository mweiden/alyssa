//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <stdexcept>

using std::string;
using std::map;


class Environment {
public:
    Environment(string _name) {
        name = _name;
    }

    ~Environment() {
        env.clear();
    }

    string getName() {
        return name;
    }

    void setVariable(string name, string value) {
        env[name] = value;
    }

    string getVariable(string name) {
        auto it = env.find(name);
        if (it == env.end()) {
            throw std::out_of_range("Variable '" + name + "' not found in environment '" + this->name + "'");
        }
        return it->second;
    }

    const std::map<string,string>::iterator begin() {
        return env.begin();
    };

    const std::map<string,string>::iterator end() {
        return env.end();
    };

private:
    map<string, string> env;
    string name;
};

#endif //ENVIRONMENT_H
