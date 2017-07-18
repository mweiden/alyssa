//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>
#include <vector>
#include <string>
#include "environment.h"

using std::string;
using std::map;


class Interpreter {
public:
    static string nextToken(std::istream &in);

    static string unparens(string expression);

    virtual string eval(string expression) = 0;

    virtual string apply(string procedure, std::vector<string> arguments) = 0;
};


class LispInterpreter : public Interpreter {
public:
    LispInterpreter(Environment *_env);

    ~LispInterpreter();

    // eval & apply
    string eval(string expression);

    string eval(string expression, Environment *env);

    string eval(std::vector<string> expression, Environment *env);

    string apply(string procedure, std::vector<string> arguments);

    // sub-functions of eval and apply
    string evalIf(std::vector<string> expression, Environment *env);

    string evalAssignment(std::vector<string> expression, Environment *env);

    string evalDefinition(std::vector<string> expression, Environment *env);

    string evalSequence(std::vector<string> expressions, Environment *env);

    std::vector<string> listOfValues(std::vector<string> operands, Environment *env);

    string applyPrimitiveProcedure(string procedure, std::vector<string> args);

    // env management
    Environment *extendEnvironment(std::vector<string> vars, std::vector<string> vals, Environment *env);

    // validataion
    static void validateExpression(string expression);

    bool isPrimitiveProcedure(string name);

    static bool isTrue(string expression);

    static bool isFalse(string expression);

    static bool isTaggedList(std::vector<string> list, string tag);

    static bool isDefinition(std::vector<string> expression);

    static bool isCompoundProcedure(std::vector<string> expression);

    static string isNull(string name, std::vector<string> seq);

    static bool isNull(string expression);

    static string isEq(string name, std::vector<string> seq);

    static bool isLambda(std::vector<string> expression);

    static bool isAssignment(std::vector<string> expression);

    static bool isBegin(std::vector<string> expression);

    static bool isCondition(std::vector<string> expression);

    static bool isSymbol(string expression);

    static bool isVariable(string expression);

    static bool isInt(string expression);

    static bool isFloat(string expression);

    static bool isCondElseClause(std::vector<string> expression);

    static bool isString(string expression, bool withQuotes);

    static bool isQuotedString(string expression);

    static bool isApplication(std::vector<string> expression);

    static bool isIf(std::vector<string> expression);

    static bool isSelfEvaluating(string expression);

    // parsing
    static std::vector<string> stringToVector(string expressions);

    static string vecToString(std::vector<string> args);

    static std::vector<string> procedureParams(std::vector<string> procedure);

    static string procedureBody(std::vector<string> procedure);

    static string makeProcedure(string parameters, string body, string envName);

    Environment *procedureEnv(std::vector<string> procedure);

    static string definitionVariable(std::vector<string> expression);

    static std::vector<string> definitionValue(std::vector<string> expression);

    static string list(std::vector<string> args);

    static string list(string name, std::vector<string> args);

    static string lambdaParameters(std::vector<string> expression);

    static string lambdaBody(std::vector<string> expression);

    static std::vector<string> makeLambda(string parameters, string body);

    static string assignmentVariable(std::vector<string> expression);

    static string assignmentValue(std::vector<string> expression);

    static std::vector<string> beginActions(std::vector<string> expression);

    static std::vector<string> condClauses(std::vector<string> expression);

    static std::vector<string> condActions(std::vector<string> expression);

    static string ifPredicate(std::vector<string> expression);

    static string ifConsequent(std::vector<string> expression);

    static string ifAlternative(std::vector<string> expression);

    static string condPredicate(std::vector<string> expression);

    static string condToIf(std::vector<string> expression);

    static string expandClauses(std::vector<string> clauses);

    static string makeIf(string predicate, string consequent, string alternative);

    static string sequenceToExpression(std::vector<string> seq);

    static string getOperator(std::vector<string> expression);

    static std::vector<string> getOperands(std::vector<string> expression);

    static string makeBegin(std::vector<string> seq);

    static string textOfQuotedString(string expression);

    static string validatedPositionalGetEq(std::vector<string> expression, int length, string name, int pos);

    static string validatedPositionalGetGe(std::vector<string> expression, int length, string name, int pos);

    // primitive functions
    static string car(string name, std::vector<string> seq);

    static string cdr(string name, std::vector<string> seq);

    static string cons(string name, std::vector<string> seq);

    static string cons(string first, string second);

    static string arithmetic(string name, std::vector<string> seq);

    template <typename T> static string arithmetic(string name, std::vector<T> seq);

    static string boolean(string name, std::vector<string> seq);

protected:
    unsigned long frameCount = 0L;
    string globalEnvName;
    std::map<string, Environment *> envs;

    std::map<string, string (*)(string,std::vector<string>)> primitiveProcedures;
};

#endif //INTERPRETER_H
