# alyssa
A basic LISP interpreter and REPL based on that described in [The Structure and Interpretation of Computer Programs](https://mitpress.mit.edu/sicp/full-text/book/book.html).

# Building

```bash
cmake --build .
```

# Testing

```bash
./tests
```

# REPL

To use the REPL, execute `./repl` after building.

```lisp
$ ./repl
Alyssa P. Hacker's LISP REPL
>> 
```

Here you have access to basic LISP pimitives, arithmetic operations, and boolean operations.

```lisp
>> (set! x 42)
x <- 42
>> x
42
>> (* 2 3 4)
24
>> (and true false)
false
>> (list 1 2 3)
(1 2 3)
>> (car (list 1 2 3))
1
>> (cdr (list 1 2 3))
(2 3)
>> (cons 1 2)
(1 2)
>> (cons 1 (list 2 3))
(1 2 3)
>> (define map (lambda (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst))))))
map <- (procedure (op lst) (if (null? lst) NIL (cons (op (car lst)) (map op (cdr lst)))) global)
>> (map (lambda (x) (eq? x 1)) (list 1 2 1 2))
(true false true false)
```

Note there are some slight differences in syntax from common LISP.
