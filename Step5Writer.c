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
***********************************************************
* File name: Writer.c
* Compiler: MS Visual Studio 2022
* Course: CST 8152 – Compilers, Lab Section: [011, 012, 013]
* Assignment: A12.
* Date: Jan 01 2025
* Professor: Paulo Sousa
* Purpose: This file is the main code for Buffer/Reader (A12)
************************************************************
*/

/*
 *.............................................................................
 * MAIN ADVICE:
 * - Please check the "TODO" labels to develop your activity.
 * - Review the functions to use "Defensive Programming".
 *.............................................................................
 */

#ifndef CODER_H_
#include "Step5Writer.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

 /* Global variables */
Variable variables[MAX_VARS];
emerald_intg var_count = 0;
emerald_intg initial_phase = 1; // Flag to track the initial phase
emerald_char output_buffer[MAX_EXPR_LEN * 10] = { 0 }; // Buffer to store write output

/* Finds variables */
emerald_intg find_variable(const emerald_strg name) {
    emerald_intg i = 0;
    for (i = 0; i < var_count; ++i) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Assign numerical variable */
emerald_void assign_numeric_variable(const emerald_strg name, emerald_doub value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = NUMERIC;
    variables[idx].value.num_value = value;
}

/* Assign string variable */
emerald_void assign_string_variable(const emerald_strg name, const emerald_strg value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = STRING;
    snprintf(variables[idx].value.str_value, sizeof(variables[idx].value.str_value), "%s", value);
}

/* Assign boolean variable */
emerald_void assign_boolean_variable(const emerald_strg name, emerald_intg value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = BOOLEAN;
    variables[idx].value.bool_value = value;
}

emerald_intg get_boolean_value(const emerald_strg name) {
    emerald_intg idx = find_variable(name);
    if (idx != -1 && variables[idx].type == BOOLEAN) {
        return variables[idx].value.bool_value;
    }
    return 0; // Default to false if variable not found
}

/* Assign char variable */
emerald_void assign_char_variable(const emerald_strg name, emerald_char value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = CHAR;
    variables[idx].value.char_value = value;
}

/* Gets char variable */
emerald_char get_char_value(const emerald_strg name) {
    emerald_intg idx = find_variable(name);
    if (idx != -1 && variables[idx].type == CHAR) {
        return variables[idx].value.char_value;
    }
    return EOS; // Default to null character if variable not found
}

/* Gets numerical value */
emerald_doub get_numeric_value(const emerald_strg name) {
    int idx = find_variable(name);
    if (idx != -1 && variables[idx].type == NUMERIC) {
        return variables[idx].value.num_value;
    }
    return ZERO;
}

/* Gets string variable */
const emerald_strg get_string_value(const emerald_strg name) {
    emerald_intg idx = find_variable(name);
    if (idx != -1 && variables[idx].type == STRING) {
        return variables[idx].value.str_value;
    }
    return STREMPTY;
}

/* Parsing for term */
emerald_doub parse_term(emerald_strg* expr) {
    emerald_doub value = 0.0;
    while (isspace(**expr)) (*expr)++;
    if (**expr == LPAR) {
        (*expr)++;
        value = parse_expression(expr);
        if (**expr == RPAR) {
            (*expr)++;
        }
    }
    else if (isalpha(**expr)) {
        emerald_char var_name[32] = { 0 };
        emerald_intg i = 0;
        while (isalnum(**expr)) {
            var_name[i++] = *(*expr)++;
        } // Finishes with alpha and number
        var_name[i++] = *(*expr)++; // Includes the suffix
        var_name[i] = EOS;
        value = get_numeric_value(var_name);
    }
    else {
        value = strtod(*expr, expr);
    }
    while (isspace(**expr)) (*expr)++;
    while (**expr == MULT || **expr == DIV) {
        emerald_char op = *(*expr)++;
        emerald_doub next_value = parse_term(expr);
        if (op == MULT) {
            value *= next_value;
        }
        else {
            if (next_value != 0)
                value /= next_value;
            else {
                printf("Error: division by zero! ");
                value = 0;
            }
        }
    }
    return value;
}

/* Parsing for expression */
emerald_doub parse_expression(emerald_strg* expr) {
    emerald_doub value = parse_term(expr);
    while (isspace(**expr)) (*expr)++;
    while (**expr == PLUS || **expr == MINUS) {
        emerald_char op = *(*expr)++;
        emerald_doub next_value = parse_term(expr);
        if (op == PLUS) {
            value += next_value;
        }
        else {
            value -= next_value;
        }
    }
    return value;
}

/* Write output */
emerald_void handle_write(emerald_strg expression) {
    emerald_char buffer[MAX_EXPR_LEN] = { 0 };
    emerald_strg start = strchr(expression, LPAR) + 1;
    emerald_strg end = strrchr(expression, RPAR);
    if (start != NULL && end != NULL && start < end) {
        *end = EOS;
        while (*start != EOS) {
            if (*start == QUOTES) {
                start++;
                while (*start != QUOTES && *start != EOS) {
                    strncat_s(buffer, sizeof(buffer), start, 1);
                    start++;
                }
                start++;
            }
            else if (isalpha(*start)) {
                emerald_char var_name[32] = { 0 };
                emerald_intg i = 0;
                while (isalnum(*start)) {
                    var_name[i++] = *start++;
                } // Finishes with alpha and number
                if (*start == '&') {
                    var_name[i++] = *start++;
                }
                var_name[i] = EOS;
                emerald_intg var_idx = find_variable(var_name);
                if (var_idx != -1) {
                    if (variables[var_idx].type == STRING) {
                        strcat_s(buffer, sizeof(buffer), get_string_value(var_name));
                    }
                    else if (variables[var_idx].type == NUMERIC) {
                        emerald_char num_str[256];
                        snprintf(num_str, sizeof(num_str), "%.2lf", get_numeric_value(var_name));
                        strcat_s(buffer, sizeof(buffer), num_str);
                    }
                    else if (variables[var_idx].type == BOOLEAN) {
                        strcat_s(buffer, sizeof(buffer), get_boolean_value(var_name) ? TRUE : FALSE);
                    }
                    else if (variables[var_idx].type == CHAR) {
                        emerald_char char_str[2] = { get_char_value(var_name), EOS };
                        strcat_s(buffer, sizeof(buffer), char_str);
                    }
                }
            }
            else if (isspace(*start)) {
                strncat_s(buffer, sizeof(buffer), start, 1);
                start++;
            }
            else {
                start++;
            }
        }
    }
    if (initial_phase) {
        strcat_s(output_buffer, sizeof(output_buffer), buffer);
        if (strlen(buffer) == 0) {
            snprintf(buffer, sizeof(buffer), "%s", "[Undefined]");
            strcat_s(output_buffer, sizeof(output_buffer), buffer);
        }
        strcat_s(output_buffer, sizeof(output_buffer), "\n");
        strncat_s(output_buffer, "\n", start, sizeof(output_buffer) - strlen(output_buffer) - 1);
    }
    else {
        printf("%s\n", buffer);
    }
}

/* Calculate expression */
emerald_void calculate(emerald_strg expression) {
    emerald_char var_name[32] = { 0 };
    if (strchr(expression, EQUALS)) {
        sscanf_s(expression, "%31s =", var_name, (unsigned)_countof(var_name));
        emerald_strg expr = strchr(expression, EQUALS) + 1;
        while (isspace(*expr)) expr++;
        if (*expr == QUOTES) {
            expr++;
            emerald_char str_value[256] = { 0 };
            emerald_intg i = 0;
            while (*expr != QUOTES && *expr != EOS) {
                str_value[i++] = *expr++;
            }
            assign_string_variable(var_name, str_value);
            if (!initial_phase) {
                printf("%s = \"%s\"\n", var_name, str_value);
            }
        }
        else if (strncmp(expr, TRUE, 4) == 0 || strncmp(expr, FALSE, 5) == 0) {
            emerald_intg bool_value = strncmp(expr, TRUE, 4) == 0 ? 1 : 0;
            assign_boolean_variable(var_name, bool_value);
            if (!initial_phase) {
                printf("%s = %s\n", var_name, bool_value ? TRUE : FALSE);
            }
        }
        else if (*expr == QUOTE) {
            expr++;
            emerald_char char_value = *expr;
            assign_char_variable(var_name, char_value);
            if (!initial_phase) {
                printf("%s = '%c'\n", var_name, char_value);
            }
        }
        else {
            emerald_doub value = parse_expression(&expr);
            assign_numeric_variable(var_name, value);
            if (!initial_phase) { ///xxx
                printf("%s = %.2lf\n", var_name, value);
            }
        }
    }
    else if (strstr(expression, WRITE)) {
        handle_write(expression);
    }
    else {
        emerald_strg expr = expression;
        emerald_doub result = parse_expression(&expr);
        if (!initial_phase) {
            printf("Result: %.2lf\n", result);
        }
    }
}

/* Process input file */
emerald_void process_file(const emerald_strg filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    emerald_char line[MAX_EXPR_LEN];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == NEWLINE || line[0] == RETURN) {
            continue; // Skip empty lines
        }
        line[strcspn(line, "\n")] = 0; // Remove newline character
        calculate(line);
    }
    fclose(file);
    initial_phase = 0; // End of initial phase
    printf("%s", output_buffer); // Print the buffered write output
    printf("\nVariable values:\n");
    emerald_intg i = 0;
    for (i = 0; i < var_count; i++) {
        if (variables[i].type == STRING) {
            printf("%s = \"%s\"\n", variables[i].name, variables[i].value.str_value);
        }
        else if (variables[i].type == NUMERIC) {
            printf("%s = %.2lf\n", variables[i].name, variables[i].value.num_value);
        }
        else if (variables[i].type == BOOLEAN) {
            printf("%s = %s\n", variables[i].name, variables[i].value.bool_value ? "true" : "false");
        }
        else if (variables[i].type == CHAR) {
            printf("%s = '%c'\n", variables[i].name, variables[i].value.char_value);
        }
    }
}

/* Split lines into components */
emerald_strg* splitIntoLines(const emerald_strg content, emerald_intg* lineCount) {
    emerald_strg* lines = malloc(MAX_LINES * sizeof(char*));
    if (!lines) {
        perror("Error allocating memory");
        return NULL;
    }
    *lineCount = 0;
    const emerald_char* start = content;
    const emerald_char* end;
    while ((end = strchr(start, NEWLINE)) != NULL) {
        if (*lineCount >= MAX_LINES) {
            fprintf(stderr, "Exceeded maximum number of lines\n");
            break;
        }
        emerald_intg lineLength = (emerald_intg)(end - start);
        lines[*lineCount] = malloc(lineLength + 1);
        if (!lines[*lineCount]) {
            perror("Error allocating memory for line");
            break;
        }
        // Use snprintf for safe copying
        snprintf(lines[*lineCount], lineLength + 1, "%.*s", lineLength, start);
        (*lineCount)++;
        start = end + 1;
    }
    // Handle the last line if it doesn't end with a newline
    if (*start != EOS && *lineCount < MAX_LINES) {
        size_t lineLength = strlen(start);
        lines[*lineCount] = malloc(lineLength + 1);
        if (!lines[*lineCount]) {
            perror("Error allocating memory for line");
        }
        else {
            // Use _strdup to copy the remaining content
            lines[*lineCount] = _strdup(start);
            if (!lines[*lineCount]) {
                perror("Error copying string using _strdup");
            }
            else {
                (*lineCount)++;
            }
        }
    }
    return lines;
}

/* Free lines */
emerald_void freeLines(emerald_strg* lines, emerald_intg lineCount) {
    emerald_intg i = 0;
    for (i = 0; i < lineCount; i++) {
        free(lines[i]);
    }
    free(lines);
}

/* Process content */
emerald_void process_content(emerald_strg fileContent) {
    emerald_intg lineCount = 0;
    emerald_strg* lines = splitIntoLines(fileContent, &lineCount);
    emerald_strg line = NULL;
    if (!lines) {
        return;
    }
    printf("Lines from content:\n");
    emerald_intg i = 0;
    for (i = 0; i < lineCount; i++) {
        line = lines[i];
        calculate(line);
    }
    initial_phase = 0; // End of initial phase
    printf("%s", output_buffer); // Print the buffered write output
    printf("\nVariable values:\n");
    for (i = 0; i < var_count; i++) {
        if (variables[i].type == STRING) {
            printf("%s = \"%s\"\n", variables[i].name, variables[i].value.str_value);
        }
        else if (variables[i].type == NUMERIC) {
            printf("%s = %.2lf\n", variables[i].name, variables[i].value.num_value);
        }
        else if (variables[i].type == BOOLEAN) {
            printf("%s = %s\n", variables[i].name, variables[i].value.bool_value ? "true" : "false");
        }
        else if (variables[i].type == CHAR) {
            printf("%s = '%c'\n", variables[i].name, variables[i].value.char_value);
        }
    }
}