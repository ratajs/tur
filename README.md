# tur

tur compiles a description of a Turing machine given in the source file (conventionaly named with a ‘tur’ suffix) into an actual Turing machine,
i.e. the tuples describing the machine’s transfer function and stores it in the machine output file (conventionaly named with a ‘tm’ suffix).

The source language is described below. It is expressive enough to describe compositions of complex machines from simpler machines.
The input and output is encoded on the tape with unary notation (sometimes called Tally Notation). Number 0 is encoded as 1, number 1 as 11, number 2 as 111 etc. Multiple numbers are separated with a single symbol 0. The blank symbol is also 0.
An empty tape is valid, it can be interpreted as an empty list. The machine should start at the beginning of the tape content (the first symbol 1 if the tape is not empty) and when it halts, the position should also be at the beginning;
otherwise the output is considered invalid. Included machines must also use this convention.

## Building and Installation

The project can be built by calling `make` and installed by calling `make install`. Plain `make install` installs it to `~/.local/bin/`, make sure that this directory is in your `$PATH`. You can install it elsewhere; for a normal, system-wide install to `/usr/bin/`, run `make PREFIX=/usr/ install`.
To uninstall, run `make uninstall` (with the same `PREFIX` you used for installing).

## Usage

The following ways to run the program are possible:

- `tur [-c] [-iIO] [-l tur | ir] [-b path] [-x suffix] [-m machine1.tm -m machine2.tm...] [source.tur] [machine.tm]` – compile
- `tur -r [-denskC] machine.tm` – run
- `tur -cr [-iOenskC] [-l tur | ir] [-b path] [-x suffix] [-m machine1.tm -m machine2.tm...] source.tur` – compile and run

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

#### `-l tur | ir`, `--input-language tur | ir`
`-l tur` (default) means that the input is in the primary source language of tur, `-l ir` means that the input is in the form of IR (intermediate representation).

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

Only single-line comments are supported. They start with `#` anywhere on line.

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
output[:0] = 1; # Prepend the number 1.
output[:2] = output[0]; # Replace the first two numbers with only the first one.
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
# The other branches are optional, endif can go here.
elseif input > 0:
  output = 2;
# More elseif branches can follow.
else:
  output = 0;
endif;
```

#### Loops (`while`, `loop`, `for`):
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

for x = 0; x < a; x++: # Initial statement, condition, step statement.
  if x % 5==0:
    continue; # Jump to the step statement.
  endif;
  output[] = x;
endfor;

for ; a > b; a--; b++: endfor; # The initial statement and the body is optional, there can be multiple step statements (but at least one).
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
output[:1] = []; # Delete the first number.
output[:0] = [1, 2, input]; # Prepend 1, 2, and the first number from input (the tape will then start with 1).
```

#### Splitting (exploding) an expression to multiple variables:
```
[a, b, c] = input; # First number goes to a, second number goes to b, third number goes to c (undefined behavior for less than 3 numbers in input).
[a, b...] = input; # First number goes to a, the reset goes to b (undefined behavior for empty input). Only the last variable can have the ellipsis.
[a, b] = input...; # The ellipsis can be there, but it is optional.
[a, b] = a[1:]; # Other indexation than ellipsis is possible, but it has to be rightwise unbounded.
[a, b] = f(a, b)...; # Any expression which can yield more numbers is possible as the right side. The ellipsis is optional here as well.
```

## Instructions
In addition the source language described above, intermediate representation (IR) can also be used. The flags -i and -I are used to print the IR as output, while -l ir can be used to accept IR as input. The first line of IR states how many tapes are used by the program. It
looks like this:
```
TAPES 3
```
If this line is missing, one tape is assumed. The next lines are instructions. Each line contains the name of the instructions followed by its arguments in parentheses. Leading and trailing whitespace and comments (#) are ignored. Here is an example of a simple program:
```
TAPES 1
decompress(1) # Use tape 1 as input
compare(1[0] = 42, 2, 3) # Compare the first number on tape 1 with 42
jump(COMEFROM 2) # Come from the label 2 (if true)
writeNumber(1[0:], 1) # Write 1 on the beginning of tape 1
jump(GOTO 1) # Go to the label 1
jump(COMEFROM 3) # Jump from the label 3 (if false)
writeNumber(1[0:], 0) # Write 0 on the beginning of tape 1
jump(GOTO 1) # Go to the label 1
jump(COMEFROM 1) # Come from the label 1
compress(1) # Use tape 1 as output
```
The following instructions are supported:

#### `decompress`
This should be the first instruction of every program. It specifies where the input of the compiled Turing machine should be placed. The only argument of this instruction is the input tape.

#### `compress`
This should be the last instruction of every program. It specifies which tape is considered to be the output. The only argument of this instruction is the output tape.

#### `writeNumber`
This instruction writes a number on a specific position on a tape. The rest of the tape is deleted. When this instruction is executed, the tape must be long enough, so that the position is not further than the end of the tape. It has two arguments separated by
comma: destination and number. The destination has three variants:
```
writeNumber(2[5:], 10) # Write 10 directly after the fifth number on the tape 2
writeNumber(2[], 15) # Append 15 to the tape 2
writeNumber(2[−2:], 20) # Replace the last two numbers on the tape 2 with 20 (Unicode minus)
```

#### `clear`
This instruction clears a range on a tape. The range must start with the index 0 or must be rightwise unbounded. The tape with range is the only argument.
```
clear(1[0:]) # Clear the whole tape 1
clear(1[0:5]) # Delete the first five numbers while keeping the rest
clear(1[5:]) # Delete everything after the fifth number
clear(1[−1:]) # Delete the last number (Unicode minus)
#clear(1[0:−1]) # This is not supported, you have to use REVERSE
```

#### `copy`
This instruction copies a range from one tape to another. The arguments are: the source tape, the source range, comma, the destination tape, the destination range. The source range is specified by the first index, followed by colon and the optional second index, all
in square brackets. The destination range must be rightwise unbounded, it is specified by an index followed by colon, all in square brackets, or by empty sqaure brackets, which means that the data should be appended. All indices can be negative (indexed from the
end), but both source indices must have the same sign.
```
copy(3[0:1], 4[0:]) # Copy the first number of the tape 3 to the beginning of the tape 4, the content of the tape 4 will be lost
copy(3[1:], 4[]) # Copy the content of the tape 3 except for the first number to the end of the tape 4, the content of the tape 4 will not be lost
copy(1[−2:−1], 2[0:]) # Copy the penultimate number from the tape 1 to the tape 2
copy(1[−1:], 2[−1]) # Replace the last number of the tape 2 with the last number of tape 1
```

#### `call`
This instruction simulates a defined Turing machine on a single tape. It has two arguments separated by comma: the tape and the machine.
```
call(1, { A11LA A01N_ }) # Increment the first number on tape 1
```

#### `jump`
This instruction jumps to or from a label, which is a number. There are two types of jumps: `GOTO` and `COMEFROM`. Each label should be associated with one `COMEFROM` jump and at least one `GOTO` jump, compare or compareTapeLength. As arguments, the type (`GOTO` or `COMEFROM`)
followed by the label (without comma) is provided. A `COMEFROM` jump must follow if and only if the previous instruction jumps somewhere (`GOTO` jump, compare, compareTapeLength).

#### `compare`
This instruction compares two arguments and jumps to one of two specified labels, depending on the result of the comparison. The comparison arguments can either be constants or tapes with indices. These operators can be used: =, ≠, <, >, ≤, ≥
```
compare(1[3] > 2[0], 1, 2) # If the fourth number on tape 1 is greater than the first number on tape 2, jump to 1, else jump to 2
compare(1[3] < 20, 3, 4) # If the fourth number on tape 1 is less than 20, jump to 3, else jump to 4
compare(1 = 1, 5, 6) # This always jumps to 5
```

#### `compareTapeLength`
This instruction compares the length of a tape with a number. These operators can be used: =, ≠, <, >, ≤, ≥
```
compareTapeLength(|1| < 2, 1, 2) # If there are fewer than 2 numbers on the tape 1, jump to 1, else jump to 2
compareTapeLength(|2| = 0, 3, 4) # If the tape 2 is empty, jump to 3, else jump to 4
```

#### `REVERSE`
This is a pseudoistruction which causes the machine to work like in mirror. The tape numbers will stay, but their starts will now be ends and vice versa. This state will remain until the next `REVERSE`. It is possible to combine jumps with `REVERSE`, but it is neces‐
sary that the reversed labels match (jumps from a reversed part of the program must lead also to a reversed part).
````
REVERSE # Start the reversed state
clear(1[1:]) # This clears everything but the last number
REVERSE # End the reversed state
````

## Examples and Tests

Several programs for `tur` are in the directory `examples`. Most of them have their test inputs and reference outputs in `.in` and `.out` files. Running `make test` will compile
the samples and run all the tests. Compiled machines ending with `.O.tm` are compiled with the `-O` flag.

<img src="https://fit.cvut.cz/static/images/fit-cvut-logo-en.svg" alt="FIT CTU logo" height="200">

This software was developed with the support of the **Faculty of Information Technology, Czech Technical University in Prague**.
For more information, visit [fit.cvut.cz](https://fit.cvut.cz).
