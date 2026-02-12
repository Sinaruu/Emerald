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
# ECHO "    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ”
# ECHO "    @@                             @@    ”
# ECHO "    @@           %&@@@@@@@@@@@     @@    ”
# ECHO "    @@       @%% (@@@@@@@@@  @     @@    ”
# ECHO "    @@      @& @   @ @       @     @@    ”
# ECHO "    @@     @ @ %  / /   @@@@@@     @@    ”
# ECHO "    @@      & @ @  @@              @@    ”
# ECHO "    @@       @/ @*@ @ @   @        @@    ”
# ECHO "    @@           @@@@  @@ @ @      @@    ”
# ECHO "    @@            /@@    @@@ @     @@    ”
# ECHO "    @@     @      / /     @@ @     @@    ”
# ECHO "    @@     @ @@   /@/   @@@ @      @@    ”
# ECHO "    @@     @@@@@@@@@@@@@@@         @@    ”
# ECHO "    @@                             @@    ”
# ECHO "    @@         S O F I A           @@    ”
# ECHO "    @@                             @@    ”
# ECHO "    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    ”
# ECHO "                                         "
# ECHO "[CODER SCRIPT ..........................]"
# ECHO "                                         "
*/

/*
***********************************************************
* File name: Reader.c
* Compiler: MS Visual Studio 2022
* Course: CST 8152 – Compilers, Lab Section: [011, 012, 013]
* Assignment: A12.
* Date: Sep 01 2025
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef CODER_H_
#include "Step1Coder.h"
#endif

 // Function to perform the Vigenère cipher (encoding or decoding)
void vigenereFile(const emerald_strg inputFileName, const emerald_strg outputFileName, const emerald_strg key, emerald_intg encode) {
	// Defensive programming - check parameters
	if (!inputFileName || !outputFileName || !key) {
		errorPrint("Error: NULL parameter passed to vigenereFile\n");
		return;
	}

	if (encode != CYPHER && encode != DECYPHER) {
		errorPrint("Error: Invalid operation. Use CYPHER or DECYPHER\n");
		return;
	}

	// Check key length before opening/truncating output file
	emerald_intg keyLen = (emerald_intg)strlen(key);
	if (keyLen == 0) {
		errorPrint("Error: Empty key provided\n");
		return;
	}

	// Define the input and output files
	FILE* inputFile = fopen(inputFileName, "r");
	FILE* outputFile = fopen(outputFileName, "w");

	// Check if files opened successfully
	if (!inputFile) {
		errorPrint("Error: Cannot open input file %s\n", inputFileName);
		return;
	}

	if (!outputFile) {
		errorPrint("Error: Cannot open output file %s\n", outputFileName);
		fclose(inputFile);
		return;
	}

	// Define local variables
	emerald_intg ch;
	emerald_intg keyIndex = 0;

	// Check for empty key
	if (keyLen == 0) {
		errorPrint("Error: Empty key provided\n");
		fclose(inputFile);
		fclose(outputFile);
		return;
	}

	// Logic: check if it is encode / decode to change the char (using Vigenere algorithm)
	while ((ch = fgetc(inputFile)) != EOF) {
		// Only process visible ASCII characters
		if (ch >= ASCII_START && ch <= ASCII_END) {
			emerald_intg keyChar = key[keyIndex % keyLen];
			emerald_intg shift = keyChar - ASCII_START;
			emerald_intg newChar;

			if (encode == CYPHER) {
				// Encoding: shift forward
				newChar = ((ch - ASCII_START + shift) % ASCII_RANGE) + ASCII_START;
			}
			else {
				// Decoding: shift backward
				newChar = ((ch - ASCII_START - shift + ASCII_RANGE) % ASCII_RANGE) + ASCII_START;
			}

			fputc(newChar, outputFile);
			keyIndex++;
		}
		else {
			// Non-visible characters are written as-is
			fputc(ch, outputFile);
		}
	}

	// Close the files
	fclose(inputFile);
	fclose(outputFile);
}

// Function to perform the Vigenère cipher (encoding or decoding) in memory
emerald_strg vigenereMem(const emerald_strg inputFileName, const emerald_strg key, emerald_intg encode) {
	// Defensive programming - check parameters
	if (!inputFileName || !key) {
		errorPrint("Error: NULL parameter passed to vigenereMem\n");
		return NULL;
	}

	if (encode != CYPHER && encode != DECYPHER) {
		errorPrint("Error: Invalid operation. Use CYPHER or DECYPHER\n");
		return NULL;
	}

	// Get file size and allocate memory
	emerald_intg size = getSizeOfFile(inputFileName);
	if (size < 0) {
		errorPrint("Error: Cannot get file size\n");
		return NULL;
	}

	emerald_strg output = (emerald_strg)malloc(size + 1);
	if (!output) {
		errorPrint("Error: Memory allocation failed\n");
		return NULL;
	}

	// Open input file
	FILE* inputFile = fopen(inputFileName, "r");
	if (!inputFile) {
		errorPrint("Error: Cannot open input file %s\n", inputFileName);
		free(output);
		return NULL;
	}

	// Define local variables
	emerald_intg ch;
	emerald_intg keyLen = (emerald_intg)strlen(key);
	emerald_intg keyIndex = 0;
	emerald_intg outputIndex = 0;

	// Check for empty key
	if (keyLen == 0) {
		errorPrint("Error: Empty key provided\n");
		fclose(inputFile);
		free(output);
		return NULL;
	}

	// Logic to code/decode - considering visible chars only
	while ((ch = fgetc(inputFile)) != EOF && outputIndex < size) {
		// Only process visible ASCII characters
		if (ch >= ASCII_START && ch <= ASCII_END) {
			emerald_intg keyChar = key[keyIndex % keyLen];
			emerald_intg shift = keyChar - ASCII_START;
			emerald_intg newChar;

			if (encode == CYPHER) {
				// Encoding: shift forward
				newChar = ((ch - ASCII_START + shift) % ASCII_RANGE) + ASCII_START;
			}
			else {
				// Decoding: shift backward
				newChar = ((ch - ASCII_START - shift + ASCII_RANGE) % ASCII_RANGE) + ASCII_START;
			}

			output[outputIndex++] = (emerald_char)newChar;
			keyIndex++;
		}
		else {
			// Non-visible characters are copied as-is
			output[outputIndex++] = (emerald_char)ch;
		}
	}

	// Null-terminate the string safely
	if (outputIndex < size + 1) {
		output[outputIndex] = '\0';
	} else if (size > 0) {
		output[size] = '\0';
	}

	// Close the file
	fclose(inputFile);

	return output;
}

// Function to encode (cypher)
void cypher(const emerald_strg inputFileName, const emerald_strg outputFileName, const emerald_strg key) {
	vigenereFile(inputFileName, outputFileName, key, CYPHER);
}

// Function to decode (decypher)
void decypher(const emerald_strg inputFileName, const emerald_strg outputFileName, const emerald_strg key) {
	vigenereFile(inputFileName, outputFileName, key, DECYPHER);
}

// Get file size (util method)
emerald_intg getSizeOfFile(const emerald_strg filename) {
	// Defensive programming - check parameter
	if (!filename) {
		errorPrint("Error: NULL filename passed to getSizeOfFile\n");
		return -1;
	}

	emerald_intg size = 0;
	FILE* file = fopen(filename, "r");

	// Check if file opened successfully
	if (!file) {
		errorPrint("Error: Cannot open file %s\n", filename);
		return -1;
	}

	// Use the logic to get the size of the file
	// Move to the end of the file
	if (fseek(file, 0, SEEK_END) != 0) {
		errorPrint("Error: Cannot seek to end of file\n");
		fclose(file);
		return -1;
	}

	// Get the current position (file size)
	size = (emerald_intg)ftell(file);
	if (size < 0) {
		errorPrint("Error: Cannot get file size\n");
		fclose(file);
		return -1;
	}

	// Close the file
	fclose(file);

	return size;
}
