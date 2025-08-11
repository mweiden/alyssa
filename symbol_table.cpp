#include "symbol_table.h"

Symbol SymbolTable::intern(const std::string &name){
    auto it = toSym.find(name);
    if(it != toSym.end()) return it->second;
    Symbol id = toStr.size();
    toSym[name]=id;
    toStr.push_back(name);
    return id;
}

const std::string &SymbolTable::str(Symbol sym) const{
    return toStr.at(sym);
}

static SymbolTable table_instance;
SymbolTable &globalSymbolTable(){
    return table_instance;
}
