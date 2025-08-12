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

