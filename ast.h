#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include "symbol_table.h"

struct SExpr {
    enum class Type { SYMBOL, NUMBER, LIST, NIL };
    Type type;
    Symbol sym;      // valid when type == SYMBOL
    double number;   // valid when type == NUMBER
    std::vector<SExpr> list; // valid when type == LIST

    SExpr() : type(Type::NIL), sym(0), number(0) {}
};

inline SExpr makeSymbol(const std::string &name){
    SExpr e; e.type = SExpr::Type::SYMBOL; e.sym = stringToSymbol(name); return e;
}
inline SExpr makeSymbol(Symbol s){ SExpr e; e.type = SExpr::Type::SYMBOL; e.sym = s; return e; }
inline SExpr makeNumber(double n){ SExpr e; e.type = SExpr::Type::NUMBER; e.number = n; return e; }
inline SExpr makeList(const std::vector<SExpr> &lst){ SExpr e; e.type=SExpr::Type::LIST; e.list=lst; return e; }
inline SExpr makeNil(){ return SExpr(); }

std::string toString(const SExpr &expr);

SExpr parse(const std::string &src);

#endif // AST_H
