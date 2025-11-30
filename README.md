# PFL

Stands for **Pythonic Functional Language**. A personal project which I started in November 2025. This language combines the simplicity of Python and 

## Features

- Functional

    Immutable states make you program less prone to errors.

- Interpreted

    Write and run your code without compiling.

- Strong Typing

    Never worry about whether `[] == 0` returns true or false.

- Simplistic

    Uses Python-like syntax which is very concise.

## How to Get Started
You can clone this repo and compile the source directly using the following command:
```
g++ src/main.cpp src/lexer.cpp src/interpreter.cpp src/parser.cpp -o bin/pfl.exe  
```
Then you can run the REPL interpreter using the following commands
```
cd bin
```
```
pfl
```
Now you can play around with pfl. Enter any PFL code that you want. Try something like
```py
foo = "hello"\
bar = "world"\
foo + " " + bar
```
You should see this output
```
hello world
```
You can also write the code inside a `.pfl` file. Then make the interpreter execute the file using the following command
```
pfl <your-file>.pfl
```
## Examples
### Hello World
A PFL program may start from a `main` function if it's present in your code. If not, your code will be executed in a top-to-bottom manner, like Python.
Both of the following codes 
```
"hello world"
```
and
```
fn main():
    "hello world"
```
results the same output. Notice how we don't use a `print` function or anything. In PFL, **everything** is an expression.

### Variables
In PFL, all variables are immutable, meaning their values cannot be reassigned. This follows the principle of functional programming.
```
x = 10
y = 5
x + y
```
