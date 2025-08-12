#include <iostream>
#include <stdexcept>
#include "interpreter.h"
#include "environment.h"

using namespace std;

int main() {
    Environment env("global");
    LispInterpreter intr(&env);

    cout << "Alyssa P. Hacker's LISP REPL" << endl;
    string line;
    cout << ">> ";
    while(std::getline(cin, line)){
        if(line.empty()){ cout << ">> "; continue; }
        try {
            SExpr result = intr.eval(line);
            cout << "\033[0;32m" << toString(result) << "\033[0m" << endl;
        } catch(const std::exception &e){
            cout << "\033[1;31m" << e.what() << "\033[0m" << endl;
        }
        cout << ">> ";
    }
    cout << endl << "LOGOUT" << endl;
}
