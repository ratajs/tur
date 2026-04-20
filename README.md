# tur

tur compiles a description of a Turing machine given in the source file (conventionaly named with a ‘tur’ suffix) into an actual Turing machine, i.e. the tuples describing the machine’s transfer function and stores it in the machine output file (conventionaly named with a ‘tm’ suffix).

The source language is described below. It is expressive enough to describe compositions of complex machines from simpler machines.

## Options

The `-c` and `-r` options determine what to do. If neither on them is present, `-c` is assumed.

#### `-c`, `--compile`
Compile the machine from source. The source is read from the source file or from the standard input if no source file is specified. The resulting machine is saved in the machine output file or printed to the standard output if no output file is specified.

#### `-r`, `--run`
Run the resulting machine. Exactly one argument is required: the machine file from which the machine transfer function is read or (when used with the `-c` option) the source file from which the machine is compiled first. The initial content of the tape is read from the standard input; the eventual content of the tape is printed to the standard output, if and when the machine halts.

### Options for compilation
The following options can be used with the `-c` option:

#### `-i`, `--instructions`
Print the instructions (intermediate representation of the program) to the standard error output.

#### `-I`, `--only-instructions`
Do not create the machine, only display the instructions. These are printed into the output file or to the standard output if no output file is specified.

#### `-O`, `--optimize`
Optimize the machine.

#### `-b path`, `--include-base path`
Use this path as the base path for include statements in the source. The default path is the directory where the source file is located.

#### `-x suffix`, `--include-suffix suffix`
Use this string as the suffix of included files. The default is `.tm`. The suffix does not have to be just an extension, the directory separator can be used to mark that the machine file is inside a directory with the name of the include.

#### `-m machine1.tm`, `--machine machine1.tm`
Use this machine for require statements in the program. Machines provided with this option map in order with require statements.

### Options for machine run
The following options can be used with the `-r` option:

#### `-s`, `--steps`
Trace the machine steps: print the tape content and the current state to the standard error output.

#### `-k`, `--keep-blanks`
Keep leading and trailing blank symbols.

#### `-d`, `--decode`
Decode sequences of ones in the output as decimal numbers.

#### `-e`, `--encode`
Encode numbers from the input as sequences of ones (unary notation).

#### `-n`, `--numbers`
A shortcut for `-d -e`.

#### `-C`, `--no-color`
Do not highlight the current position in the output or while tracing with formating sequences. Normally, when tracing or when the output is printed without encoding, the current position is highlighted, which may pose a problem when using this output further.

## The Source Language

The source language is a high-order language, described below in detail. As an example, the following source get compiled into a machine that computes the greatest common divisor of two given numbers:
```
[a, b] = input;
while b > 0:
  B = a mod b;
  a = b;
  b = B;
endwhile;
output = a;
```

Every variable represents one tape of the machine on which numbers can be written. The special names `input` and `output` are reserved for the input tape and the output tape.

### Expressions
The following types of expressions are supported:

#### A numeric constant (any positive integer or zero):
```
0
18
```

#### The number of items on the tape of a variable:
```
count variable
ρ variable
```

#### The value of a variable or a specific slice thereof:
```
input # The first number of a variable
input[0] # Also the first number
input[0:1] # Also the first number
input[4] # The fifth number
input[5:7] # The fifth and sixth number
input[1:] # All numbers except the first one
input[0:2] # First two numbers
input[:2] # Also first two numbers
input[0:] # All numbers
input[:] # Also all numbers
input... # Also all numbers (either three dots or the Unicode ellipsis, both is supported)
```

#### The result of a machine on a given input (any non-logical expressions); the result can be sliced:
```
f() # The result of the run of f on an empty tape (first number)
f()... # The result of the run of f on an empty tape (the whole tape)
g(input[0], input[1])[1] # The second number of the run of the g machine with the first two numbers of the input
g(input[:2])[1] # The same thing
g(12, input)... # The whole result of running g on a tape with the number 12 and the first number of the input
h(g(f(input)...)...)... # Composition of three functions
```

#### Arithmetical and logical operations:
```
a + b # Addition
a - b # Subtraction, both hyphen-minus and Unicode minus is supported
a ⋅ b # Multiplication (Unicode dot operator)
a × b # Also multiplication
a * b # Also multiplication
a / b # Integer division (undefined behavior for division by zero), both normal slash and Unicode fraction slash is supported
a ∶ b # Also division (Unicode ratio)
a mod b # Modulo (remainder; undefined behavior for division by zero)
a % b # Also modulo
a min b # Minumum of two values
a & b # Also minumum of two values
a max b # Maximum of two values
a | b # Also maximum of two values
a==b # Equality
a!=b # Inequality
a ≠ b # Also inequality
a < b # Less than
a > b # Greater than
a ≤ b # Less than or equal
a <= b # Also less than or equal
a ≥ b # Greater than or equal
a >= b # Also greater than or equal
!(a==b) # Negation
¬ a==b # Also negation
a==b ∧ c==d # Conjunction (and)
a==b && c==d # Also conjunction
a==b and c==d # Also conjunction
a==b ∨ c==d # Disjunction (or)
a==b || c==d # Also disjunction
a==b or c==d # Also disjunction
a==b xor c==d # Exclusive disjunction (xor)
```

### Statements
The following types of statements are supported:

#### A definition of a Turing machine:
```
dec1 = { A 1 0 R _ }; # old state, old symbol, new symbol, direction, new state
dec2 = { stateA 1 0 R stateB }; # Any state name is valid, if it does not contain a digit, a whitespace, } or #.
dec3 = { stateA10RstateB }; # The whitespace is optional (any number of spaces, tabs or newlines can be used).
inc1 = { A11LA A01N_ }; # More transitions (the old state of the first transitions is the initial state). This whitespace is not optional.
inc2 = { A11LA A01SB }; # _ had no special meaning, when the machine halts, the answer is read. S is an alias of N.
inc3 = {
  A 1 1 L A
  A 0 1 N _ # place one extra 1
}; # Using more lines and adding comments might improve legibility.
```

#### An include of a machine defined in a separate file (the syntax of the machine file is the same as in the {} block):
```
include gcd; # gcd.tm is expected in the same directory as the source file, unless specified otherwise with CLI options.
```

#### An include of a requried machine passed by a command line option at compile time:
```
require f; # Compilation requires an option like this: -m machine1.tm
require g; # More requirements require more -m options.
```

#### Call of a Turing machine on a variable (a tape). Unlike call expressions, changes will affect the variable directly:
```
f(input...); # Using ... is required.
```

#### Assignment of an expression to a variable, possibly at a given index:
```
output = 1; # The variable output will contain a single number 1.
output[2:] = 2; # The first two numbers will be preserved, 2 will rewrite the rest (undefined behavior for less than two numbers in output).
output[] = 3; # Append the number 3.
#output[4] = 4; # This is forbidden (for now), any destination slice must be rightwise unbounded. A workaround is: output[4:] = [4, output[5:]];
output = input; # Assign the first number of input.
output = input...; # Assign the whole content of input.
output[10:] = f(input[5:8])[2:4]; # Any non-logical expression of any length can be assigned.
output = output[1:5];
output[2:] = output[1:5];
```

#### Incrementing or decrementating the first number on a tape:
```
a++;
b--; # Either two hyphen-minuses or two Unicode minuses.
```

#### Branching (`if`, `else`):
```
if input==42:
  output = 1;
else: # The else part is optional, endif can directly follow the first branch.
  output = 0;
endif;
```

#### Loops (`while`, `loop`):
```
loop: # Loop without condition (until break):
  while a > 0: # Loop with condition.
    if b==10:
      continue; # Skip to the next iteration of the innermost loop (the inner loop here).
    endif;
    a--;
  endwhile;
  b++;
  if a==10:
    break; # Exit the innermost loop (the outer loop here).
  endif;
endloop;
```

#### `exit` statement:
```
if output==20:
  exit; # Skip behind the last statement of the program.
endif;
output = 40; # This will be skipped if the first number of the output is 20.
```

#### Joining (imploding) multiple expressions to a varible:
```
output = [1, 2, 3]; # output will contain those three numbers.
output[2:] = [1, 2, 3]; # The variable can be indexed.
output[] = [1, 2, 3]; # Appending is also possible.
output = [f(input)..., 10, input[2:5], output]; # Any non-logical expressions can be used.
```

#### Splitting (exploding) an expression to multiple variables:
```
[a, b, c] = input; # First number goes to a, second number goes to b, third number goes to c (undefined behavior for less than 3 numbers in input).
[a, b...] = input; # First number goes to a, the reset goes to b (undefined behavior for empty input). Only the last variable can have the ellipsis.
[a, b] = input...; # The ellipsis can be there, but it is optional.
[a, b] = a[1:]; # Other indexation than ellipsis is possible, but it has to be rightwise unbounded.
[a, b] = f(a, b)...; # Any expression which can yield more numbers is possible as the right side. The ellipsis is optional here as well.
```

<img src="https://fit.cvut.cz/static/images/fit-cvut-logo-en.svg" alt="FIT CTU logo" height="200">

This software was developed with the support of the **Faculty of Information Technology, Czech Technical University in Prague**.
For more information, visit [fit.cvut.cz](https://fit.cvut.cz).
