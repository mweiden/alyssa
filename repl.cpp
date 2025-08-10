#include <iostream>
#include <stdexcept>
#include "interpreter.h"
#include "environment.h"

using namespace std;


int main() {

    Environment env = Environment("global");
    LispInterpreter intr = LispInterpreter(&env);

    cout << "Alyssa P. Hacker's LISP REPL" << endl;

    cout << ">> ";

    while(not cin.eof()) {
        string token = Interpreter::nextToken(cin);
        try {
            string result = intr.eval(token);
            cout << "\033[0;32m" << result << "\033[0m" << endl;
        }
        catch (const std::invalid_argument& ia) {
            cout << "\033[1;31m" << ia.what() << "\033[0m" << endl;
        }
        cout << ">> ";
    };
    cout << endl << "LOGOUT" << endl;
}
