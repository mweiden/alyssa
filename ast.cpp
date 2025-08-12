#include "ast.h"
#include <cctype>
#include <stdexcept>
#include <sstream>

namespace {
class Parser {
public:
    explicit Parser(const std::string &src) : s(src), pos(0) {}
    SExpr parseExpr() {
        skipWhitespace();
        if (pos >= s.size()) throw std::runtime_error("unexpected end of input");
        char c = s[pos];
        if (c == '(') {
            ++pos; // consume '('
            SExpr::List list;
            skipWhitespace();
            while (pos < s.size() && s[pos] != ')') {
                list.push_back(parseExpr());
                skipWhitespace();
            }
            if (pos >= s.size() || s[pos] != ')') throw std::runtime_error("missing ')'");
            ++pos; // consume ')'
            return SExpr(list);
        } else {
            return parseAtom();
        }
    }
private:
    SExpr parseAtom() {
        size_t start = pos;
        while (pos < s.size() && !std::isspace(static_cast<unsigned char>(s[pos])) && s[pos] != '(' && s[pos] != ')') {
            ++pos;
        }
        std::string token = s.substr(start, pos - start);
        // number?
        char *endptr = nullptr;
        double val = std::strtod(token.c_str(), &endptr);
        if (endptr != token.c_str() && *endptr == '\0') {
            return SExpr(val);
        }
        return SExpr(token); // symbol
    }
    void skipWhitespace() {
        while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
    }
    std::string s;
    size_t pos;
};
} // namespace

SExpr parse(const std::string &src) {
    Parser p(src);
    return p.parseExpr();
}

static std::string toStringList(const SExpr::List &list) {
    std::string out = "(";
    for (size_t i = 0; i < list.size(); ++i) {
        out += toString(list[i]);
        if (i + 1 < list.size()) out += ' ';
    }
    out += ')';
    return out;
}

std::string toString(const SExpr &expr) {
    if (expr.isNumber()) {
        std::ostringstream ss;
        ss << std::get<double>(expr.value);
        return ss.str();
    } else if (expr.isSymbol()) {
        return std::get<std::string>(expr.value);
    } else if (expr.isList()) {
        return toStringList(std::get<SExpr::List>(expr.value));
    } else if (expr.isLambda()) {
        return "<lambda>";
    } else if (expr.isPrimitive()) {
        return "<primitive>";
    }
    return "nil";
}
