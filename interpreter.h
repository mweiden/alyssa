//
// Created by Matthew Weiden on 7/15/17.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "environment.h"
#include "symbol_table.h"

using std::string;
using std::map;


class Interpreter {
public:
    static Symbol nextToken(std::istream &in);

    static string unparens(const string &expression);

    virtual string eval(const string &expression) = 0;

    virtual string apply(const string &procedure,
                         const std::vector<string> &arguments) = 0;
};


class LispInterpreter : public Interpreter {
public:
    LispInterpreter(Environment *_env);

    ~LispInterpreter();

    // eval & apply
    string eval(const string &expression);

    string eval(const string &expression, Environment *env);

    string eval(const std::vector<string> &expression, Environment *env);

    string apply(const string &procedure,
                 const std::vector<string> &arguments);

    // sub-functions of eval and apply
    string evalIf(const std::vector<string> &expression, Environment *env);

    string evalAssignment(const std::vector<string> &expression,
                          Environment *env);

    string evalDefinition(const std::vector<string> &expression,
                          Environment *env);

    string evalSequence(const std::vector<string> &expressions,
                        Environment *env);

    std::vector<string> listOfValues(const std::vector<string> &operands,
                                     Environment *env);

    string applyPrimitiveProcedure(const string &procedure,
                                   const std::vector<string> &args);

    // env management
    std::unique_ptr<Environment> extendEnvironment(
            const std::vector<string> &vars,
            const std::vector<string> &vals,
            Environment *env);

    // validataion
    static void validateExpression(const string &expression);

    bool isPrimitiveProcedure(const string &name);

    static bool isTrue(const string &expression);

    static bool isFalse(const string &expression);

    static bool isTaggedList(const std::vector<string> &list,
                             const string &tag);

    static bool isDefinition(const std::vector<string> &expression);

    static bool isCompoundProcedure(const std::vector<string> &expression);

    static string isNull(const string &name,
                         const std::vector<string> &seq);

    static bool isNull(const string &expression);

    static string isEq(const string &name,
                       const std::vector<string> &seq);

    static bool isLambda(const std::vector<string> &expression);

    static bool isAssignment(const std::vector<string> &expression);

    static bool isBegin(const std::vector<string> &expression);

    static bool isCondition(const std::vector<string> &expression);

    static bool isSymbol(const string &expression);

    static bool isVariable(const string &expression);

    static bool isInt(const string &expression);

    static bool isFloat(const string &expression);

    static bool isCondElseClause(const std::vector<string> &expression);

    static bool isString(const string &expression, bool withQuotes);

    static bool isQuotedString(const string &expression);

    static bool isApplication(const std::vector<string> &expression);

    static bool isIf(const std::vector<string> &expression);

    static bool isSelfEvaluating(const string &expression);

    // parsing
    static std::vector<string> stringToVector(const string &expressions);

    static string vecToString(const std::vector<string> &args);

    static std::vector<string> procedureParams(const std::vector<string> &procedure);

    static string procedureBody(const std::vector<string> &procedure);

    static string makeProcedure(const string &parameters,
                                const string &body,
                                const string &envName);

    Environment *procedureEnv(const std::vector<string> &procedure);

    static string definitionVariable(const std::vector<string> &expression);

    static std::vector<string> definitionValue(const std::vector<string> &expression);

    static string list(const std::vector<string> &args);

    static string list(const string &name, const std::vector<string> &args);

    static string lambdaParameters(const std::vector<string> &expression);

    static string lambdaBody(const std::vector<string> &expression);

    static std::vector<string> makeLambda(const string &parameters,
                                          const string &body);

    static string assignmentVariable(const std::vector<string> &expression);

    static string assignmentValue(const std::vector<string> &expression);

    static std::vector<string> beginActions(const std::vector<string> &expression);

    static std::vector<string> condClauses(const std::vector<string> &expression);

    static std::vector<string> condActions(const std::vector<string> &expression);

    static string ifPredicate(const std::vector<string> &expression);

    static string ifConsequent(const std::vector<string> &expression);

    static string ifAlternative(const std::vector<string> &expression);

    static string condPredicate(const std::vector<string> &expression);

    static string condToIf(const std::vector<string> &expression);

    static string expandClauses(const std::vector<string> &clauses);

    static string makeIf(const string &predicate,
                         const string &consequent,
                         const string &alternative);

    static string sequenceToExpression(const std::vector<string> &seq);

    static string getOperator(const std::vector<string> &expression);

    static std::vector<string> getOperands(const std::vector<string> &expression);

    static string makeBegin(const std::vector<string> &seq);

    static string textOfQuotedString(const string &expression);

    static string validatedPositionalGetEq(const std::vector<string> &expression,
                                           int length,
                                           const string &name,
                                           int pos);

    static string validatedPositionalGetGe(const std::vector<string> &expression,
                                           int length,
                                           const string &name,
                                           int pos);

    // primitive functions
    static string car(const string &name, const std::vector<string> &seq);

    static string cdr(const string &name, const std::vector<string> &seq);

    static string cons(const string &name, const std::vector<string> &seq);

    static string cons(const string &first, const string &second);

    static string arithmetic(const string &name,
                             const std::vector<string> &seq);

    template <typename T> static string arithmetic(const string &name,
                                                   const std::vector<T> &seq);

    static string boolean(const string &name,
                          const std::vector<string> &seq);

protected:
    unsigned long frameCount = 0L;
    string globalEnvName;
    std::map<string, Environment *> envs;

    std::map<string, string (*)(const string&, const std::vector<string>&)> primitiveProcedures;
};

#endif //INTERPRETER_H
