#include <gtest/gtest.h>
#include "ast.h"
#include "interpreter.h"
#include "environment.h"

TEST(Parser, ParseNumber){
    SExpr e = parse("42");
    ASSERT_EQ(e.type, SExpr::Type::NUMBER);
    EXPECT_DOUBLE_EQ(e.number, 42);
}

TEST(Parser, ParseSymbol){
    SExpr e = parse("foo");
    ASSERT_EQ(e.type, SExpr::Type::SYMBOL);
    EXPECT_EQ(symbolToString(e.sym), "foo");
}

TEST(Parser, ParseList){
    SExpr e = parse("(a b 3)");
    ASSERT_EQ(e.type, SExpr::Type::LIST);
    ASSERT_EQ(e.list.size(), 3u);
    EXPECT_EQ(symbolToString(e.list[0].sym), "a");
}

TEST(Interpreter, EvalArithmetic){
    Environment env("global");
    LispInterpreter intr(&env);
    SExpr r = intr.eval("(+ 1 2 3)");
    ASSERT_EQ(r.type, SExpr::Type::NUMBER);
    EXPECT_DOUBLE_EQ(r.number, 6);
}

TEST(Interpreter, Define){
    Environment env("global");
    LispInterpreter intr(&env);
    intr.eval("(define x 5)");
    SExpr r = intr.eval("(+ x 1)");
    ASSERT_EQ(r.type, SExpr::Type::NUMBER);
    EXPECT_DOUBLE_EQ(r.number, 6);
}
