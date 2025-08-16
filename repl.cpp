#include <iostream>
#include <stdexcept>
#include <readline/history.h>
#include <readline/readline.h>
#include "interpreter.h"
#include "environment.h"
#include "ast.h"

using namespace std;

int main() {
    Environment env("global");
    Interpreter intr(&env);

    cout << "Alyssa P. Hacker's LISP REPL" << endl;

    char *raw_line;
    while ((raw_line = readline(">> ")) != nullptr) {
        std::string line(raw_line);
        free(raw_line);
        if (line.empty()) { continue; }
        add_history(line.c_str());
        try {
            SExpr result = intr.eval(line);
            cout << "\033[0;32m" << toString(result) << "\033[0m" << endl;
        } catch (const std::exception &e) {
            cout << "\033[1;31m" << e.what() << "\033[0m" << endl;
        }
    }
    cout << endl << "LOGOUT" << endl;
}
