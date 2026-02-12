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
* TODO ......................................................
*	- Adjust datatypes for your LANGUAGE.
*   - Use defensive programming
*	- Check boundary conditions
*	- Check flags.
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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/

BufferPointer readerAddChar(BufferPointer const readerPointer, emerald_char ch) {
	emerald_strg tempReader = NULL;
	emerald_intg newSize = 0;
	/* TO_DO: Check if readerPointer is NULL, if so return NULL */
	if (readerPointer == NULL) {
		printf("No pointer data to add character.");
		return NULL;
	}
	/* TO_DO: Check if ch is a valid ASCII character (0-127), if not increment numReaderErrors and return NULL */
	if (ch < ASCII_START || ch > ASCII_END) {
		readerPointer->numReaderErrors++;
		return NULL;
	}

	/* TO_DO: Check if there's space in buffer: compare (position.wrte < size) */
	if (readerPointer->position.wrte < readerPointer->size) {
		/* TO_DO: There IS space - set flags.isFull to EMERALD_FALSE */
		readerPointer->flags.isFull = EMERALD_FALSE;
		/* TO_DO: Set flags.isEmpty to EMERALD_FALSE (buffer now has content) */
		readerPointer->flags.isEmpty = EMERALD_FALSE;
	}
	else {
		/* TO_DO: Buffer is full - need to grow it */
		/* TO_DO: Set flags.isFull to EMERALD_TRUE */
		readerPointer->flags.isFull = EMERALD_TRUE;
		/* TO_DO: Calculate newSize using formula: newSize = (emerald_intg)(readerPointer->size * (1.0 + readerPointer->factor)) */
		emerald_intg newSize = (readerPointer->size * READER_DEFAULT_FACTOR) + readerPointer->size;
		/* TO_DO: Check if newSize is valid: must be positive and <= READER_MAX_SIZE, if not return NULL */
		if (newSize <= 0 || newSize > READER_MAX_SIZE) {
			readerPointer->numReaderErrors++;
			return NULL;
		}
		/* TO_DO: Try to reallocate memory: tempReader = (emerald_strg)realloc(readerPointer->content, newSize) */
		tempReader = realloc(readerPointer->content, newSize);
		/* TO_DO: Check if realloc succeeded (tempReader != NULL), if failed return NULL */
		if (tempReader == NULL) {
			readerPointer->numReaderErrors++;
			return NULL;
		}
		/* TO_DO: Check if memory address changed: compare tempReader with readerPointer->content */
		/* TO_DO: If address changed, set flags.isMoved to EMERALD_TRUE */
		/* TO_DO: Update readerPointer->content = tempReader */
		/* TO_DO: Update readerPointer->size = newSize */
		/* TO_DO: Set flags.isFull to EMERALD_FALSE (we just made more space) */
		if (tempReader != readerPointer->content) {
			readerPointer->flags.isMoved = EMERALD_TRUE;
		}
		readerPointer->content = tempReader;
		readerPointer->size = newSize;
		readerPointer->flags.isFull = EMERALD_FALSE;
	}

	/* TO_DO: Add the character: readerPointer->content[readerPointer->position.wrte] = ch */
	readerPointer->content[readerPointer->position.wrte] = ch;
	/* TO_DO: Increment write position: readerPointer->position.wrte++ */
	readerPointer->position.wrte++;
	/* TO_DO: Update histogram for this character: readerPointer->histogram[(int)ch]++ */
	/* TO_DO: Note: only update histogram if ch is in valid range (0-127) */
	if (ch >= ASCII_START && ch <= ASCII_END) {
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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerClear(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */
	if (readerPointer == NULL) {
		printf("No pointer data to clear.");
			return EMERALD_FALSE;
	}
	/* TO_DO: Reset all positions to zero:
	   - position.wrte = 0
	   - position.read = 0
	   - position.mark = 0 */
	readerPointer->position.wrte = 0;
	readerPointer->position.read = 0;
	readerPointer->position.mark = 0;
	   /* TO_DO: Reset flags to initial state:
		  - flags.isEmpty = EMERALD_TRUE
		  - flags.isFull = EMERALD_FALSE
		  - flags.isRead = EMERALD_FALSE
		  - flags.isMoved = EMERALD_FALSE */
	readerPointer->flags.isEmpty = EMERALD_TRUE;
	readerPointer->flags.isFull = EMERALD_FALSE;
	readerPointer->flags.isRead = EMERALD_FALSE;
	readerPointer->flags.isMoved = EMERALD_FALSE;
	

	return EMERALD_TRUE;  /* TO_DO: Change to EMERALD_TRUE on success */
}

/*
***********************************************************
* Function name: readerFree
* Purpose: Releases the buffer address
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerIsFull(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */

	/* TO_DO: Return the value of flags.isFull */

	return EMERALD_FALSE;
}


/*
***********************************************************
* Function name: readerIsEmpty
* Purpose: Checks if buffer reader is empty.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerIsEmpty(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */

	/* TO_DO: Return the value of flags.isEmpty */

	return EMERALD_FALSE;
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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerSetMark(BufferPointer const readerPointer, emerald_intg mark) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */

	/* TO_DO: Check if mark is valid: must be >= 0 and <= position.wrte */
	/* TO_DO: If mark is invalid, return EMERALD_FALSE */

	/* TO_DO: Set position.mark = mark */

	return EMERALD_FALSE;  /* TO_DO: Change to EMERALD_TRUE on success */
}


/*
***********************************************************
* Function name: readerPrint
* Purpose: Prints the string in the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Number of chars printed.
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerPrint(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Create a counter for number of chars printed */
	/* TO_DO: Save current position.read value */
	/* TO_DO: Reset position.read to 0 using readerRecover() */

	/* TO_DO: Loop while flags.isRead is FALSE:
	   - Call readerGetChar() to get next character
	   - Print the character using printf("%c", ch)
	   - Increment counter */

	   /* TO_DO: Restore position.read to saved value */
	   /* TO_DO: Return the counter (number of chars printed) */

	return 0;
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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerLoad(BufferPointer const readerPointer, emerald_strg fileName) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR */
	/* TO_DO: Check if fileName is NULL, if so return READER_ERROR */

	/* TO_DO: Call vigenereMem(fileName, STR_LANGNAME, DECYPHER) to decrypt file */
	/* TO_DO: Store result in a string variable (decryptedContent) */
	/* TO_DO: Check if decryptedContent is NULL, if so return READER_ERROR */

	/* TO_DO: Create counter for chars loaded */
	/* TO_DO: Loop through each character in decryptedContent:
	   - Call readerAddChar() to add each character
	   - Check if readerAddChar() returns NULL (error occurred)
	   - If error, free decryptedContent and return READER_ERROR
	   - Increment counter */

	   /* TO_DO: Free the decryptedContent memory */
	   /* TO_DO: Return counter (number of chars loaded) */

	return 0;
}

/*
***********************************************************
* Function name: readerRecover
* Purpose: Rewinds the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value
*	Boolean value about operation success
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerRecover(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */

	/* TO_DO: Set position.read = 0 */
	/* TO_DO: Set position.mark = 0 */

	return EMERALD_FALSE;  /* TO_DO: Change to EMERALD_TRUE on success */
}


/*
***********************************************************
* Function name: readerRetract
* Purpose: Retracts the buffer to put back the char in buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerRetract(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */

	/* TO_DO: Check if position.read > 0 (can't retract if at beginning) */
	/* TO_DO: If position.read is 0, return EMERALD_FALSE */

	/* TO_DO: Decrement position.read by 1: position.read-- */

	return EMERALD_FALSE;  /* TO_DO: Change to EMERALD_TRUE on success */
}


/*
***********************************************************
* Function name: readerRestore
* Purpose: Resets the buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Boolean value about operation success
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_boln readerRestore(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return EMERALD_FALSE */

	/* TO_DO: Set position.read = position.mark (restore to marked position) */

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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_char readerGetChar(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_TERMINATOR ('\0') */

	/* TO_DO: Check if position.read >= position.wrte (reached end of content) */
	/* TO_DO: If at end, set flags.isRead = EMERALD_TRUE and return READER_TERMINATOR */

	/* TO_DO: Set flags.isRead = EMERALD_FALSE (not at end) */

	/* TO_DO: Get character at current read position: ch = content[position.read] */
	/* TO_DO: Increment position.read++ */
	/* TO_DO: Return the character */

	return '\0';
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
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_strg readerGetContent(BufferPointer const readerPointer, emerald_intg pos) {
	/* TO_DO: Check if readerPointer is NULL, if so return NULL */

	/* TO_DO: Check if pos is valid: must be >= 0 and < position.wrte */
	/* TO_DO: If pos is invalid, return NULL */

	/* TO_DO: Return pointer to content starting at position: return &(readerPointer->content[pos]) */

	return NULL;
}

/*
***********************************************************
* Function name: readerGetPosRead
* Purpose: Returns the value of getCPosition.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	The read position offset.
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerGetPosRead(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Return position.read */

	return 0;
}


/*
***********************************************************
* Function name: readerGetPosWrte
* Purpose: Returns the position of char to be added
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Write position
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerGetPosWrte(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Return position.wrte */

	return 0;
}


/*
***********************************************************
* Function name: readerGetPosMark
* Purpose: Returns the position of mark in the buffer
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Mark position.
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerGetPosMark(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Return position.mark */

	return 0;
}


/*
***********************************************************
* Function name: readerGetSize
* Purpose: Returns the current buffer capacity
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Size of buffer.
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerGetSize(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Return size */

	return 0;
}

/*
***********************************************************
* Function name: readerGetFlags
* Purpose: Returns the entire flags of Buffer.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Flags from Buffer.
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/
#define FLAGS_
#undef FLAGS_
#ifndef FLAGS_
emerald_void readerPrintFlags(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return immediately */
	if (!readerPointer)
		return;

	/* TO_DO: Print all four flags with labels:
	   printf("Flag.isEmpty = %d\n", readerPointer->flags.isEmpty);
	   printf("Flag.isFull = %d\n", readerPointer->flags.isFull);
	   printf("Flag.isMoved = %d\n", readerPointer->flags.isMoved);
	   printf("Flag.isRead = %d\n", readerPointer->flags.isRead); */
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
* TO_DO:
*   - Use defensive programming
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_void readerPrintStat(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return immediately */

	/* TO_DO: Loop through histogram array (0 to NCHAR-1):
	   - If histogram[i] > 0, print: "B[char]=%d, " where char is (char)i
	   - This shows which characters were found and how many times
	   - Example output: B[a]=5, B[b]=3, B[ ]=10, */
}

/*
***********************************************************
* Function name: readerNumErrors
* Purpose: Returns the number of errors found.
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	Number of errors.
* TO_DO:
*   - Use defensive programming
*	- Adjust for your LANGUAGE.
*************************************************************
*/
emerald_intg readerNumErrors(BufferPointer const readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Return numReaderErrors */

	return 0;
}

/*
***********************************************************
* Function name: readerChecksum
* Purpose: Sets the checksum of the reader (4 bits).
* Parameters:
*   readerPointer = pointer to Buffer Reader
* Return value:
*	[None]
* TO_DO:
*   - Use defensive programming
*	- Check boundary conditions
*	- Adjust for your LANGUAGE.
*************************************************************
*/

emerald_intg readerChecksum(BufferPointer readerPointer) {
	/* TO_DO: Check if readerPointer is NULL, if so return READER_ERROR (-1) */

	/* TO_DO: Create variable to hold sum: emerald_intg sum = 0 */

	/* TO_DO: Loop through all characters in content (from 0 to position.wrte):
	   - Add each character's ASCII value to sum
	   - sum += (int)content[i] */

	   /* TO_DO: Calculate checksum using 8 bits: checkSum = sum & 0xFF */
	   /* TO_DO: Store result in readerPointer->checkSum */
	   /* TO_DO: Return the checkSum value */

	return 0;
}
