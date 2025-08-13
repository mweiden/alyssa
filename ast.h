#ifndef AST_H
#define AST_H

#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <functional>

struct Environment; // forward declaration
struct SExpr; // forward declaration for Lambda

struct Lambda {
    std::vector<std::string> params;
    std::shared_ptr<SExpr> body;
    Environment *env; // enclosing environment
};

struct SExpr {
    using List = std::vector<SExpr>;
    using Primitive = std::function<SExpr(const std::vector<SExpr>&)>;
    using Value = std::variant<std::monostate, double, std::string, List, std::shared_ptr<Lambda>, Primitive>;

    Value value;

    SExpr() = default;
    SExpr(double num) : value(num) {}
    SExpr(const std::string &sym) : value(sym) {}
    SExpr(const char *sym) : value(std::string(sym)) {}
    SExpr(const List &list) : value(list) {}
    SExpr(const Primitive &prim) : value(prim) {}
    SExpr(std::shared_ptr<Lambda> lam) : value(lam) {}

    bool isNumber() const { return std::holds_alternative<double>(value); }
    bool isSymbol() const { return std::holds_alternative<std::string>(value); }
    bool isList() const { return std::holds_alternative<List>(value); }
    bool isLambda() const { return std::holds_alternative<std::shared_ptr<Lambda>>(value); }
    bool isPrimitive() const { return std::holds_alternative<Primitive>(value); }
};

// Parsing utilities
SExpr parse(const std::string &src);

// Debug/printing helper
std::string toString(const SExpr &expr);

#endif // AST_H
