//
// Created by Matthew Weiden on 7/15/17.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include "interpreter.h"
#include "environment.h"

using std::string;


TEST(Environment, SettersAndGettersTest) {
    Environment env = Environment("test");
    env.setVariable("'int", "1");
    EXPECT_EQ(env.getVariable("'int"), "1");
}

TEST(Environment, SymbolKeyStoresStringValue) {
    Environment env("test");
    Symbol name = stringToSymbol("'foo");
    env.setVariable(name, "bar");
    EXPECT_EQ(env.getVariable(name), "bar");
}

TEST(Interpreter, NextTokenTest) {
    // should not chomp when there's no leading whitespace
    std::istringstream ss1("token ");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss1)), "token");

    // should end on \0
    std::istringstream ss2("  token");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss2)), "token");

    // should end on whitespace
    std::istringstream ss3("  token\n");
    std::istringstream ss4(" token token2");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss3)), "token");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss4)), "token");

    // should end on EOF
    const char tokenStr[] = {' ', ' ', 't', 'o', 'k', 'e', 'n', EOF};
    std::istringstream ss5(tokenStr);
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss5)), "token");

    // should respect perens
    std::istringstream ss6("(hi hello bye goodbye)");
    std::istringstream ss7("(hi hello) (bye goodbye)");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss6)), "(hi hello bye goodbye)");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss7)), "(hi hello)");
    EXPECT_EQ(symbolToString(Interpreter::nextToken(ss7)), "(bye goodbye)");
}


TEST(Interpreter, UnparensTest) {
    EXPECT_EQ(Interpreter::unparens("(abc)"), "abc");
}

TEST(LispInterpreter, StringToVectorTest) {
    std::vector<string> expected{"one", "two", "three"};
    EXPECT_EQ(LispInterpreter::stringToVector("one two three"), expected);
}

TEST(LispInterpreter, IsNumTest) {
    EXPECT_TRUE(LispInterpreter::isInt("1"));
    EXPECT_FALSE(LispInterpreter::isInt("1."));
    EXPECT_FALSE(LispInterpreter::isInt("1.0"));
    EXPECT_FALSE(LispInterpreter::isInt(".1"));
    EXPECT_FALSE(LispInterpreter::isInt("0.1"));
    EXPECT_FALSE(LispInterpreter::isInt("1..0"));
    EXPECT_FALSE(LispInterpreter::isFloat("1"));
    EXPECT_FALSE(LispInterpreter::isFloat("1."));
    EXPECT_TRUE(LispInterpreter::isFloat("1.0"));
    EXPECT_FALSE(LispInterpreter::isFloat(".1"));
    EXPECT_TRUE(LispInterpreter::isFloat("0.1"));
    EXPECT_FALSE(LispInterpreter::isFloat("1..0"));
}

TEST(LispInterpreter, ArithmeticEmptySequenceThrows) {
    std::vector<string> empty;
    EXPECT_THROW(LispInterpreter::arithmetic("+", empty), std::invalid_argument);
}

TEST(LispInterpreter, IsQuotedStringTest) {
    EXPECT_FALSE(LispInterpreter::isQuotedString("abc"));
    EXPECT_TRUE(LispInterpreter::isQuotedString("\"abc\""));
    EXPECT_FALSE(LispInterpreter::isQuotedString("\"ab\"c\""));
    EXPECT_TRUE(LispInterpreter::isQuotedString("\"ab\\\"c\""));
}

TEST(LispInterpreter, IsStringTest) {
    EXPECT_TRUE(LispInterpreter::isString("abc", false));
    EXPECT_TRUE(LispInterpreter::isQuotedString("\"abc\""));
    EXPECT_FALSE(LispInterpreter::isQuotedString("\"ab\"c\""));
    EXPECT_TRUE(LispInterpreter::isQuotedString("\"ab\\\"c\""));
}

TEST(LispInterpreter, IsSelfEvaluatingTest) {
    EXPECT_TRUE(LispInterpreter::isSelfEvaluating("\"abc\""));
    EXPECT_FALSE(LispInterpreter::isSelfEvaluating("a\"bc"));
    EXPECT_TRUE(LispInterpreter::isSelfEvaluating("1.0"));
    EXPECT_FALSE(LispInterpreter::isSelfEvaluating(".0"));
}

TEST(LispInterpreter, IsSymbolTest) {
    EXPECT_TRUE(LispInterpreter::isSymbol("'ab3"));
    EXPECT_FALSE(LispInterpreter::isSymbol("ab'"));
}

TEST(LispInterpreter, IsTaggedList) {
    std::vector<string> list{"set!", "x", "1"};
    EXPECT_TRUE(LispInterpreter::isTaggedList(list, "set!"));
    EXPECT_FALSE(LispInterpreter::isTaggedList(list, "matt!"));
}

TEST(LispInterpreter, IsAssignment) {
    std::vector<string> vec1{"set!", "x", "1"};
    std::vector<string> vec2{"matt!", "x", "1"};
    EXPECT_TRUE(LispInterpreter::isAssignment(vec1));
    EXPECT_FALSE(LispInterpreter::isAssignment(vec2));
}

TEST(LispInterpreter, VariableValue) {
    std::vector<string> vec{"set!", "'x", "1"};
    EXPECT_EQ(LispInterpreter::assignmentVariable(vec), "'x");
    EXPECT_EQ(LispInterpreter::assignmentValue(vec), "1");
}

TEST(LispInterpreter, IsDefinitionTest) {
    std::vector<string> vec1{"define", "(foo param1 param2)", "'ok"};
    std::vector<string> vec2{"definez", "(foo param1 param2)", "'ok"};
    EXPECT_EQ(LispInterpreter::isDefinition(vec1), true);
    EXPECT_EQ(LispInterpreter::isDefinition(vec2), false);
}

TEST(LispInterpreter, DefinitionVariableTest) {
    std::vector<string> vec1{"define", "var1", "(lambda (param1 param2) 'ok)"};
    std::vector<string> vec2{"define", "(var2 param1 param2)", "'ok"};
    std::vector<string> vec3{"define", "(foo param1 param2)"};
    EXPECT_EQ(LispInterpreter::definitionVariable(vec1), "var1");
    EXPECT_EQ(LispInterpreter::definitionVariable(vec2), "var2");
    EXPECT_ANY_THROW(LispInterpreter::definitionVariable(vec3));
}

TEST(LispInterpreter, DefinitionValueTest) {
    std::vector<string> vec1{"define", "var1", "(lambda (param1 param2) 'ok)"};
    std::vector<string> vec2{"define", "(var1 param1 param2)", "'ok"};
    std::vector<string> expected{"lambda", "(param1 param2)", "'ok"};
    EXPECT_EQ(LispInterpreter::definitionValue(vec1), expected);
    EXPECT_EQ(LispInterpreter::definitionValue(vec2), expected);
}

TEST(LispInterpreter, IsLambdaTest) {
    std::vector<string> vec1{"lambda", "(param1 param2)", "'ok"};
    std::vector<string> vec2{"lambdaz", "(param1 param2)", "'ok"};
    EXPECT_EQ(LispInterpreter::isLambda(vec1), true);
    EXPECT_EQ(LispInterpreter::isLambda(vec2), false);
}

TEST(LispInterpreter, LambdaParametersTest) {
    std::vector<string> vec1{"lambda", "(param1 param2)", "'ok"};
    std::vector<string> vec2{"lambda", "(param1 param2)"};
    EXPECT_EQ(LispInterpreter::lambdaParameters(vec1), "(param1 param2)");
    EXPECT_ANY_THROW(LispInterpreter::lambdaParameters(vec2));
}

TEST(LispInterpreter, LambdaBodyTest) {
    std::vector<string> vec1{"lambda", "(param1 param2)", "'ok"};
    std::vector<string> vec2{"lambda", "(param1 param2)"};
    EXPECT_EQ(LispInterpreter::lambdaBody(vec1), "'ok");
    EXPECT_ANY_THROW(LispInterpreter::lambdaBody(vec2));
}

TEST(LispInterpreter, MakeLambdaTest) {
    std::vector<string> expected{"lambda", "(param1 param2)", "'ok"};
    EXPECT_EQ(LispInterpreter::makeLambda("(param1 param2)", "'ok"), expected);
}

TEST(LispInterpreter, IsIfTest) {
    std::vector<string> vec1{"if", "(symbol? (cadr exp))", "(cadr exp)", "(caadr exp)"};
    std::vector<string> vec2{"iff", "(symbol? (cadr exp))", "(cadr exp)", "(caadr exp)"};
    EXPECT_TRUE(LispInterpreter::isIf(vec1));
    EXPECT_FALSE(LispInterpreter::isIf(vec2));
}

TEST(LispInterpreter, IfPredicateTest) {
    std::vector<string> vec1{"if", "(symbol? (cadr exp))", "(cadr exp)", "(caadr exp)"};
    std::vector<string> vec2{"if", "(symbol? (cadr exp))", "(cadr exp)"};
    EXPECT_EQ(LispInterpreter::ifPredicate(vec1), "(symbol? (cadr exp))");
    EXPECT_ANY_THROW(LispInterpreter::ifPredicate(vec2));
}

TEST(LispInterpreter, IfConsequentTest) {
    std::vector<string> vec1{"if", "(symbol? (cadr exp))", "(cadr exp)", "(caadr exp)"};
    std::vector<string> vec2{"if", "(symbol? (cadr exp))", "(cadr exp)"};
    EXPECT_EQ(LispInterpreter::ifConsequent(vec1), "(cadr exp)");
    EXPECT_ANY_THROW(LispInterpreter::ifConsequent(vec2));
}

TEST(LispInterpreter, IfAlternativeTest) {
    std::vector<string> vec1{"if", "(symbol? (cadr exp))", "(cadr exp)", "(caadr exp)"};
    std::vector<string> vec2{"if", "(symbol? (cadr exp))", "(cadr exp)"};
    EXPECT_EQ(LispInterpreter::ifAlternative(vec1), "(caadr exp)");
    EXPECT_ANY_THROW(LispInterpreter::ifAlternative(vec2));
}

TEST(LispInterpreter, TextOfQuotedStringTest) {
    EXPECT_EQ(LispInterpreter::textOfQuotedString("\"abc\""), "abc");
    EXPECT_ANY_THROW(LispInterpreter::textOfQuotedString("abc"));
}

TEST(LispInterpreter, MakeProcedureTest) {
    EXPECT_EQ(LispInterpreter::makeProcedure("(param)", "body", "env"), "(procedure (param) body env)");
}

TEST(LispInterpreter, ProcedureBodyTest) {
    std::vector<string> procedure{"procedure", "(op lst)", "(cons (op (car lst)) (map op (cdr lst)))", "test"};
    EXPECT_EQ(LispInterpreter::procedureBody(procedure), "(cons (op (car lst)) (map op (cdr lst)))");
}

TEST(LispInterpreter, IsPrimitiveProcedureTest) {
    Environment env = Environment("test");
    env.setVariable("x", "1");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec{"1", "2"};
    std::vector<string> stored{"(1 2)"};
    std::vector<string> null{"NIL"};
    EXPECT_EQ(intr.applyPrimitiveProcedure("list", vec), "(1 2)");
    EXPECT_EQ(intr.applyPrimitiveProcedure("cons", vec), "(1 2)");
    EXPECT_EQ(intr.applyPrimitiveProcedure("car", stored), "1");
    EXPECT_EQ(intr.applyPrimitiveProcedure("cdr", stored), "(2)");
    EXPECT_EQ(intr.applyPrimitiveProcedure("null?", vec), "false");
    EXPECT_EQ(intr.applyPrimitiveProcedure("null?", null), "true");
}

TEST(LispInterpreter, ApplyPrimitiveProcedureTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    EXPECT_TRUE(intr.isPrimitiveProcedure("car"));
    EXPECT_FALSE(intr.isPrimitiveProcedure("carr"));
}

TEST(LispInterpreter, EvalSequenceTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec{"false", "true"};
    EXPECT_EQ(intr.evalSequence(vec, &env), "true");
}

TEST(LispInterpreter, EvalSequenceEmptyTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec{};
    EXPECT_EQ(intr.evalSequence(vec, &env), "NIL");
}

TEST(LispInterpreter, EvalAssignmentTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec{"set!", "x", "1"};
    intr.evalAssignment(vec, &env);
    EXPECT_EQ(env.getVariable("x"), "1");
}

TEST(LispInterpreter, EvalDefinitionTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec{"define", "(application? exp)", "(pair? exp)"};
    EXPECT_EQ(intr.evalDefinition(vec, &env), "application? <- (procedure (exp) (pair? exp) test)");
    EXPECT_EQ(env.getVariable("application?"), "(procedure (exp) (pair? exp) test)");
}

TEST(LispInterpreter, IsBeginTest) {
    std::vector<string> vec{"begin", "(+ 1 2)"};
    EXPECT_TRUE(LispInterpreter::isBegin(vec));
}

TEST(LispInterpreter, BeginActionsTest) {
    std::vector<string> vec{"begin", "(+ 1 2)", "(+ 2 3)"};
    std::vector<string> expected{"(+ 1 2)", "(+ 2 3)"};
    EXPECT_EQ(LispInterpreter::beginActions(vec), expected);
}

TEST(LispInterpreter, IsConditionTest) {
    std::vector<string> vec{"cond", "((eq? 1 1) 1)", "((eq? 2 3) 3)"};
    EXPECT_TRUE(LispInterpreter::isCondition(vec));
}

TEST(LispInterpreter, ConditionPredicateTest) {
    std::vector<string> vec{"(eq? 1 1)", "1"};
    EXPECT_EQ(LispInterpreter::condPredicate(vec), "(eq? 1 1)");
    vec.pop_back();
    EXPECT_ANY_THROW(LispInterpreter::condPredicate(vec));
}

TEST(LispInterpreter, ConditionClausesTest) {
    std::vector<string> vec{"cond", "((eq? 1 1) 1)", "((eq? 2 3) 3)"};
    std::vector<string> expected{"((eq? 1 1) 1)", "((eq? 2 3) 3)"};
    EXPECT_EQ(LispInterpreter::condClauses(vec), expected);
}

TEST(LispInterpreter, ConditionActionsTest) {
    std::vector<string> vec{"(eq? 1 1)", "1"};
    std::vector<string> expected{"1"};
    EXPECT_EQ(LispInterpreter::condActions(vec), expected);
}

TEST(LispInterpreter, IsConditionElseClauseTest) {
    std::vector<string> vec1{"else", "(scan (cdr vars) (cdr vals))"};
    std::vector<string> vec2{"esle", "(scan (cdr vars) (cdr vals))"};
    EXPECT_TRUE(LispInterpreter::isCondElseClause(vec1));
    EXPECT_FALSE(LispInterpreter::isCondElseClause(vec2));
}

TEST(LispInterpreter, CondToIfTest) {
    std::vector<string> vec{"cond", "((eq? 1 1) 1)", "((eq? 2 3) 3)"};
    EXPECT_EQ(LispInterpreter::condToIf(vec), "(if (eq? 1 1) 1 (if (eq? 2 3) 3 false))");
}

TEST(LispInterpreter, ExpandClausesTest) {
    std::vector<string> vec{"((eq? 1 1) 1)", "((eq? 2 3) 3)"};
    EXPECT_EQ(LispInterpreter::expandClauses(vec), "(if (eq? 1 1) 1 (if (eq? 2 3) 3 false))");
}

TEST(LispInterpreter, MakeIfTest) {
    EXPECT_EQ(LispInterpreter::makeIf("(symbol? x)", "true", "false"), "(if (symbol? x) true false)");
}

TEST(LispInterpreter, SequenceToExpressionTest) {
    std::vector<string> empty{};
    std::vector<string> vec1{"true"};
    std::vector<string> vec2{"(eval 1)", "(eval 2)"};
    EXPECT_EQ(LispInterpreter::sequenceToExpression(empty), "NIL");
    EXPECT_EQ(LispInterpreter::sequenceToExpression(vec1), "true");
    EXPECT_EQ(LispInterpreter::sequenceToExpression(vec2), "(begin (eval 1) (eval 2))");
}

TEST(LispInterpreter, IsApplicationTest) {
    std::vector<string> vec{"(eval 1)", "(eval 2)"};
    EXPECT_TRUE(LispInterpreter::isApplication(vec));
}

TEST(LispInterpreter, GetOperatorTest) {
    std::vector<string> vec{"+", "1", "2"};
    EXPECT_EQ(LispInterpreter::getOperator(vec), "+");
}

TEST(LispInterpreter, GetOperandsTest) {
    std::vector<string> vec{"+", "1", "2"};
    std::vector<string> operands{"1", "2"};
    EXPECT_EQ(LispInterpreter::getOperands(vec), operands);
}

TEST(LispInterpreter, MakeBeginTest) {
    std::vector<string> vec{"(eval 1)", "(eval 2)"};
    EXPECT_EQ(LispInterpreter::makeBegin(vec), "(begin (eval 1) (eval 2))");
}

TEST(LispInterpreter, EvalIfTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec1{"if", "true", "1", "2"};
    std::vector<string> vec2{"if", "false", "1", "2"};
    EXPECT_EQ(intr.evalIf(vec1, &env), "1");
    EXPECT_EQ(intr.evalIf(vec2, &env), "2");
}

TEST(LispInterpreter, EvalListOfValuesTest) {
    Environment env = Environment("test");
    env.setVariable("'x", "1");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> vec{"(if (eq? 1 1) 1 2)", "2"};
    std::vector<string> expected{"1", "2"};
    EXPECT_EQ(intr.listOfValues(vec, &env), expected);
}

TEST(LispInterpreter, EvalTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    EXPECT_EQ(intr.eval("eq?", &env), "eq?");
    EXPECT_EQ(intr.eval("(cons 1 2)", &env), "(1 2)");
    EXPECT_EQ(intr.eval("'ok", &env), "'ok");
    EXPECT_EQ(intr.eval("NIL", &env), "NIL");
}

TEST(LispInterpreter, ApplyTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    std::vector<string> args{"1", "1"};
    EXPECT_EQ(intr.apply("eq?", args), "true");
}

TEST(LispInterpreter, SmokeTest) {
    Environment env = Environment("test");
    LispInterpreter intr = LispInterpreter(&env);
    EXPECT_EQ(intr.eval("(+ 1 1)"), "2");
    EXPECT_EQ(intr.eval("(+ 1.0 1)"), "2.000000");
    EXPECT_EQ(intr.eval("(not false)"), "true");
    EXPECT_EQ(intr.eval("(or false true)"), "true");
    EXPECT_EQ(intr.eval("(and false true)"), "false");
    EXPECT_EQ(intr.eval("(car (list 1 2))"), "1");
    EXPECT_EQ(intr.eval("(cdr (list 1 2))"), "(2)");
    EXPECT_EQ(
            intr.eval("(define map (lambda (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst))))))"),
            "map <- (procedure (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst)))) test)"
    );
    EXPECT_EQ(intr.eval("(map (lambda (x) (eq? x 1)) (list 1 2 1 2))"), "(true false true false)");
}
