# alyssa
A basic LISP interpreter and REPL based on that described in [The Structure and Interpretation of Computer Programs](https://mitpress.mit.edu/sicp/full-text/book/book.html).

# Building

```bash
cmake --build .
```

# REPL

To use the REPL, execute `./repl` after building.

```lisp
$ ./repl
lyssa P. Hacker: LISP REPL
>> 
```

Here you have access to basic LISP pimitives, arithmetic operations, and boolean operations.

```lisp
>> (define map (lambda (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst))))))
map <- (procedure (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst)))) global)
>> (map (lambda (x) (eq? x 1)) (list 1 2 1 2))
(true false true false)
>> (* 2 3 4)
24
>> (and true false)
false
>> (set! x 42)
x <- 42
>> x
42
```

Note there are some slight differences in syntax from common LISP.
