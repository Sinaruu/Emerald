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
# ECHO "[CODER SCRIPT ..........................]"
# ECHO "                                         "
*/


/*
************************************************************
* File name: Parser.h
* Compiler: MS Visual Studio 2022
* Course: CST 8152 – Compilers, Lab Section: [011, 012]
* Assignment: A32.
* Date: May 01 2023
* Professor: Paulo Sousa
* Purpose: This file is the main header for Parser (.h)
************************************************************
*/

#ifndef PARSER_H_
#define PARSER_H_

/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif
#ifndef READER_H_
#include "Reader.h"
#endif
#ifndef SCANNER_H_
#include "Scanner.h"
#endif

/* Global vars */
static Token			lookahead;
extern BufferPointer	stringLiteralTable;
extern emerald_intg		line;
extern Token			tokenizer(emerald_void);
extern emerald_strg		keywordTable[KWT_SIZE];
static emerald_intg		syntaxErrorNumber = 0;

#define LANG_WRTE		"print&"
#define LANG_READ		"input&"
#define LANG_MAIN		"main&"

/* TO_DO: Create ALL constants for keywords (sequence given in table.h) */

/* Constants */
enum KEYWORDS {
	NO_ATTR = -1,
	KW_data,
	KW_code,
	KW_int,
	KW_real,
	KW_string,
	KW_if,
	KW_then,
	KW_else,
	KW_while,
	KW_do,
	KW_return
};

enum ART_OPS {
	OP_ADD = 0,	// "+"
	OP_SUB,		// "-"
	OP_MUL,		// "*"
	OP_DIV		// "/"
};

enum REL_OPS {
	OP_EQ = 0,	// "=="
	OP_NEQ,		// "!="
	OP_GT,		// ">"
	OP_LT,		// "<"
	OP_GTE,		// ">="
	OP_LTE,		// "<="
};

/* TO_DO: Define the number of BNF rules */
#define NUM_BNF_RULES 14

/* Parser */
typedef struct parserData {
	emerald_intg parsHistogram[NUM_BNF_RULES];	/* Number of BNF Statements */
} ParserData, * pParsData;

/* Number of errors */
emerald_intg numParserErrors;

/* Scanner data */
ParserData psData;

/* Function definitions */
emerald_void startParser();
emerald_void matchToken(emerald_intg, emerald_intg);
emerald_void syncErrorHandler(emerald_intg);
emerald_void printError();
emerald_void printBNFData(ParserData psData);

/* List of BNF statements */
enum BNF_RULES {
	BNF_error,										/*  0: Error token */
	BNF_codeSession,								/*  1 */
	BNF_comment,									/*  2 */
	BNF_dataSession,								/*  3 */
	BNF_optVarListDeclarations,						/*  4 */
	BNF_optionalStatements,							/*  5 */
	BNF_outputStatement,							/*  6 */
	BNF_outputVariableList,							/*  7 */
	BNF_program,									/*  8 */
	BNF_statement,									/*  9 */
	BNF_statements,									/* 10 */
	BNF_statementsPrime,							/* 11 */
	BNF_optParams,									/* 12 */
	BNF_returnStatement								/* 13 */	
};


/* TO_DO: Define the list of keywords */
static emerald_strg BNFStrTable[NUM_BNF_RULES] = {
	"BNF_error",
	"BNF_codeSession",
	"BNF_comment",
	"BNF_dataSession",
	"BNF_optVarListDeclarations",
	"BNF_optionalStatements",
	"BNF_outputStatement",
	"BNF_outputVariableList",
	"BNF_program",
	"BNF_statement",
	"BNF_statements",
	"BNF_statementsPrime",
	"BNF_optParams",
	"BNF_returnStatement"
};

/* TO_DO: Place ALL non-terminal function declarations */
emerald_void codeSession();
emerald_void comment();
emerald_void dataSession();
emerald_void optVarListDeclarations();
emerald_void optionalStatements();
emerald_void outputStatement();
emerald_void outputVariableList();
emerald_void program();
emerald_void statement();
emerald_void statements();
emerald_void statementsPrime();
emerald_void optParams();
emerald_void paramList();

#endif
