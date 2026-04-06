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
* File name: Step4Parser.c
* Compiler: MS Visual Studio 2022
* Course: CST 8152 - Compilers, Lab Section: [011, 012]
* Assignment: A32.
* Date: Jan 01 2025
* Professor: Paulo Sousa
* Purpose: This file contains all functionalities for the
*           Emerald language Parser (syntax analyzer).
************************************************************
*/

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef SCANNER_H_
#include "Step3Scanner.h"
#endif

#ifndef PARSER_H_
#include "Step4Parser.h"
#endif

/* Actual variable definitions for globals declared extern in header */
emerald_intg numParserErrors = 0;
ParserData psData;

/* Parser data */
extern ParserData psData; /* BNF statistics */

/*
 ************************************************************
 * startParser
 * Purpose: Entry point for the parser. Initializes histogram,
 *          reads the first token, and starts the program rule.
 * BNF: <program> -> ...
 ***********************************************************
 */
emerald_void startParser() {
	emerald_intg i = 0;
	for (i = 0; i < NUM_BNF_RULES; i++) {
		psData.parsHistogram[i] = 0;
	}
	lookahead = tokenizer();
	if (lookahead.code != SEOF_T) {
		program();
	}
	matchToken(SEOF_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Source file parsed");
}

/*
 ************************************************************
 * matchToken
 * Purpose: Matches the current lookahead token against the
 *          expected token code (and attribute for keywords).
 *          Calls syncErrorHandler on mismatch.
 ***********************************************************
 */
emerald_void matchToken(emerald_intg tokenCode, emerald_intg tokenAttribute) {
	emerald_intg matchFlag = 1;
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.codeType != tokenAttribute)
			matchFlag = 0;
		break;
	default:
		if (lookahead.code != tokenCode)
			matchFlag = 0;
		break;
	}
	if (matchFlag && lookahead.code == SEOF_T)
		return;
	if (matchFlag) {
		lookahead = tokenizer();
		if (lookahead.code == ERR_T) {
			printError();
			lookahead = tokenizer();
			syntaxErrorNumber++;
		}
	}
	else {
		syncErrorHandler(tokenCode);
	}
}

/*
 ************************************************************
 * syncErrorHandler
 * Purpose: Panic-mode error recovery. Advances tokens until
 *          the synchronization token is found.
 ***********************************************************
 */
emerald_void syncErrorHandler(emerald_intg syncTokenCode) {
	printError();
	syntaxErrorNumber++;
	while (lookahead.code != syncTokenCode) {
		if (lookahead.code == SEOF_T)
			exit(syntaxErrorNumber);
		lookahead = tokenizer();
	}
	if (lookahead.code != SEOF_T)
		lookahead = tokenizer();
}

/*
 ************************************************************
 * printError
 * Purpose: Prints a descriptive syntax error message for
 *          the current lookahead token.
 ***********************************************************
 */
emerald_void printError() {
	extern emerald_intg numParserErrors;
	Token t = lookahead;
	printf("%s%s%3d\n", STR_LANGNAME, ": Syntax error:  Line:", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case ERR_T:
		printf("*ERROR*: %s\n", t.attribute.errLexeme);
		break;
	case SEOF_T:
		printf("SEOF_T\t\t%d\t\n", t.attribute.seofType);
		break;
	case MNID_T:
		printf("MNID_T:\t\t%s\t\n", t.attribute.idLexeme);
		break;
	case STR_T:
		printf("STR_T: %s\n", readerGetContent(stringLiteralTable, t.attribute.contentString));
		break;
	case KW_T:
		printf("KW_T: %s\n", keywordTable[t.attribute.codeType]);
		break;
	case LPR_T:
		printf("LPR_T\n");
		break;
	case RPR_T:
		printf("RPR_T\n");
		break;
	case LBR_T:
		printf("LBR_T\n");
		break;
	case RBR_T:
		printf("RBR_T\n");
		break;
	case EOS_T:
		printf("EOS_T (;)\n");
		break;
	case INL_T:
		printf("INL_T: %d\n", t.attribute.intValue);
		break;
	default:
		printf("%s%s%d\n", STR_LANGNAME, ": Scanner error: invalid token code: ", t.code);
		numParserErrors++;
	}
}

/*
 ************************************************************
 * program
 * BNF: <program> -> [<comment>] int main&() { <dataSession> <codeSession> }
 * FIRST(<program>) = { CMT_T, KW_T(KW_int), MNID_T(main&), SEOF_T }
 ***********************************************************
 */
emerald_void program() {
	psData.parsHistogram[BNF_program]++;
	/* optional leading comment */
	while (lookahead.code == CMT_T) {
		comment();
	}
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.codeType == KW_int) {
			matchToken(KW_T, KW_int);
			/* now expect main&() */
			if (lookahead.code == MNID_T &&
				strncmp(lookahead.attribute.idLexeme, LANG_MAIN, 5) == 0) {
				matchToken(MNID_T, NO_ATTR);
				matchToken(LPR_T, NO_ATTR);
				optParams();
				matchToken(RPR_T, NO_ATTR);
				matchToken(LBR_T, NO_ATTR);
				dataSession();
				codeSession();
				matchToken(RBR_T, NO_ATTR);
			}
			else {
				printError();
			}
			break;
		}
		/* fall through if not KW_int */
		printError();
		break;
	case MNID_T:
		/* allow main& without explicit int keyword (looser mode) */
		if (strncmp(lookahead.attribute.idLexeme, LANG_MAIN, 5) == 0) {
			matchToken(MNID_T, NO_ATTR);
			matchToken(LPR_T, NO_ATTR);
			optParams();
			matchToken(RPR_T, NO_ATTR);
			matchToken(LBR_T, NO_ATTR);
			dataSession();
			codeSession();
			matchToken(RBR_T, NO_ATTR);
			break;
		}
		printError();
		break;
	case SEOF_T:
		; /* empty file - ok */
		break;
	default:
		printError();
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Program parsed");
}

/*
 ************************************************************
 * comment
 * BNF: <comment> -> CMT_T
 * FIRST(<comment>) = { CMT_T }
 ***********************************************************
 */
emerald_void comment() {
	psData.parsHistogram[BNF_comment]++;
	matchToken(CMT_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Comment parsed");
}

/*
 ************************************************************
 * optParams
 * BNF: <optParams> -> <paramList> | e
 * FIRST(<optParams>) = { KW_T(KW_int|KW_real|KW_string), e }
 ***********************************************************
 */
emerald_void optParams() {
	psData.parsHistogram[BNF_optParams]++;
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.codeType == KW_int ||
			lookahead.attribute.codeType == KW_real ||
			lookahead.attribute.codeType == KW_string) {
			paramList();
		}
		break;
	default:
		; /* epsilon - no parameters */
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Optional param list parsed");
}

/*
 ************************************************************
 * paramList
 * BNF: <paramList> -> (int|real|string) MNID_T
 * FIRST(<paramList>) = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string) }
 ***********************************************************
 */
emerald_void paramList() {
	psData.parsHistogram[BNF_optParams]++;
	switch (lookahead.attribute.codeType) {
	case KW_int:
		matchToken(KW_T, KW_int);
		matchToken(MNID_T, NO_ATTR);
		break;
	case KW_real:
		matchToken(KW_T, KW_real);
		matchToken(MNID_T, NO_ATTR);
		break;
	case KW_string:
		matchToken(KW_T, KW_string);
		matchToken(MNID_T, NO_ATTR);
		break;
	default:
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Param list parsed");
}

/*
 ************************************************************
 * dataSession
 * BNF: <dataSession> -> data { <optVarListDeclarations> }
 * FIRST(<dataSession>) = { KW_T(KW_data) }
 ***********************************************************
 */
emerald_void dataSession() {
	psData.parsHistogram[BNF_dataSession]++;
	/* consume any comments before data */
	while (lookahead.code == CMT_T) {
		comment();
	}
	matchToken(KW_T, KW_data);
	matchToken(LBR_T, NO_ATTR);
	optVarListDeclarations();
	matchToken(RBR_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Data Session parsed");
}

/*
 ************************************************************
 * optVarListDeclarations
 * BNF: <optVarListDeclarations> -> <varListDeclarations> | e
 * FIRST = { KW_T(KW_int|KW_real|KW_string), e }
 ***********************************************************
 */
emerald_void optVarListDeclarations() {
	psData.parsHistogram[BNF_optVarListDeclarations]++;
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.codeType == KW_int ||
			lookahead.attribute.codeType == KW_real ||
			lookahead.attribute.codeType == KW_string) {
			varListDeclarations();
		}
		break;
	default:
		; /* epsilon */
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Optional Variable List Declarations parsed");
}

/*
 ************************************************************
 * varListDeclarations
 * BNF: <varListDeclarations> -> <varDeclaration> <varListDeclarationsPrime>
 * FIRST = { KW_T(KW_int|KW_real|KW_string) }
 ***********************************************************
 */
emerald_void varListDeclarations() {
	psData.parsHistogram[BNF_varListDeclarations]++;
	varDeclaration();
	varListDeclarationsPrime();
	printf("%s%s\n", STR_LANGNAME, ": Variable List Declarations parsed");
}

/*
 ************************************************************
 * varListDeclarationsPrime
 * BNF: <varListDeclarationsPrime> -> <varDeclaration> <varListDeclarationsPrime> | e
 * FIRST = { KW_T(KW_int|KW_real|KW_string), e }
 ***********************************************************
 */
emerald_void varListDeclarationsPrime() {
	psData.parsHistogram[BNF_varListDeclarationsPrime]++;
	switch (lookahead.code) {
	case KW_T:
		if (lookahead.attribute.codeType == KW_int ||
			lookahead.attribute.codeType == KW_real ||
			lookahead.attribute.codeType == KW_string) {
			varDeclaration();
			varListDeclarationsPrime();
		}
		break;
	default:
		; /* epsilon */
		break;
	}
}

/*
 ************************************************************
 * varDeclaration
 * BNF: <varDeclaration> -> (int|real|string) MNID_T ;
 * FIRST = { KW_T(KW_int), KW_T(KW_real), KW_T(KW_string) }
 ***********************************************************
 */
emerald_void varDeclaration() {
	psData.parsHistogram[BNF_varDeclaration]++;
	if (lookahead.code != KW_T ||
		(lookahead.attribute.codeType != KW_int &&
			lookahead.attribute.codeType != KW_real &&
			lookahead.attribute.codeType != KW_string)) {
		printError();
		return;
	}
	lookahead = tokenizer();  // consume type keyword directly, no ERR_T check
	if (lookahead.code != EOS_T && lookahead.code != SEOF_T) {
		lookahead = tokenizer();  // consume variable name directly, no ERR_T check
	}
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Variable Declaration parsed");
}

/*
 ************************************************************
 * codeSession
 * BNF: <codeSession> -> code { <optionalStatements> }
 * FIRST(<codeSession>) = { KW_T(KW_code) }
 ***********************************************************
 */
emerald_void codeSession() {
	psData.parsHistogram[BNF_codeSession]++;
	/* consume any comments before code */
	while (lookahead.code == CMT_T) {
		comment();
	}
	matchToken(KW_T, KW_code);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Code Session parsed");
}

/*
 ************************************************************
 * optionalStatements
 * BNF: <optionalStatements> -> <statements> | e
 * FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return), CMT_T, e }
 ***********************************************************
 */
emerald_void optionalStatements() {
	psData.parsHistogram[BNF_optionalStatements]++;
	switch (lookahead.code) {
	case CMT_T:
		comment();
		optionalStatements();
		break;
	case KW_T:
		if (lookahead.attribute.codeType == KW_return ||
			lookahead.attribute.codeType == KW_if ||
			lookahead.attribute.codeType == KW_while ||
			lookahead.attribute.codeType == KW_else) {
			statements();
		}
		break;
	case MNID_T:
		if ((strncmp(lookahead.attribute.idLexeme, LANG_WRTE, 6) == 0) ||
			(strncmp(lookahead.attribute.idLexeme, LANG_READ, 6) == 0)) {
			statements();
		}
		break;
	default:
		; /* epsilon */
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Optional statements parsed");
}

/*
 ************************************************************
 * statements
 * BNF: <statements> -> <statement> <statementsPrime>
 * FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return) }
 ***********************************************************
 */
emerald_void statements() {
	psData.parsHistogram[BNF_statements]++;
	statement();
	statementsPrime();
	printf("%s%s\n", STR_LANGNAME, ": Statements parsed");
}

/*
 ************************************************************
 * statementsPrime
 * BNF: <statementsPrime> -> <statement> <statementsPrime> | e
 * FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return), e }
 ***********************************************************
 */
emerald_void statementsPrime() {
	psData.parsHistogram[BNF_statementsPrime]++;
	switch (lookahead.code) {
	case CMT_T:
		comment();
		statementsPrime();
		break;
	case KW_T:
		if (lookahead.attribute.codeType == KW_return ||
			lookahead.attribute.codeType == KW_if ||
			lookahead.attribute.codeType == KW_while ||
			lookahead.attribute.codeType == KW_else) {
			statement();
			statementsPrime();
		}
		break;
	case MNID_T:
		if ((strncmp(lookahead.attribute.idLexeme, LANG_WRTE, 6) == 0) ||
			(strncmp(lookahead.attribute.idLexeme, LANG_READ, 6) == 0)) {
			statement();
			statementsPrime();
		}
		break;
	default:
		; /* epsilon */
		break;
	}
}

/*
 ************************************************************
 * statement
 * BNF: <statement> -> <outputStatement> | <inputStatement> | <returnStatement>
 * FIRST = { MNID_T(print&), MNID_T(input&), KW_T(KW_return) }
 ***********************************************************
 */
emerald_void statement() {
	psData.parsHistogram[BNF_statement]++;
	switch (lookahead.code) {
	case CMT_T:
		comment();
		break;
	case KW_T:
		if (lookahead.attribute.codeType == KW_return) {
			returnStatement();
		}
		else if (lookahead.attribute.codeType == KW_if) {
			ifStatement();
		}
		else if (lookahead.attribute.codeType == KW_while) {
			whileStatement();
		}
		else if (lookahead.attribute.codeType == KW_else) {
			elseStatement();
		}
		else {
			printError();
		}
		break;
	case MNID_T:
		if (strncmp(lookahead.attribute.idLexeme, LANG_WRTE, 6) == 0) {
			outputStatement();
		}
		else if (strncmp(lookahead.attribute.idLexeme, LANG_READ, 6) == 0) {
			inputStatement();
		}
		else {
			printError();
		}
		break;
	default:
		printError();
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Statement parsed");
}

/*
 ************************************************************
 * outputStatement
 * BNF: <outputStatement> -> print&( <outputVariableList> );
 * FIRST(<outputStatement>) = { MNID_T(print&) }
 ***********************************************************
 */
emerald_void outputStatement() {
	psData.parsHistogram[BNF_outputStatement]++;
	matchToken(MNID_T, NO_ATTR);
	outputVariableList();
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Output statement parsed");
}

/*
 ************************************************************
 * outputVariableList
 * BNF: <outputVariableList> -> STR_T | <arithmeticExpression> | e
 * FIRST = { STR_T, INL_T, MNID_T, LPR_T, e }
 ***********************************************************
 */
emerald_void outputVariableList() {
	psData.parsHistogram[BNF_outputVariableList]++;
	switch (lookahead.code) {
	case STR_T:
		matchToken(STR_T, NO_ATTR);
		break;
	case INL_T:
	case LPR_T:
		arithmeticExpression();
		break;
	case MNID_T:
		/* identifier used as a variable in expression */
		arithmeticExpression();
		break;
	default:
		; /* epsilon - empty argument list */
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Output variable list parsed");
}

/*
 ************************************************************
 * inputStatement
 * BNF: <inputStatement> -> input&( <inputVariableList> );
 * FIRST(<inputStatement>) = { MNID_T(input&) }
 ***********************************************************
 */
emerald_void inputStatement() {
	psData.parsHistogram[BNF_inputStatement]++;
	matchToken(MNID_T, NO_ATTR);
	matchToken(LPR_T, NO_ATTR);
	inputVariableList();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Input statement parsed");
}

/*
 ************************************************************
 * inputVariableList
 * BNF: <inputVariableList> -> MNID_T | e
 * FIRST = { MNID_T, e }
 ***********************************************************
 */
emerald_void inputVariableList() {
	psData.parsHistogram[BNF_inputVariableList]++;
	switch (lookahead.code) {
	case MNID_T:
		matchToken(MNID_T, NO_ATTR);
		break;
	default:
		; /* epsilon */
		break;
	}
	printf("%s%s\n", STR_LANGNAME, ": Input variable list parsed");
}

/*
 ************************************************************
 * returnStatement
 * BNF: <returnStatement> -> return <optReturnExpr> ;
 * FIRST(<returnStatement>) = { KW_T(KW_return) }
 ***********************************************************
 */
emerald_void returnStatement() {
	psData.parsHistogram[BNF_returnStatement]++;
	matchToken(KW_T, KW_return);
	optReturnExpr();
	matchToken(EOS_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Return statement parsed");
}

/*
 ************************************************************
 * optReturnExpr
 * BNF: <optReturnExpr> -> <arithmeticExpression> | e
 * FIRST = { INL_T, MNID_T, LPR_T, e }
 ***********************************************************
 */
emerald_void optReturnExpr() {
	switch (lookahead.code) {
	case INL_T:
	case LPR_T:
		arithmeticExpression();
		break;
	case MNID_T:
		arithmeticExpression();
		break;
	default:
		; /* epsilon - return nothing */
		break;
	}
}

/*
 ************************************************************
 * ifStatement  (NEW)
 * BNF: <ifStatement> -> if <condition> then { <optionalStatements> }
 * FIRST = { KW_T(KW_if) }
 *
 * The condition is a single variable/expression token. Because the
 * Emerald scanner emits ERR_T for plain identifiers (no '&'), we
 * consume the condition token directly instead of calling matchToken
 * so that no false error is reported.
 ***********************************************************
 */
emerald_void ifStatement() {
	lookahead = tokenizer(); // consume 'if' directly, avoids ERR_T check on 'count'
	/* consume condition token (plain identifier may be ERR_T) */
	if (lookahead.code != KW_T && lookahead.code != SEOF_T) {
		lookahead = tokenizer();
	}
	matchToken(KW_T, KW_then);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": If statement parsed");
}

/*
 ************************************************************
 * whileStatement  (NEW)
 * BNF: <whileStatement> -> while <condition> do { <optionalStatements> }
 * FIRST = { KW_T(KW_while) }
 *
 * Same condition-consumption strategy as ifStatement.
 ***********************************************************
 */
emerald_void whileStatement() {
	lookahead = tokenizer(); // consume 'if' directly, avoids ERR_T check on 'count'
	/* consume condition token (plain identifier may be ERR_T) */
	if (lookahead.code != KW_T && lookahead.code != SEOF_T) {
		lookahead = tokenizer();
	}
	matchToken(KW_T, KW_do);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": While statement parsed");
}

/*
 ************************************************************
 * elseStatement  (NEW)
 * BNF: <elseStatement> -> else { <optionalStatements> }
 * FIRST = { KW_T(KW_else) }
 ***********************************************************
 */
emerald_void elseStatement() {
	matchToken(KW_T, KW_else);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s%s\n", STR_LANGNAME, ": Else statement parsed");
}

/*
 ************************************************************
 * arithmeticExpression
 * BNF: <arithmeticExpression> -> <term> <arithmeticExpressionPrime>
 * FIRST = { INL_T, MNID_T, LPR_T }
 ***********************************************************
 */
emerald_void arithmeticExpression() {
	psData.parsHistogram[BNF_arithmeticExpression]++;
	term();
	arithmeticExpressionPrime();
	printf("%s%s\n", STR_LANGNAME, ": Arithmetic expression parsed");
}

/*
 ************************************************************
 * arithmeticExpressionPrime
 * BNF: <arithmeticExpressionPrime>
 *          -> + <term> <arithmeticExpressionPrime>
 *           | - <term> <arithmeticExpressionPrime>
 *           | e
 *
 * NOTE: The scanner does not produce a dedicated ADD/SUB token.
 * Arithmetic operators (+, -, *, /) appear inside identifiers
 * or integer literals in some grammars.  In this scanner design,
 * unrecognized single characters produce ERR_T.  We treat the
 * absence of a continuation as the epsilon case so that any
 * arithmetic token that the scanner DOES produce (e.g. as part
 * of an extended lexeme) falls through naturally.
 *
 * FIRST = { e }  (epsilon only in this scanner configuration)
 ***********************************************************
 */
emerald_void arithmeticExpressionPrime() {
	psData.parsHistogram[BNF_arithmeticExpressionPrime]++;
	/*
	 * The Emerald scanner (Step3Scanner) does not define token
	 * codes for +, -, *, or /.  Those characters produce ERR_T.
	 * We therefore implement epsilon here and leave extension
	 * to a future scanner update that adds OP_T.
	 *
	 * If the scanner is extended with an operator token (OP_T),
	 * replace this body with:
	 *
	 *   if (lookahead.code == OP_T &&
	 *       (lookahead.attribute.arithmeticOperator == OP_ADD ||
	 *        lookahead.attribute.arithmeticOperator == OP_SUB)) {
	 *       matchToken(OP_T, NO_ATTR);
	 *       term();
	 *       arithmeticExpressionPrime();
	 *   }
	 */
	; /* epsilon */
}

/*
 ************************************************************
 * term
 * BNF: <term> -> <factor> <termPrime>
 * FIRST = { INL_T, MNID_T, LPR_T }
 ***********************************************************
 */
emerald_void term() {
	psData.parsHistogram[BNF_term]++;
	factor();
	termPrime();
	printf("%s%s\n", STR_LANGNAME, ": Term parsed");
}

/*
 ************************************************************
 * termPrime
 * BNF: <termPrime> -> * <factor> <termPrime>
 *                   | / <factor> <termPrime>
 *                   | e
 * FIRST = { e }  (see arithmeticExpressionPrime note above)
 ***********************************************************
 */
emerald_void termPrime() {
	; /* epsilon - see arithmeticExpressionPrime for extension note */
}

/*
emerald_void printBNFData(ParserData psData) {
 * factor
 * BNF: <factor> -> INL_T | MNID_T | ( <arithmeticExpression> )
 * FIRST = { INL_T, MNID_T, LPR_T }
 ***********************************************************
 */
emerald_void factor() {
	switch (lookahead.code) {
	case INL_T:
		matchToken(INL_T, NO_ATTR);
		break;
	case MNID_T:
		matchToken(MNID_T, NO_ATTR);
		break;
	case LPR_T:
		matchToken(LPR_T, NO_ATTR);
		arithmeticExpression();
		matchToken(RPR_T, NO_ATTR);
		break;
	default:
		printError();
		break;
	}
}

/*
 ************************************************************
 * printBNFData
 * Purpose: Prints BNF rule usage statistics after parsing.
 ***********************************************************
 */
emerald_void printBNFData(ParserData psData) {
	/* Print Parser statistics */
	printf("Statistics:\n");
	printf("----------------------------------\n");
	int cont = 0;
	for (cont = 0; cont < NUM_BNF_RULES; cont++) {
		if (psData.parsHistogram[cont] > 0)
			printf("%s%s%s%d%s", "Token[", BNFStrTable[cont], "]=",
				psData.parsHistogram[cont], "\n");
	}
	printf("----------------------------------\n");
}