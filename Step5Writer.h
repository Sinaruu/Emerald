/*
************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Fall, 2025
* Author: Egor Kivilev, Hoang Thien Loc Ngyuen
* Professors: Paulo Sousa
************************************************************
#
# ECHO "=---------------------------------------="
# ECHO "|  COMPILERS - ALGONQUIN COLLEGE (F25)  |"
# ECHO "=---------------------------------------="
# ECHO "    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    "
# ECHO "    @@    *                    *   @@    "
# ECHO "    @@         @@@@@@@@@@          @@    "
# ECHO "    @@       @@@@      @@@@        @@    "
# ECHO "    @@      @@@          @@@       @@    "
# ECHO "    @@      @@            @@       @@    "
# ECHO "    @@      @@@@@@@@@@@@@@         @@    "
# ECHO "    @@      @@@@@@@@@@@@           @@    "
# ECHO "    @@      @@                     @@    "
# ECHO "    @@      @@@          @@@       @@    "
# ECHO "    @@       @@@        @@@        @@    "
# ECHO "    @@        @@@@@@@@@@@@         @@    "
# ECHO "    @@          @@@@@@@@           @@    "
# ECHO "    @@       ~~~~~~~~~~~~~~~       @@    "
# ECHO "    @@        E M E R A L D        @@    "
# ECHO "    @@    *                    *   @@    "
# ECHO "    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    "
# ECHO "                                         "
# ECHO "[WRITER SCRIPT .........................]"
# ECHO "                                         "
*/

/*
************************************************************
* File name: Writer.h
* Compiler: MS Visual Studio 2022
* Course: CST 8152 – Compilers, Lab Section: [011, 012]
* Assignment: A12.
* Date: Jan 01 2025
* Professor: Paulo Sousa
* Purpose: This file is the main header for Reader (.h)
************************************************************
*/

/*
 *.............................................................................
 * MAIN ADVICE:
 * Please check the "TODO" labels to develop your activity.
 *.............................................................................
 */

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef WRITER_H_
#define WRITER_H_

#define MAX_VARS 100
#define MAX_EXPR_LEN 256
#define MAX_LINES 100

#define WRITE "print&"

#define EOS '\0'
#define ZERO 0.0
#define STREMPTY ""
#define LPAR '('
#define RPAR ')'
#define PLUS '+'
#define MINUS '-'
#define MULT '*'
#define DIV '/'

#define EQUALS '='
#define MOD '%'
#define SPACE ' '
#define TAB '\t'
#define NEWLINE '\n'
#define RETURN '\r'
#define QUOTES '"'
#define QUOTE '\''

#define TRUE "true"
#define FALSE "false"

typedef enum { NUMERIC, STRING, BOOLEAN, CHAR } VarType;

typedef struct {
    emerald_char name[32];
    VarType type;
    union {
        emerald_doub num_value;
        emerald_char str_value[256];
        emerald_intg bool_value; // For BOOLEAN type (1 for true, 0 for false)
        emerald_char char_value; // For CHAR type
    } value;
} Variable;

emerald_intg find_variable(const emerald_strg name);

emerald_void assign_boolean_variable(const emerald_strg name, emerald_intg value);
emerald_void assign_char_variable(const emerald_strg name, emerald_char value);
emerald_void assign_numeric_variable(const emerald_strg name, emerald_doub value);
emerald_void assign_string_variable(const emerald_strg name, const emerald_strg value);

emerald_intg get_boolean_value(const emerald_strg name);
emerald_char get_char_value(const emerald_strg name);
emerald_doub get_numeric_value(const emerald_strg name);
const emerald_strg get_string_value(const emerald_strg name);

emerald_doub parse_term(emerald_strg* expr);
emerald_doub parse_expression(emerald_strg* expr);
emerald_void calculate(emerald_strg expression);

emerald_void handle_write(emerald_strg expression);
emerald_void process_file(const emerald_strg filename);
emerald_void process_content(emerald_strg content);

#endif
