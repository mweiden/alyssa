#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <string>
#include <unordered_map>
#include <vector>
using Symbol = std::size_t;

class SymbolTable {
public:
    Symbol intern(const std::string &name);
    const std::string &str(Symbol sym) const;
private:
    std::unordered_map<std::string, Symbol> toSym;
    std::vector<std::string> toStr;
};

SymbolTable &globalSymbolTable();

inline Symbol stringToSymbol(const std::string &s){
    return globalSymbolTable().intern(s);
}

inline const std::string &symbolToString(Symbol sym){
    return globalSymbolTable().str(sym);
}

#endif
