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
# ECHO "[PARSER SCRIPT .........................]"
# ECHO "                                         "
*/

/*
************************************************************
* File name: Step4Parser.h
* Compiler: MS Visual Studio 2022
* Course: CST 8152 - Compilers, Lab Section: [011, 012]
* Assignment: A32.
* Date: Jan 01 2025
* Professor: Paulo Sousa
* Purpose: This file is the main header for Parser (.h)
*           for the Emerald language.
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
#include "Step2Reader.h"
#endif
#ifndef SCANNER_H_
#include "Step3Scanner.h"
#endif

/* Global vars */
static Token			lookahead;
extern BufferPointer	stringLiteralTable;
extern emerald_intg		line;
extern Token			tokenizer(emerald_void);
extern emerald_strg		keywordTable[KWT_SIZE];
static emerald_intg		syntaxErrorNumber = 0;

/* Language method identifiers */
#define LANG_WRTE		"print&"
#define LANG_READ		"input&"
#define LANG_MAIN		"main&"

/* Keyword enum - must match keywordTable order in Step3Scanner.h */
enum KEYWORDS {
	NO_ATTR = -1,
	KW_data,		/* 0 */
	KW_code,		/* 1 */
	KW_int,			/* 2 */
	KW_real,		/* 3 */
	KW_string,		/* 4 */
	KW_if,			/* 5 */
	KW_then,		/* 6 */
	KW_else,		/* 7 */
	KW_while,		/* 8 */
	KW_do,			/* 9 */
	KW_return		/* 10 */
};

/*
 ************************************************************
 * Emerald Grammar (BNF):
 *
 * <program>
 *     -> int main&() { <dataSession> <codeSession> }
 *     FIRST = { KW_T(KW_int), CMT_T, MNID_T(main&), SEOF_T }
 *
 * <comment>
 *     -> CMT_T
 *     FIRST = { CMT_T }
 *
 * <optParams>
 *     -> <paramList> | e
 *     FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string), e }
 *
 * <paramList>
 *     -> (int|real|string) MNID_T
 *     FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string) }
 *
 * <dataSession>
 *     -> data { <optVarListDeclarations> }
 *     FIRST = { KW_T(KW_data) }
 *
 * <optVarListDeclarations>
 *     -> <varListDeclarations> | e
 *     FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string), e }
 *
 * <varListDeclarations>
 *     -> <varDeclaration> <varListDeclarationsPrime>
 *     FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string) }
 *
 * <varListDeclarationsPrime>
 *     -> <varDeclaration> <varListDeclarationsPrime> | e
 *     FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string), e }
 *
 * <varDeclaration>
 *     -> (int|real|string) MNID_T ;
 *     FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string) }
 *
 * <codeSession>
 *     -> code { <optionalStatements> }
 *     FIRST = { KW_T(KW_code) }
 *
 * <optionalStatements>
 *     -> <statements> | e
 *     FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return), e }
 *
 * <statements>
 *     -> <statement> <statementsPrime>
 *     FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return) }
 *
 * <statementsPrime>
 *     -> <statement> <statementsPrime> | e
 *     FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return), e }
 *
 * <statement>
 *     -> <outputStatement> | <inputStatement> | <returnStatement>
 *     FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return) }
 *
 * <outputStatement>
 *     -> print&( <outputVariableList> );
 *     FIRST = { MNID_T(print&) }
 *
 * <outputVariableList>
 *     -> STR_T | <arithmeticExpression> | e
 *     FIRST = { STR_T, INL_T, MNID_T, LPR_T, e }
 *
 * <inputStatement>
 *     -> input&( <inputVariableList> );
 *     FIRST = { MNID_T(input&) }
 *
 * <inputVariableList>
 *     -> MNID_T | e
 *     FIRST = { MNID_T, e }
 *
 * <returnStatement>
 *     -> return <optReturnExpr> ;
 *     FIRST = { KW_T(KW_return) }
 *
 * <optReturnExpr>
 *     -> <arithmeticExpression> | e
 *     FIRST = { INL_T, MNID_T, LPR_T, e }
 *
 * <arithmeticExpression>
 *     -> <term> <arithmeticExpressionPrime>
 *     FIRST = { INL_T, MNID_T, LPR_T }
 *
 * <arithmeticExpressionPrime>
 *     -> + <term> <arithmeticExpressionPrime>
 *      | - <term> <arithmeticExpressionPrime>
 *      | e
 *     FIRST = { '+', '-', e }
 *
 * <term>
 *     -> <factor> <termPrime>
 *     FIRST = { INL_T, MNID_T, LPR_T }
 *
 * <termPrime>
 *     -> * <factor> <termPrime>
 *      | / <factor> <termPrime>
 *      | e
 *     FIRST = { '*', '/', e }
 *
 * <factor>
 *     -> INL_T | MNID_T | ( <arithmeticExpression> )
 *     FIRST = { INL_T, MNID_T, LPR_T }
 ************************************************************
 */

 /* Number of BNF rules */
#define NUM_BNF_RULES 22

/* Parser data structure */
typedef struct parserData {
	emerald_intg parsHistogram[NUM_BNF_RULES];
} ParserData, * pParsData;

/* Number of errors */
extern emerald_intg numParserErrors;

/* Parser data (global) */
extern ParserData psData;

/* BNF rule index enum */
enum BNF_RULES {
	BNF_error,						/*  0 */
	BNF_codeSession,				/*  1 */
	BNF_comment,					/*  2 */
	BNF_dataSession,				/*  3 */
	BNF_optVarListDeclarations,		/*  4 */
	BNF_optionalStatements,			/*  5 */
	BNF_outputStatement,			/*  6 */
	BNF_outputVariableList,			/*  7 */
	BNF_program,					/*  8 */
	BNF_statement,					/*  9 */
	BNF_statements,					/* 10 */
	BNF_statementsPrime,			/* 11 */
	BNF_optParams,					/* 12 */
	BNF_returnStatement,			/* 13 */
	BNF_varListDeclarations,		/* 14 */
	BNF_varListDeclarationsPrime,	/* 15 */
	BNF_varDeclaration,				/* 16 */
	BNF_inputStatement,				/* 17 */
	BNF_inputVariableList,			/* 18 */
	BNF_arithmeticExpression,		/* 19 */
	BNF_arithmeticExpressionPrime,	/* 20 */
	BNF_term						/* 21 */
};

/* BNF string table for printing statistics */
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
	"BNF_returnStatement",
	"BNF_varListDeclarations",
	"BNF_varListDeclarationsPrime",
	"BNF_varDeclaration",
	"BNF_inputStatement",
	"BNF_inputVariableList",
	"BNF_arithmeticExpression",
	"BNF_arithmeticExpressionPrime",
	"BNF_term"
};

/* ---- Function declarations ---- */

/* Core parser functions */
emerald_void startParser(emerald_void);
emerald_void matchToken(emerald_intg, emerald_intg);
emerald_void syncErrorHandler(emerald_intg);
emerald_void printError(emerald_void);
emerald_void printBNFData(ParserData psData);

/* Non-terminal grammar functions */
emerald_void program(emerald_void);
emerald_void comment(emerald_void);
emerald_void optParams(emerald_void);
emerald_void paramList(emerald_void);
emerald_void dataSession(emerald_void);
emerald_void optVarListDeclarations(emerald_void);
emerald_void varListDeclarations(emerald_void);
emerald_void varListDeclarationsPrime(emerald_void);
emerald_void varDeclaration(emerald_void);
emerald_void codeSession(emerald_void);
emerald_void optionalStatements(emerald_void);
emerald_void statements(emerald_void);
emerald_void statementsPrime(emerald_void);
emerald_void statement(emerald_void);
emerald_void outputStatement(emerald_void);
emerald_void outputVariableList(emerald_void);
emerald_void inputStatement(emerald_void);
emerald_void inputVariableList(emerald_void);
emerald_void returnStatement(emerald_void);
emerald_void optReturnExpr(emerald_void);
emerald_void arithmeticExpression(emerald_void);
emerald_void arithmeticExpressionPrime(emerald_void);
emerald_void term(emerald_void);
emerald_void termPrime(emerald_void);
emerald_void factor(emerald_void);

#endif