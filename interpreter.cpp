//
// Created by Matthew Weiden on 7/15/17.
//
#include "interpreter.h"

#include <iostream>
#include <regex>
#include <sstream>

using std::string;
using std::map;


// helper functions
bool simpleMatch(string rxStr, string expression) {
    std::regex rx(rxStr);
    std::smatch match;
    std::regex_match(expression, match, rx);
    return match.size() == 1;
}


template<typename T>
T reduce(const std::vector<T> &data,
                T (*reduceFn)(T, T),
                int leftOffset = 0,
                int rightOffset = 0
) {
    typedef typename std::vector<T>::const_iterator Iterator;
    Iterator it = data.cbegin() + leftOffset;
    Iterator end = data.cend() - rightOffset;
    if (it == end) {
        throw 0;
    } else {
        T accumulator = *it;
        ++it;
        for (; it != end; ++it) {
            accumulator = reduceFn(accumulator, *it);
        }
        return accumulator;
    }
}


// Interpreter
string Interpreter::nextToken(std::istream &in) {
    int i = 0;
    char current;
    string token = "";

    // chew off leading spaces
    do { current = in.get(); } while (isspace(current));

    if (current == '(') {
        int openParens = 0;
        do {
            if (current == '(') openParens++;
            if (current == ')') openParens--;
            token += current;
            current = in.get();
        } while (openParens > 0 and current not_eq '\0' and current not_eq EOF);
    } else {
        while (current not_eq '\0' and current not_eq EOF and isgraph(current)) {
            token += current;
            current = in.get();
        }
    }
    return token;
}


string Interpreter::unparens(string expression) {
    if (expression[0] == '(') {
        string s(expression.begin() + 1, expression.end() - 1);
        return s;
    } else {
        return expression;
    }
}


// LispInterpreter
LispInterpreter::LispInterpreter(Environment *_env) {
    globalEnvName = _env->getName();
    envs[_env->getName()] = _env;
    primitiveProcedures["list"] = list;
    primitiveProcedures["car"] = car;
    primitiveProcedures["cdr"] = cdr;
    primitiveProcedures["cons"] = cons;
    primitiveProcedures["null?"] = isNull;
    primitiveProcedures["eq?"] = isEq;
    primitiveProcedures["+"] = arithmetic;
    primitiveProcedures["-"] = arithmetic;
    primitiveProcedures["*"] = arithmetic;
    primitiveProcedures["/"] = arithmetic;
    primitiveProcedures["and"] = boolean;
    primitiveProcedures["or"] = boolean;
    primitiveProcedures["not"] = boolean;
}

LispInterpreter::~LispInterpreter() {
    primitiveProcedures.clear();
    for (auto const &kv : envs) {
        if (kv.first != globalEnvName) {
            delete kv.second;
        }
    }
    envs.clear();
}

string LispInterpreter::eval(string expression) {
    return eval(expression, envs[globalEnvName]);
}

string LispInterpreter::eval(string expression, Environment *env) {
    if (envs.find(env->getName()) == envs.end()) envs[env->getName()] = env;

    if (isPrimitiveProcedure(expression)) {
        return expression;
    } else if (isSelfEvaluating(expression)) {
        return expression;
    } else if (isVariable(expression)) {
        return env->getVariable(expression);
    } else {
        std::vector<string> expVec;
        if (expression != "" and expression[0] == '(')
            expVec = stringToVector(unparens(expression));
        else
            expVec = {expression};
        return eval(expVec, env);
    }
}

string LispInterpreter::eval(std::vector<string> expression, Environment *env) {

    if (isAssignment(expression)) {
        return evalAssignment(expression, env);
    } else if (isDefinition(expression)) {
        return evalDefinition(expression, env);
    } else if (isIf(expression)) {
        return evalIf(expression, env);
    } else if (isLambda(expression)) {
        return makeProcedure(lambdaParameters(expression), lambdaBody(expression), env->getName());
    } else if (isBegin(expression)) {
        return evalSequence(beginActions(expression), env);
    } else if (isCondition(expression)) {
        return eval(condToIf(expression), env);
    } else if (isApplication(expression)) {
        string procedure = eval(getOperator(expression), env);
        std::vector<string> arguments = listOfValues(getOperands(expression), env);
        string applied = apply(procedure, arguments);
        return applied;
    } else {
        throw std::invalid_argument("Unknown expression type -- EVAL \"" + vecToString(expression) + "\"");
    }
}

Environment* LispInterpreter::extendEnvironment(std::vector<string> vars, std::vector<string> vals, Environment *env) {
    if (vals.size() != vars.size()) throw std::invalid_argument("Vars and vals aren't the same length!");
    Environment *newEnv = new Environment(env->getName() + std::to_string(frameCount));
    frameCount++;
    std::map<string, string>::iterator it;
    for (it = env->begin(); it != env->end(); it++) {
        newEnv->setVariable(it->first, it->second);
    }
    for (int i = 0; i < vars.size(); i++) {
        newEnv->setVariable(vars[i], vals[i]);
    }
    return newEnv;
}

string LispInterpreter::apply(string procedure, std::vector<string> arguments) {
    std::vector<string> procedureVec = stringToVector(unparens(procedure));
    if (isPrimitiveProcedure(procedure)) {
        return applyPrimitiveProcedure(procedure, arguments);
    } else if (isCompoundProcedure(procedureVec)) {
        Environment *extendedEnv = extendEnvironment(
                procedureParams(procedureVec),
                arguments,
                procedureEnv(procedureVec)
        );
        string result = eval(procedureBody(procedureVec), extendedEnv);
        return result;
    } else {
        throw std::invalid_argument("Unkown procedure type -- APPLY \"" + procedure + "\"");
    }
}

std::vector<string> LispInterpreter::listOfValues(std::vector<string> operands, Environment *env) {
    if (operands.size() == 0) {
        std::vector<string> empty{"NIL"};
        return empty;
    } else {
        std::vector<string> result;
        for (auto const &s: operands) {
            result.push_back(eval(s, env));
        }
        return result;
    }
}


void LispInterpreter::validateExpression(string expression) {
    int parens = 0;
    int i = 0;
    for (int i = 0; i < expression.length(); i++) {
        char c = expression[i];
        if (c == '(') parens++;
        else if (c == ')') parens--;
        if (parens < 0) break;
        i++;
    }
    if (parens != 0) {
        string buffer(" ", i);
        throw std::invalid_argument("Unmatched parens!\n" + expression + "\n" + buffer + "\033[1;31m^\033[0m");
    }
}

std::vector<string> LispInterpreter::procedureParams(std::vector<string> procedure) {
    return stringToVector(unparens(validatedPositionalGetEq(procedure, 4, "PROCEDURE", 1)));
}

string LispInterpreter::procedureBody(std::vector<string> procedure) {
    return validatedPositionalGetEq(procedure, 4, "PROCEDURE", 2);
}

Environment* LispInterpreter::procedureEnv(std::vector<string> procedure) {
    string name = LispInterpreter::validatedPositionalGetEq(procedure, 4, "PROCEDURE", 3);
    return envs[name];
}

string LispInterpreter::applyPrimitiveProcedure(string procedure, std::vector<string> args) {
    return primitiveProcedures[procedure](procedure, args);
}

bool LispInterpreter::isPrimitiveProcedure(string name) {
    return primitiveProcedures.find(name) != primitiveProcedures.end();
}

string LispInterpreter::evalSequence(std::vector<string> expressions, Environment *env) {
    string ret;
    for (auto const &expression: expressions) {
        ret = eval(expression, env);
    }
    return ret;
}

string LispInterpreter::makeProcedure(string parameters, string body, string envName) {
    std::vector<string> procedure{parameters};
    procedure.insert(procedure.begin(), "procedure");
    procedure.push_back(body);
    procedure.push_back(envName);
    return list(procedure);
}

bool LispInterpreter::isTrue(string expression) {
    return expression == "true";
}

bool LispInterpreter::isFalse(string expression) {
    return expression == "false";
}

string LispInterpreter::evalIf(std::vector<string> expression, Environment *env) {
    if (isTrue(eval(ifPredicate(expression), env))) {
        return eval(ifConsequent(expression), env);
    } else {
        return eval(ifAlternative(expression), env);
    }
}

string LispInterpreter::evalAssignment(std::vector<string> expression, Environment *env) {
    string var = assignmentVariable(expression);
    string val = eval(assignmentValue(expression), env);
    env->setVariable(var, val);
    return var + " <- " + val;
}

string LispInterpreter::evalDefinition(std::vector<string> expression, Environment *env) {
    string var = definitionVariable(expression);
    string val = eval(definitionValue(expression), env);
    env->setVariable(var, val);
    return var + " <- " + val;
}

std::vector<string> LispInterpreter::stringToVector(string expressions) {
    std::istringstream ss(expressions);
    string token = Interpreter::nextToken(ss);

    std::vector<string> ret{};
    while (token not_eq "") {
        ret.emplace_back(token);
        token = Interpreter::nextToken(ss);
    }
    return ret;
}

string LispInterpreter::vecToString(std::vector<string> args) {
    string listStr = "";
    for (std::vector<string>::iterator it = args.begin(); it < args.end(); it++) {
        listStr += *it;
        if (it != (args.end() - 1)) listStr += " ";
    }
    return listStr;
}

bool LispInterpreter::isTaggedList(std::vector<string> list, string tag) {
    if (list.size() > 0) {
        return list[0] == tag;
    } else {
        return false;
    }
}

bool LispInterpreter::isDefinition(std::vector<string> expression) {
    return isTaggedList(expression, "define");
}

bool LispInterpreter::isCompoundProcedure(std::vector<string> expression) {
    return isTaggedList(expression, "procedure");
}

string LispInterpreter::definitionVariable(std::vector<string> expression) {
    if (expression.size() != 3) throw std::invalid_argument("Not a valid definition!");
    if (isVariable(expression[1])) {
        return expression[1];
    } else {
        return stringToVector(unparens(expression[1]))[0];
    }
}

std::vector<string> LispInterpreter::definitionValue(std::vector<string> expression) {
    if (expression.size() != 3) throw std::invalid_argument("Not a valid definition!");
    if (isVariable(expression[1])) {
        return stringToVector(unparens(expression[2]));
    } else {
        std::vector<string> shorthand = stringToVector(unparens(expression[1]));
        shorthand.erase(shorthand.begin(), shorthand.begin() + 1);
        return makeLambda(list(shorthand), expression[2]);
    }
}

string LispInterpreter::list(std::vector<string> args) {
    if (args.size() < 1) return "NIL";
    string listStr = "";
    for (auto const &value: args) {
        if (listStr == "")
            listStr += "(";
        else
            listStr += " ";
        listStr += value;
    }
    listStr += ")";
    return listStr;
}

string LispInterpreter::list(string name, std::vector<string> args) {
    return list(args);
}

string LispInterpreter::car(string name, std::vector<string> seq) {
    if (seq[0] == "NIL" or seq[0] == "()") return "NIL";
    return stringToVector(unparens(seq[0]))[0];
}

string LispInterpreter::cdr(string name, std::vector<string> seq) {
    if (seq[0] == "NIL" or seq[0] == "()") return "NIL";
    std::vector<string> unpacked = stringToVector(unparens(seq[0]));
    unpacked.erase(unpacked.begin(), unpacked.begin() + 1);
    return list(unpacked);
}

string LispInterpreter::cons(string name, std::vector<string> seq) {
    if (seq.size() != 2) throw std::invalid_argument("Not a valid pair!");
    return cons(seq[0], seq[1]);
}

string LispInterpreter::cons(string first, string second) {
    if (second == "NIL") {
        return "(" + first + ")";
    } else if (second[0] == '(') {
        std::vector<string> parsedSecond = stringToVector(unparens(second));
        parsedSecond.insert(parsedSecond.begin(), first);
        return list(parsedSecond);
    } else {
        return "(" + first + " " + second + ")";
    }
}

string LispInterpreter::isNull(string name, std::vector<string> seq) {
    return seq.size() == 1 && seq[0] == "NIL" ? "true" : "false";
}

bool LispInterpreter::isNull(string expression) {
    return "NIL" == expression;
}

string LispInterpreter::isEq(string name, std::vector<string> seq) {
    if (seq.size() != 2) throw std::invalid_argument("Not a valid equality statement!");
    return seq[0] == seq[1] ? "true" : "false";
}

string LispInterpreter::arithmetic(string name, std::vector<string> seq) {
    bool castFloat = std::find_if(seq.begin(), seq.end(), [](string s) -> bool { return isFloat(s); }) != seq.end();
    if (castFloat) {
        std::vector<double> nums;
        for (int i = 0; i < seq.size(); i++) nums.push_back(std::stod(seq[i]));
        return arithmetic(name, nums);
    } else {
        std::vector<int> nums;
        for (int i = 0; i < seq.size(); i++) nums.push_back(std::stoi(seq[i]));
        return arithmetic(name, nums);
    }
}

template<typename T>
string LispInterpreter::arithmetic(string name, std::vector<T> seq) {
    T result;
    T (*add)(T, T) = [](T t1, T t2) -> T { return t1 + t2; };
    T (*mult)(T, T) = [](T t1, T t2) -> T { return t1 * t2; };
    if (name == "+") {
        result = reduce(seq, add);
    } else if (name == "-") {
        result = seq[0] - reduce(seq, add, 1);
    } else if (name == "/") {
        result = seq[0] / reduce(seq, mult, 1);
    } else if (name == "*") {
        result = reduce(seq, mult);
    } else {
        throw std::invalid_argument("Not a valid arithmetic operator!");
    }
    return std::to_string(result);
}

string LispInterpreter::boolean(string name, std::vector<string> seq) {
    std::vector<bool> bools;
    for (int i = 0; i < seq.size(); i++) {
        if (seq[i] != "true" and seq[i] != "false") throw std::invalid_argument("Not a valid bool! \"" + seq[i] + "\"");
        bools.push_back(seq[i] == "true");
    }
    bool (*AND)(bool, bool) = [](bool t1, bool t2) -> bool { return t1 and t2; };
    bool (*OR)(bool, bool) = [](bool t1, bool t2) -> bool { return t1 or t2; };
    string result;
    if (name == "and") {
        result = reduce(bools, AND) ? "true" : "false";
    } else if (name == "or") {
        result = reduce(bools, OR) ? "true" : "false";
    } else if (name == "not") {
        if (bools.size() != 1) throw std::invalid_argument("Not a valid not operation!");
        result = bools[0] ? "false" : "true";
    } else {
        throw std::invalid_argument("Not a valid arithmetic operator!");
    }
    return result;
}

bool LispInterpreter::isLambda(std::vector<string> expression) {
    return isTaggedList(expression, "lambda");
}

string LispInterpreter::lambdaParameters(std::vector<string> expression) {
    return LispInterpreter::validatedPositionalGetEq(expression, 3, "LAMBDA", 1);
}

string LispInterpreter::lambdaBody(std::vector<string> expression) {
    return validatedPositionalGetEq(expression, 3, "LAMBDA", 2);
}

std::vector<string> LispInterpreter::makeLambda(string parameters, string body) {
    std::vector<string> lexp{"lambda", parameters};
    lexp.emplace_back(body);
    return lexp;
}

bool LispInterpreter::isAssignment(std::vector<string> expression) {
    return isTaggedList(expression, "set!");
}

string LispInterpreter::assignmentVariable(std::vector<string> expression) {
    return validatedPositionalGetEq(expression, 3, "ASSIGNMENT", 1);
}

string LispInterpreter::assignmentValue(std::vector<string> expression) {
    return validatedPositionalGetEq(expression, 3, "ASSIGNMENT", 2);
}

bool LispInterpreter::isBegin(std::vector<string> expression) {
    return isTaggedList(expression, "begin");
}

std::vector<string> LispInterpreter::beginActions(std::vector<string> expression) {
    std::vector<string> actions(expression.begin() + 1, expression.end());
    return actions;
}

bool LispInterpreter::isCondition(std::vector<string> expression) {
    return isTaggedList(expression, "cond");
}

std::vector<string> LispInterpreter::condClauses(std::vector<string> expression) {
    std::vector<string> clauses(expression.begin() + 1, expression.end());
    return clauses;
}

bool LispInterpreter::isCondElseClause(std::vector<string> expression) {
    return isTaggedList(expression, "else");
}

string LispInterpreter::condPredicate(std::vector<string> expression) {
    return validatedPositionalGetGe(expression, 2, "COND-PREDICATE", 0);
}

std::vector<string> LispInterpreter::condActions(std::vector<string> expression) {
    std::vector<string> actions(expression.begin() + 1, expression.end());
    return actions;
}

string LispInterpreter::condToIf(std::vector<string> expression) {
    return expandClauses(condClauses(expression));
}

string LispInterpreter::expandClauses(std::vector<string> clauses) {
    if (clauses.size() == 0) {
        return "false";
    } else {
        std::vector<string> first = stringToVector(unparens(clauses[0]));
        std::vector<string> rest(clauses.begin() + 1, clauses.end());
        if (isCondElseClause(first)) {
            if (rest.size() == 0) {
                return sequenceToExpression(condActions(first));
            } else {
                throw std::invalid_argument("ELSE clause isn't last -- COND->IF " + list(clauses));
            }
        } else {
            return makeIf(condPredicate(first), sequenceToExpression(condActions(first)), expandClauses(rest));
        }
    }
}

string LispInterpreter::makeIf(string predicate, string consequent, string alternative) {
    std::vector<string> expression{"if", predicate, consequent, alternative};
    return list(expression);
}

string LispInterpreter::sequenceToExpression(std::vector<string> seq) {
    if (seq.size() == 0) {
        return "NIL";
    } else if (seq.size() == 1) {
        return seq[0];
    } else {
        return makeBegin(seq);
    }
}

bool LispInterpreter::isApplication(std::vector<string> expression) {
    return expression.size() > 1;
}

string LispInterpreter::getOperator(std::vector<string> expression) {
    return validatedPositionalGetGe(expression, 1, "OPERATOR-STATEMENT", 0);
}

std::vector<string> LispInterpreter::getOperands(std::vector<string> expression) {
    std::vector<string> operands(expression.begin() + 1, expression.end());
    return operands;
}

string LispInterpreter::makeBegin(std::vector<string> seq) {
    std::vector<string> begin{seq};
    begin.insert(begin.begin(), "begin");
    return list(begin);
}

bool LispInterpreter::isIf(std::vector<string> expression) {
    return isTaggedList(expression, "if");
}

string LispInterpreter::ifPredicate(std::vector<string> expression) {
    return validatedPositionalGetEq(expression, 4, "IF", 1);
}

string LispInterpreter::ifConsequent(std::vector<string> expression) {
    return validatedPositionalGetEq(expression, 4, "IF", 2);
}

string LispInterpreter::ifAlternative(std::vector<string> expression) {
    return validatedPositionalGetEq(expression, 4, "IF", 3);
}

string LispInterpreter::validatedPositionalGetEq(std::vector<string> expression, int length, string name, int pos) {
    if (expression.size() != length) throw std::invalid_argument("Not a valid " + name + "!");
    return expression[pos];
}

string LispInterpreter::validatedPositionalGetGe(std::vector<string> expression, int length, string name, int pos) {
    if (expression.size() < length) throw std::invalid_argument("Not a valid " + name + "!");
    return expression[pos];
}

bool LispInterpreter::isSelfEvaluating(string expression) {
    if (isTrue(expression) or isFalse(expression) or isNull(expression)) {
        return true;
    } else if (isSymbol(expression)) {
        return true;
    } else if (isFloat(expression) or isInt(expression)) {
        return true;
    } else if (isQuotedString(expression)) {
        return true;
    } else {
        return false;
    }
}

bool LispInterpreter::isString(string expression, bool withQuotes) {
    if (expression.length() > 0 and expression[0] == '(') return false;
    bool escapeActive = false;
    for (int i = 0; i < expression.length(); i++) {
        char c = expression[i];
        if (i == 0 or i == expression.length() - 1) {
            if (withQuotes and (escapeActive or c != '"')) return false;
        } else if (c == '"' and not escapeActive) {
            return false;
        }
        escapeActive = not escapeActive and c == '\\';
    }
    return true;
}

bool LispInterpreter::isQuotedString(string expression) {
    return isString(expression, true);
}

string LispInterpreter::textOfQuotedString(string expression) {
    if (expression.length() < 2 or expression[0] != '"' or expression.back() != '"')
        throw std::invalid_argument("Not a quoted string!");
    return expression.substr(1, expression.length() - 2);
}

bool LispInterpreter::isSymbol(string expression) {
    return simpleMatch("'[a-zA-Z0-9]+", expression);
}

bool LispInterpreter::isVariable(string expression) {
    return simpleMatch("[a-zA-Z0-9_-]+", expression);
}

bool LispInterpreter::isInt(string expression) {
    return simpleMatch("[0-9]+", expression);
}

bool LispInterpreter::isFloat(string expression) {
    return simpleMatch("[0-9]+[.][0-9]+", expression);
}
