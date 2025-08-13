//
// Updated tests for AST-based interpreter
//

#include <gtest/gtest.h>
#include "ast.h"
#include "interpreter.h"
#include "environment.h"

TEST(Environment, SetAndGet) {
    Environment env("test");
    env.setVariable("x", SExpr(1.0));
    SExpr val = env.getVariable("x");
    ASSERT_TRUE(val.isNumber());
    EXPECT_DOUBLE_EQ(std::get<double>(val.value), 1.0);
}

TEST(Parser, SimpleList) {
    SExpr ast = parse("(+ 1 2)");
    ASSERT_TRUE(ast.isList());
    auto list = std::get<SExpr::List>(ast.value);
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(std::get<std::string>(list[0].value), "+");
    EXPECT_DOUBLE_EQ(std::get<double>(list[1].value), 1);
    EXPECT_DOUBLE_EQ(std::get<double>(list[2].value), 2);
}

TEST(Interpreter, Arithmetic) {
    Environment env("global");
    Interpreter interp(&env);
    SExpr result = interp.eval("(+ 1 2)");
    ASSERT_TRUE(result.isNumber());
    EXPECT_DOUBLE_EQ(std::get<double>(result.value), 3);
}

TEST(Interpreter, LambdaApplication) {
    Environment env("global");
    Interpreter interp(&env);
    interp.eval("(define add (lambda (a b) (+ a b)))");
    SExpr result = interp.eval("(add 4 5)");
    ASSERT_TRUE(result.isNumber());
    EXPECT_DOUBLE_EQ(std::get<double>(result.value), 9);
}

TEST(Interpreter, BuiltinsAndSet) {
    Environment env("global");
    Interpreter interp(&env);

    interp.eval("(set! x 42)");
    SExpr x = interp.eval("x");
    ASSERT_TRUE(x.isNumber());
    EXPECT_DOUBLE_EQ(std::get<double>(x.value), 42);

    SExpr andRes = interp.eval("(and true false)");
    ASSERT_TRUE(andRes.isSymbol());
    EXPECT_EQ(std::get<std::string>(andRes.value), "false");

    SExpr listRes = interp.eval("(list 1 2 3)");
    ASSERT_TRUE(listRes.isList());
    EXPECT_EQ(toString(listRes), "(1 2 3)");

    SExpr carRes = interp.eval("(car (list 1 2 3))");
    ASSERT_TRUE(carRes.isNumber());
    EXPECT_DOUBLE_EQ(std::get<double>(carRes.value), 1);

    SExpr cdrRes = interp.eval("(cdr (list 1 2 3))");
    ASSERT_TRUE(cdrRes.isList());
    EXPECT_EQ(toString(cdrRes), "(2 3)");

    SExpr consRes1 = interp.eval("(cons 1 2)");
    ASSERT_TRUE(consRes1.isList());
    EXPECT_EQ(toString(consRes1), "(1 2)");

    SExpr consRes2 = interp.eval("(cons 1 (list 2 3))");
    ASSERT_TRUE(consRes2.isList());
    EXPECT_EQ(toString(consRes2), "(1 2 3)");

    interp.eval("(define map (lambda (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst))))))");
    SExpr mapRes = interp.eval("(map (lambda (x) (eq? x 1)) (list 1 2 1 2))");
    ASSERT_TRUE(mapRes.isList());
    EXPECT_EQ(toString(mapRes), "(true false true false)");
}

