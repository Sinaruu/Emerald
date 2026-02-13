/*
************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Summer, 2025
* Author: Egor Kivilev, Hoang Thien Loc Ngyuen
* Professors: Paulo Sousa
************************************************************
#
# ECHO "=---------------------------------------="
# ECHO "|  COMPILERS - ALGONQUIN COLLEGE (S25)  |"
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
# ECHO "[READER SCRIPT .........................]"
# ECHO "                                         "
*/

/*
***********************************************************
* File name: Reader.c
* Compiler: MS Visual Studio 2022
* Course: CST 8152 – Compilers, Lab Section: [011, 012, 013]
* Assignment: A12.
* Date: Jan 01 2025
* Professor: Paulo Sousa
* Purpose: This file is the main code for Buffer/Reader (A12)
************************************************************
*/

#include <ctype.h>
#include <string.h>

#ifndef COMPILERS_H_
#include "Compilers.h"
#endif

#ifndef CODER_H_
#include "Step1Coder.h"
#endif

#ifndef READER_H_
#include "Step2Reader.h"
#endif

/*
***********************************************************
* Function name: readerCreate
* Purpose: Creates the buffer reader according to capacity, increment
	factor and operational mode ('f', 'a', 'm')
* Author: Svillen Ranev / Paulo Sousa
* History/Versions: S22
* Called functions: calloc(), malloc()
* Parameters:
*   size = initial capacity
*   increment = increment factor
*   mode = operational mode
* Return value: bPointer (pointer to reader)
* Algorithm: Allocation of memory according to inicial (default) values.
*************************************************************
*/

BufferPointer readerCreate(emerald_intg size, emerald_real factor) {
	BufferPointer readerPointer = NULL;

	// Defensive programming: validate size
	if (size < 0 || size > READER_MAX_SIZE) {
		size = READER_DEFAULT_SIZE;
	}

	// Defensive programming: validate factor
	if (factor < 0) {
		factor = READER_DEFAULT_FACTOR;
	}

	// Allocate reader structure
	readerPointer = (BufferPointer)calloc(1, sizeof(Buffer));
	if (readerPointer == NULL) {
		return NULL;
	}

	// Allocate content buffer
	readerPointer->content = (emerald_strg)malloc(size);
	if (readerPointer->content == NULL) {
		free(readerPointer);  // Clean up before returning
		return NULL;
	}

	// Initialize size and factor
	readerPointer->size = size;
	readerPointer->factor = factor;

	// Initialize positions
	readerPointer->position.wrte = 0;
	readerPointer->position.read = 0;
	readerPointer->position.mark = 0;

	// Initialize flags
	readerPointer->flags.isEmpty = EMERALD_TRUE;  // Buffer starts empty
	readerPointer->flags.isFull = EMERALD_FALSE;
	readerPointer->flags.isRead = EMERALD_FALSE;
	readerPointer->flags.isMoved = EMERALD_FALSE;

	// Initialize histogram
	for (emerald_intg i = 0; i < NCHAR; i++) {
		readerPointer->histogram[i] = 0;
	}

	// Initialize error counter and checksum
	readerPointer->numReaderErrors = 0;
	readerPointer->checkSum = 0;

	return readerPointer;
}


/*
***********************************************************
* Function name: readerAddChar
* Purpose: Adds a char to buffer reader
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   ch = char to be added
* Return value:
*	readerPointer (pointer to Buffer Reader)
*************************************************************
*/

BufferPointer readerAddChar(BufferPointer const readerPointer, emerald_char ch) {
	emerald_strg tempReader = NULL;
	emerald_intg newSize = 0;
	if (readerPointer == NULL) {
		printf("No pointer data to add character.");
		return NULL;
	}

	if (ch < READER_ASCII_START || ch > READER_ASCII_END) {
		readerPointer->numReaderErrors++;
		return NULL;
	}

	if (readerPointer->position.wrte < readerPointer->size) {
		readerPointer->flags.isFull = EMERALD_FALSE;
		readerPointer->flags.isEmpty = EMERALD_FALSE;
	}
	else {
		readerPointer->flags.isFull = EMERALD_TRUE;

		emerald_real ratio = 1.0f + readerPointer->factor;
		emerald_intg newSize = (emerald_intg)(readerPointer->size * ratio);

		if (newSize <= 0 || newSize > READER_MAX_SIZE) {
			readerPointer->numReaderErrors++;
			return NULL;
		}

		tempReader = realloc(readerPointer->content, newSize);

		if (tempReader == NULL) {
			readerPointer->numReaderErrors++;
			return NULL;
		}

		if (tempReader != readerPointer->content) {
			readerPointer->flags.isMoved = EMERALD_TRUE;
		}
		readerPointer->content = tempReader;
		readerPointer->size = newSize;
		readerPointer->flags.isFull = EMERALD_FALSE;
	}

	readerPointer->content[readerPointer->position.wrte] = ch;
	readerPointer->position.wrte++;

	if (ch >= READER_ASCII_START && ch <= READER_ASCII_END) {
		readerPointer->histogram[(int)ch]++;
	}
	return readerPointer;
}

/*
***********************************************************
* Function name: readerClear
* Purpose: Clears the buffer reader
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerClear(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		printf("No pointer data to clear.");
			return EMERALD_FALSE;
	}

	readerPointer->position.wrte = 0;
	readerPointer->position.read = 0;
	readerPointer->position.mark = 0;

	readerPointer->flags.isEmpty = EMERALD_TRUE;
	readerPointer->flags.isFull = EMERALD_FALSE;
	readerPointer->flags.isRead = EMERALD_FALSE;
	readerPointer->flags.isMoved = EMERALD_FALSE;
	

	return EMERALD_TRUE;
}

/*
***********************************************************
* Function name: readerFree
* Purpose: Releases the buffer address
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerFree(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	free(readerPointer->content);
	free(readerPointer);

	return EMERALD_TRUE;
}

/*
***********************************************************
* Function name: readerIsFull
* Purpose: Checks if buffer reader is full
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerIsFull(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	return readerPointer->flags.isFull;
}


/*
***********************************************************
* Function name: readerIsEmpty
* Purpose: Checks if buffer reader is empty.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerIsEmpty(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	return readerPointer->flags.isEmpty;
}

/*
***********************************************************
* Function name: readerSetMark
* Purpose: Adjust the position of mark in the buffer
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   mark = mark position for char
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerSetMark(BufferPointer const readerPointer, emerald_intg mark) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	if (!(mark >= 0 && mark <= readerPointer->position.wrte)) {
		return EMERALD_FALSE;
	}
	readerPointer->position.mark = mark;

	return EMERALD_TRUE;
}


/*
***********************************************************
* Function name: readerPrint
* Purpose: Prints the string in the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Number of chars printed.
*************************************************************
*/
emerald_intg readerPrint(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	emerald_intg charCount = 0;
	emerald_intg currentPosition = readerPointer->position.read;

	readerRecover(readerPointer);

	while (!readerPointer->flags.isRead) {
		emerald_char ch = readerGetChar(readerPointer);
		printf("%c", ch);
		charCount++;
	}

	readerPointer->position.read = currentPosition;

	return charCount;
}

/*
***********************************************************
* Function name: readerLoad
* Purpose: Loads the string in the buffer with the content of
	an specific file.
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   fileDescriptor = pointer to file descriptor
* Return value:
*	Number of chars read and put in buffer.
*************************************************************
*/
emerald_intg readerLoad(BufferPointer const readerPointer, emerald_strg fileName) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	if (fileName == NULL) {
		return READER_ERROR;
	}

	emerald_strg decryptedContent = vigenereMem(fileName, STR_LANGNAME, DECYPHER);

	if (decryptedContent == NULL) {
		return READER_ERROR;
	}

	emerald_intg charCount = 0;

	while (decryptedContent[charCount] != '\0') {
		BufferPointer status = readerAddChar(readerPointer, decryptedContent[charCount]);
		if (status == NULL) {
			free(decryptedContent);
			return READER_ERROR;
		}

		charCount++;
	}

	free(decryptedContent);
	return charCount;
}

/*
***********************************************************
* Function name: readerRecover
* Purpose: Rewinds the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerRecover(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	readerPointer->position.mark = 0;
	readerPointer->position.read = 0;

	return EMERALD_TRUE;
}


/*
***********************************************************
* Function name: readerRetract
* Purpose: Retracts the buffer to put back the char in buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerRetract(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	if (readerPointer->position.read <= 0) {
		return EMERALD_FALSE;
	}

	readerPointer->position.read -= 1;

	return EMERALD_TRUE;
}


/*
***********************************************************
* Function name: readerRestore
* Purpose: Resets the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
*************************************************************
*/
emerald_boln readerRestore(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return EMERALD_FALSE;
	}

	readerPointer->position.read = readerPointer->position.mark;

	return EMERALD_TRUE;
}



/*
***********************************************************
* Function name: readerGetChar
* Purpose: Returns the char in the getC position.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Char in the getC position.
*************************************************************
*/
emerald_char readerGetChar(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_TERMINATOR;
	}

	if (readerPointer->position.read >= readerPointer->position.wrte) {
		readerPointer->flags.isRead = EMERALD_TRUE;
		return READER_TERMINATOR;
	}

	readerPointer->flags.isRead = EMERALD_FALSE;

	emerald_char ch = readerPointer->content[readerPointer->position.read];
	readerPointer->position.read++;

	return ch;
}


/*
***********************************************************
* Function name: readerGetContent
* Purpose: Returns the pointer to String.
* Parameters:
*   readerPointer = pointer to Buffer Reader
*   pos = position to get the pointer
* Return value:
*	Position of string char.
*************************************************************
*/
emerald_strg readerGetContent(BufferPointer const readerPointer, emerald_intg pos) {
	if (readerPointer == NULL) {
		return NULL;
	}

	if (!(pos >= 0 && pos < readerPointer->position.wrte)) {
		return NULL;
	}

	return &(readerPointer->content[pos]);
}

/*
***********************************************************
* Function name: readerGetPosRead
* Purpose: Returns the value of getCPosition.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	The read position offset.
*************************************************************
*/
emerald_intg readerGetPosRead(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	return readerPointer->position.read;
}


/*
***********************************************************
* Function name: readerGetPosWrte
* Purpose: Returns the position of char to be added
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Write position
*************************************************************
*/
emerald_intg readerGetPosWrte(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	return readerPointer->position.wrte;
}


/*
***********************************************************
* Function name: readerGetPosMark
* Purpose: Returns the position of mark in the buffer
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Mark position.
*************************************************************
*/
emerald_intg readerGetPosMark(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	return readerPointer->position.mark;
}


/*
***********************************************************
* Function name: readerGetSize
* Purpose: Returns the current buffer capacity
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Size of buffer.
*************************************************************
*/
emerald_intg readerGetSize(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	return readerPointer->size;
}

/*
***********************************************************
* Function name: readerGetFlags
* Purpose: Returns the entire flags of Buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Flags from Buffer.
*************************************************************
*/
#define FLAGS_
#undef FLAGS_
#ifndef FLAGS_
emerald_void readerPrintFlags(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return;
	}

	printf("Flag.isEmpty = %d\n", readerPointer->flags.isEmpty);
	printf("Flag.isFull = %d\n", readerPointer->flags.isFull);
	printf("Flag.isMoved = %d\n", readerPointer->flags.isMoved);
	printf("Flag.isRead = %d\n", readerPointer->flags.isRead);
}
#else
#define bGetFlags(readerPointer) ((readerPointer)?(readerPointer->flags):(RT_FAIL_1))
#endif

/*
***********************************************************
* Function name: readerShowStat
* Purpose: Shows the char statistic.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value: (Void)
*************************************************************
*/
emerald_void readerPrintStat(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return;
	}

	emerald_intg i = 0;
	for (i; i < NCHAR - 1; i++) {
		if (readerPointer->histogram[i] > 0) {
			printf("B[%c]=%d, ", (char)i, readerPointer->histogram[i]);
		}
	}
}

/*
***********************************************************
* Function name: readerNumErrors
* Purpose: Returns the number of errors found.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Number of errors.
*************************************************************
*/
emerald_intg readerNumErrors(BufferPointer const readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	return readerPointer->numReaderErrors;
}

/*
***********************************************************
* Function name: readerChecksum
* Purpose: Sets the checksum of the reader (4 bits).
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	[None]
*************************************************************
*/

emerald_intg readerChecksum(BufferPointer readerPointer) {
	if (readerPointer == NULL) {
		return READER_ERROR;
	}

	emerald_intg sum = 0;
	emerald_intg i = 0;

	for (i; i < readerPointer->position.wrte; i++) {
		sum += readerPointer->content[i];
	}

	emerald_intg checkSum = sum & 0xFF;
	readerPointer->checkSum = checkSum;

	return checkSum;
}
