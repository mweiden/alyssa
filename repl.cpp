// REPL implementation optionally using GNU Readline

#include <iostream>
#include <stdexcept>
#ifdef HAVE_READLINE
#include <readline/readline.h>
#if __has_include(<readline/history.h>)
#include <readline/history.h>
#else
extern "C" void add_history(const char*);
#endif
#endif
#include "interpreter.h"
#include "environment.h"
#include "ast.h"

using namespace std;

int main() {
    Environment env("global");
    Interpreter intr(&env);

    cout << "Alyssa P. Hacker's LISP REPL" << endl;

#ifdef HAVE_READLINE
    char *raw_line;
    while ((raw_line = readline(">> ")) != nullptr) {
        std::string line(raw_line);
        free(raw_line);
        if (line.empty()) { continue; }
        add_history(line.c_str());
#else
    std::string line;
    while (true) {
        cout << ">> ";
        if (!getline(cin, line)) { break; }
        if (line.empty()) { continue; }
#endif
        try {
            SExpr result = intr.eval(line);
            cout << "\033[0;32m" << toString(result) << "\033[0m" << endl;
        } catch (const std::exception &e) {
            cout << "\033[1;31m" << e.what() << "\033[0m" << endl;
        }
    }
    cout << endl << "LOGOUT" << endl;
}
