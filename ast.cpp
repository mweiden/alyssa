#include "ast.h"
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace {
struct Parser {
    const std::string &src;
    size_t pos;
    Parser(const std::string &s) : src(s), pos(0) {}

    void skip(){ while(pos < src.size() && isspace(src[pos])) ++pos; }
    bool eof() const { return pos >= src.size(); }
    char peek(){ return eof()? '\0' : src[pos]; }
    char get(){ return eof()? '\0' : src[pos++]; }

    SExpr parseExpr(){
        skip();
        if(eof()) return makeNil();
        char c = peek();
        if(c == '('){
            get(); // consume '('
            std::vector<SExpr> elems;
            skip();
            while(!eof() && peek() != ')'){
                elems.push_back(parseExpr());
                skip();
            }
            if(peek() != ')') throw std::runtime_error("missing )");
            get();
            return makeList(elems);
        } else if(c == ')') {
            throw std::runtime_error("unexpected )");
        } else {
            return parseAtom();
        }
    }

    SExpr parseAtom(){
        std::string tok;
        while(!eof()){
            char c = peek();
            if(isspace(c) || c=='(' || c==')') break;
            tok += c; pos++;
        }
        if(tok.empty()) return makeNil();
        // number?
        char *end=nullptr;
        double val = std::strtod(tok.c_str(), &end);
        if(end && *end=='\0'){ return makeNumber(val); }
        return makeSymbol(tok);
    }
};
} // namespace

SExpr parse(const std::string &src){
    Parser p(src);
    return p.parseExpr();
}

std::string toString(const SExpr &expr){
    switch(expr.type){
        case SExpr::Type::NUMBER:{
            std::ostringstream ss; ss<<expr.number; return ss.str();
        }
        case SExpr::Type::SYMBOL:
            return symbolToString(expr.sym);
        case SExpr::Type::LIST:{
            std::string s="("; bool first=true; for(auto &e: expr.list){ if(!first) s+=' '; first=false; s+=toString(e); } s+=")"; return s; }
        case SExpr::Type::NIL:
        default:
            return "NIL";
    }
}
