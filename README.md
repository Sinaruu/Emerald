# Emerald Compiler

> A compiler project built for **CST8152 - Compilers** at Algonquin College
>
> **Authors:** Egor Kivilev, Hoang Thien Loc Nguyen  
> **Professor:** Paulo Sousa

---

```
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @@    *                    *   @@
    @@         @@@@@@@@@@          @@
    @@       @@@@      @@@@        @@
    @@      @@@          @@@       @@
    @@      @@            @@       @@
    @@      @@@@@@@@@@@@@@         @@
    @@      @@@@@@@@@@@@           @@
    @@      @@                     @@
    @@      @@@          @@@       @@
    @@       @@@        @@@        @@
    @@        @@@@@@@@@@@@         @@
    @@          @@@@@@@@           @@
    @@       ~~~~~~~~~~~~~~~       @@
    @@        E M E R A L D        @@
    @@    *                    *   @@
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
```

---

## Overview

Emerald is a custom programming language with a full compiler pipeline implemented in C. The pipeline covers five stages: Coder (encryption), Reader (buffered I/O), Scanner (lexical analysis), Parser (syntax analysis), and Writer (interpretation/execution).

---

## The Emerald Language

Emerald programs are structured with a `data` block for variable declarations and a `code` block for statements. Identifiers for functions and methods use an `&` suffix. Comments begin with `#`.

### Types

| Type     | Keyword  | Example            |
|----------|----------|--------------------|
| Integer  | `int`    | `int x&;`          |
| Float    | `real`   | `real y&;`         |
| String   | `string` | `string name&;`    |

### Keywords

`data`, `code`, `int`, `real`, `string`, `if`, `then`, `else`, `while`, `do`, `return`

### Built-in Functions

| Function   | Description                  |
|------------|------------------------------|
| `print&()` | Outputs values to the screen |
| `input&()` | Reads input from the user    |

### Example Program

```emerald
a = 1
b = a*2
c = (a+b)*3
write(c)
d = "Paulo"
write(d)
write(d, "has", c, "years")
e = true
f = 'Z'
write(e)
write(f)
```

**Output:**
```
9
Paulo
Paulo has 9 years
true
Z
```

---

## Project Structure

```
.
├── Compilers.c          # Entry point — routes to each pipeline stage
├── Compilers.h          # Shared type definitions and language constants
├── Step1Coder.c/.h      # Stage 1: Vigenère cipher (encrypt/decrypt source files)
├── Step2Reader.c/.h     # Stage 2: Buffered file reader
├── Step3Scanner.c/.h    # Stage 3: Lexical scanner (tokenizer)
├── Step4Parser.c/.h     # Stage 4: Recursive-descent parser
├── Step5Writer.c/.h     # Stage 5: Interpreter / code writer
├── Main1Coder.c         # Main driver for Stage 1
├── Main2Reader.c        # Main driver for Stage 2
├── Main3Reader.c        # Main driver for Stage 3
├── Main4Parser.c        # Main driver for Stage 4
├── Main5Writer.c        # Main driver for Stage 5
├── test_code.txt        # Example Emerald source program
└── encrypted_code.txt   # Example encrypted source file
```

---

## Pipeline Stages

### Stage 1 – Coder
Encrypts and decrypts Emerald source files using the **Vigenère cipher** with the key `Emerald`. This allows source code to be stored and distributed in an obfuscated form.

### Stage 2 – Reader
A dynamic character buffer that loads source files into memory for processing. Supports resizing, position tracking, checksums, and usage statistics.

### Stage 3 – Scanner
A finite-automaton-based lexical analyzer that tokenizes source code into the following token types:

| Token    | Description                        |
|----------|------------------------------------|
| `MNID_T` | Method/function name (ends in `&`) |
| `INL_T`  | Integer literal                    |
| `STR_T`  | String literal (single-quoted)     |
| `KW_T`   | Keyword                            |
| `CMT_T`  | Comment                            |
| `EOS_T`  | End of statement (`;`)             |
| `ERR_T`  | Error token                        |
| `SEOF_T` | End of file                        |

### Stage 4 – Parser
A recursive-descent parser that validates Emerald source code against the language grammar (BNF). Reports syntax errors and tracks which grammar rules were matched.

**Grammar summary:**
```
<program>     → int main&() { <dataSession> <codeSession> }
<dataSession> → data { <varDeclarations> }
<codeSession> → code { <statements> }
<statement>   → <outputStatement> | <inputStatement> | <returnStatement>
```

### Stage 5 – Writer
An interpreter that executes Emerald programs by evaluating arithmetic expressions, managing variables (int, real, string, boolean, char), and handling `print&` output statements.

---

## Building

### Requirements
- C compiler: GCC or MSVC (C99 or later)
- Standard C library

### Compile (GCC example)
```bash
gcc -o emerald \
    Compilers.c \
    Step1Coder.c \
    Step2Reader.c \
    Step3Scanner.c \
    Step4Parser.c \
    Step5Writer.c \
    Main1Coder.c \
    Main2Reader.c \
    Main3Scanner.c \
    Main4Parser.c \
    Main5Writer.c
```

---

## Usage

Run the compiler with an option number followed by the source file:

```
./emerald <option> <source_file>
```

| Option | Stage   | Command Example                                    |
|--------|---------|----------------------------------------------------|
| `1`    | Coder   | `./emerald 1 1 input.txt output.txt`               |
| `2`    | Reader  | `./emerald 2 source.txt`                           |
| `3`    | Scanner | `./emerald 3 source.txt`                           |
| `4`    | Parser  | `./emerald 4 source.txt`                           |
| `5`    | Writer  | `./emerald 5 source.txt`                           |

### Run the example program
```bash
./emerald 5 test_code.txt
```

### Encrypt a source file
```bash
./emerald 1 1 test_code.txt encrypted_code.txt
```

### Decrypt a source file
```bash
./emerald 1 0 encrypted_code.txt decrypted_code.txt
```

---

## Language Grammar (BNF Summary)

```
<program>                 → int main&() { <dataSession> <codeSession> }
<dataSession>             → data { <optVarListDeclarations> }
<codeSession>             → code { <optionalStatements> }
<varDeclaration>          → (int | real | string) MNID_T ;
<statement>               → <outputStatement> | <inputStatement> | <returnStatement>
<outputStatement>         → print&( <outputVariableList> ) ;
<inputStatement>          → input&( <inputVariableList> ) ;
<returnStatement>         → return <optReturnExpr> ;
<arithmeticExpression>    → <term> ((+ | -) <term>)*
<term>                    → <factor> ((* | /) <factor>)*
<factor>                  → INL_T | MNID_T | ( <arithmeticExpression> )
```
