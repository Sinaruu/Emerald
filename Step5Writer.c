/*
************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Fall, 2025
* Author: Egor Kivilev, Hoang Thien Loc Ngyuen
* Professors: Paulo Sousa
************************************************************
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

#ifndef WRITER_H_
#include "Step5Writer.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Variable variables[MAX_VARS];
emerald_intg var_count = 0;
emerald_intg initial_phase = 1;
emerald_char output_buffer[MAX_EXPR_LEN * 10] = { 0 };

emerald_intg find_variable(const emerald_strg name) {
    emerald_intg i = 0;
    for (i = 0; i < var_count; ++i) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

emerald_void assign_numeric_variable(const emerald_strg name, emerald_doub value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = NUMERIC;
    variables[idx].value.num_value = value;
}

emerald_void assign_string_variable(const emerald_strg name, const emerald_strg value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = STRING;
    snprintf(variables[idx].value.str_value, sizeof(variables[idx].value.str_value), "%s", value);
}

emerald_void assign_boolean_variable(const emerald_strg name, emerald_intg value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = BOOLEAN;
    variables[idx].value.bool_value = value;
}

emerald_void assign_char_variable(const emerald_strg name, emerald_char value) {
    emerald_intg idx = find_variable(name);
    if (idx == -1) {
        idx = var_count++;
        snprintf(variables[idx].name, sizeof(variables[idx].name), "%s", name);
    }
    variables[idx].type = CHAR;
    variables[idx].value.char_value = value;
}

emerald_intg get_boolean_value(const emerald_strg name) {
    emerald_intg idx = find_variable(name);
    if (idx != -1 && variables[idx].type == BOOLEAN) {
        return variables[idx].value.bool_value;
    }
    return 0;
}

emerald_char get_char_value(const emerald_strg name) {
    emerald_intg idx = find_variable(name);
    if (idx != -1 && variables[idx].type == CHAR) {
        return variables[idx].value.char_value;
    }
    return EOS;
}

emerald_doub get_numeric_value(const emerald_strg name) {
    int idx = find_variable(name);
    if (idx != -1 && variables[idx].type == NUMERIC) {
        return variables[idx].value.num_value;
    }
    return ZERO;
}

const emerald_strg get_string_value(const emerald_strg name) {
    emerald_intg idx = find_variable(name);
    if (idx != -1 && variables[idx].type == STRING) {
        return variables[idx].value.str_value;
    }
    return STREMPTY;
}

emerald_doub parse_term(emerald_strg* expr) {
    emerald_doub value = 0.0;
    while (isspace((unsigned char)**expr)) (*expr)++;

    if (**expr == LPAR) {
        (*expr)++;
        value = parse_expression(expr);
        if (**expr == RPAR) {
            (*expr)++;
        }
    }
    else if (isalpha((unsigned char)**expr)) {
        emerald_char var_name[32] = { 0 };
        emerald_intg i = 0;
        while (isalnum((unsigned char)**expr) && i < 31) {
            var_name[i++] = *(*expr)++;
        }
        if (**expr == '&') {
            var_name[i++] = *(*expr)++;
        }
        var_name[i] = EOS;
        value = get_numeric_value(var_name);
    }
    else {
        value = strtod(*expr, expr);
    }

    while (isspace((unsigned char)**expr)) (*expr)++;

    while (**expr == MULT || **expr == DIV) {
        emerald_char op = *(*expr)++;
        while (isspace((unsigned char)**expr)) (*expr)++;
        emerald_doub next_value = parse_term(expr);
        if (op == MULT) {
            value *= next_value;
        }
        else {
            if (next_value != 0.0)
                value /= next_value;
            else {
                printf("Error: division by zero! ");
                value = 0.0;
            }
        }
    }
    return value;
}

emerald_doub parse_expression(emerald_strg* expr) {
    emerald_doub value = parse_term(expr);
    while (isspace((unsigned char)**expr)) (*expr)++;
    while (**expr == PLUS || **expr == MINUS) {
        emerald_char op = *(*expr)++;
        while (isspace((unsigned char)**expr)) (*expr)++;
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

emerald_void handle_write(emerald_strg expression) {
    emerald_char buffer[MAX_EXPR_LEN] = { 0 };

    emerald_strg start = strchr(expression, LPAR);
    if (!start) return;
    start++;

    emerald_strg end = strrchr(expression, RPAR);
    if (!end || end <= start) return;
    *end = EOS; 

    emerald_intg first_arg = 1;
    while (*start != EOS) {
        while (isspace((unsigned char)*start)) start++;
        if (*start == ',') {
            start++;
            if (!first_arg) {
                strcat_s(buffer, sizeof(buffer), " ");
            }
            continue;
        }
        if (*start == EOS) break;

        first_arg = 0;

        if (*start == QUOTES) {
            start++;
            while (*start != QUOTES && *start != EOS) {
                strncat_s(buffer, sizeof(buffer), start, 1);
                start++;
            }
            if (*start == QUOTES) start++;
        }
        else if (isalpha((unsigned char)*start)) {
            emerald_char var_name[32] = { 0 };
            emerald_intg i = 0;
            while (isalnum((unsigned char)*start) && i < 31) {
                var_name[i++] = *start++;
            }
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
                    emerald_doub num = get_numeric_value(var_name);
                    emerald_char num_str[64];
                    if (num == (long long)num)
                        snprintf(num_str, sizeof(num_str), "%lld", (long long)num);
                    else
                        snprintf(num_str, sizeof(num_str), "%.2lf", num);
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
            if (*start != EOS && *start != RPAR && *start != ',') {
                start++;
            }
        }
        else if (isspace((unsigned char)*start)) {
            start++;
        }
        else {
            start++;
        }
    }

    if (initial_phase) {
        strcat_s(output_buffer, sizeof(output_buffer), buffer);
        if (strlen(buffer) == 0) {
            strcat_s(output_buffer, sizeof(output_buffer), "[Undefined]");
        }
        strcat_s(output_buffer, sizeof(output_buffer), "\n");
    }
    else {
        printf("%s\n", buffer);
    }
}

emerald_void calculate(emerald_strg expression) {
    emerald_char var_name[32] = { 0 };

    emerald_intg len = (emerald_intg)strlen(expression);
    if (len > 0 && expression[len - 1] == RETURN) {
        expression[len - 1] = EOS;
        len--;
    }

    if (len == 0) return;

    if (strchr(expression, EQUALS)) {
        sscanf_s(expression, "%31s =", var_name, (unsigned)_countof(var_name));
        emerald_intg vlen = (emerald_intg)strlen(var_name);
        if (vlen > 0 && var_name[vlen - 1] == '=') var_name[vlen - 1] = EOS;

        emerald_strg expr = strchr(expression, EQUALS) + 1;
        while (isspace((unsigned char)*expr)) expr++;

        if (*expr == QUOTES) {
            expr++;
            emerald_char str_value[256] = { 0 };
            emerald_intg i = 0;
            while (*expr != QUOTES && *expr != EOS) {
                str_value[i++] = *expr++;
            }
            str_value[i] = EOS;
            assign_string_variable(var_name, str_value);
            if (!initial_phase) {
                printf("%s = \"%s\"\n", var_name, str_value);
            }
        }
        else if (strncmp(expr, TRUE, 4) == 0 || strncmp(expr, FALSE, 5) == 0) {
            emerald_intg bool_value = (strncmp(expr, TRUE, 4) == 0) ? 1 : 0;
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
            if (!initial_phase) {
                if (value == (long long)value)
                    printf("%s = %lld\n", var_name, (long long)value);
                else
                    printf("%s = %.2lf\n", var_name, value);
            }
        }
    }
    else if (strstr(expression, WRITE) || strstr(expression, "write(")) {
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
        snprintf(lines[*lineCount], lineLength + 1, "%.*s", lineLength, start);
        (*lineCount)++;
        start = end + 1;
    }

    if (*start != EOS && *lineCount < MAX_LINES) {
        lines[*lineCount] = _strdup(start);
        if (!lines[*lineCount]) {
            perror("Error copying string using strdup");
        }
        else {
            (*lineCount)++;
        }
    }
    return lines;
}

emerald_void freeLines(emerald_strg* lines, emerald_intg lineCount) {
    emerald_intg i = 0;
    for (i = 0; i < lineCount; i++) {
        free(lines[i]);
    }
    free(lines);
}

emerald_void process_file(const emerald_strg filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    emerald_strg fileContent = malloc(fileSize + 1);
    if (!fileContent) {
        perror("Error allocating memory");
        fclose(file);
        return;
    }
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = EOS;
    fclose(file);

    process_content(fileContent);
    free(fileContent);
}

emerald_void process_content(emerald_strg fileContent) {
    emerald_intg lineCount = 0;
    emerald_strg* lines = splitIntoLines(fileContent, &lineCount);
    if (!lines) return;

    printf("Lines from content:\n");
    emerald_intg i = 0;
    for (i = 0; i < lineCount; i++) {
        calculate(lines[i]);
    }

    initial_phase = 0;

    printf("%s", output_buffer);

    printf("\nVariable values:\n");
    for (i = 0; i < var_count; i++) {
        if (variables[i].type == STRING) {
            printf("%s = \"%s\"\n", variables[i].name, variables[i].value.str_value);
        }
        else if (variables[i].type == NUMERIC) {
            emerald_doub v = variables[i].value.num_value;
            if (v == (long long)v)
                printf("%s = %lld\n", variables[i].name, (long long)v);
            else
                printf("%s = %.2lf\n", variables[i].name, v);
        }
        else if (variables[i].type == BOOLEAN) {
            printf("%s = %s\n", variables[i].name, variables[i].value.bool_value ? "true" : "false");
        }
        else if (variables[i].type == CHAR) {
            printf("%s = '%c'\n", variables[i].name, variables[i].value.char_value);
        }
    }

    freeLines(lines, lineCount);
}