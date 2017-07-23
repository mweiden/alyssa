#include <iostream>
#include <csignal>
#include "interpreter.h"
#include "environment.h"

using namespace std;


void sig_handler(int signum) {
    cout << "OK bye. " << signum << endl;
    exit(0);
}

int main() {

    signal(SIGINT, sig_handler);

    cout << "Alyssa P. Hacker's LISP REPL" << endl;

    string expression;


    Environment env = Environment("global");
    LispInterpreter intr = LispInterpreter(&env);

    cout << ">> ";

    while(not cin.eof()) {
        string token = Interpreter::nextToken(cin);
        try {
            string result = intr.eval(token);
            cout << result << endl;
        }
        catch (const std::invalid_argument& ia) {
            cout << "\033[1;31m" << ia.what() << "\033[0m" << endl;
        }
        cout << ">> ";
    };
    cout << endl << "LOGOUT" << endl;
}