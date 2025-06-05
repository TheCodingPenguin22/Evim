#ifndef ROW_OPERATIONS_H
#define ROW_OPERATIONS_H

#include "data.h"

void editorUpdateRow(erow *row);
void editorRowInsertChar(erow *row, int at, int c);
int editorRowCxToRx(erow *row, int cx);
void editorInsertRow(int at, char *s, size_t len);
void editorInsertChar(int c);
void editorRowDelChar(erow *row, int at);
void editorFreeRow(erow *row);
void editorDelRow(int at);

/*
 * This function appends a string to the end of a given row.
 *
 *  :param erow *row -- Pointer to the current row.
 *  :param char *s -- The string that is getting appended.
 *  :param size_t len -- The length of the current row.
 *
 */
void editorRowAppendString(erow *row, char *s, size_t len);

/*
 * This function checks if a given row is blank.
 *
 *  :param erow *row -- A pointer to the current row.
 *  :param size_t len -- The length of the current row.
 *
 */
int editorCheckIfRowIsBlank(erow *row, size_t len);

/*
 * This function checks if the cursor is at the end of the row
 * and if it is and the current row + 1 does not exceed the current
 * amount of rows that exists then it increments the y value and sets the
 * current at to 0;
 *
 *  :param char posPtr -- the current position of the pointer that is beging
 * incremented.
 * :param char *lastCharOfRow -- The last character in the current
 * row.
 * :param int *at -- the current cursor position.
 *
 */
void editorGoToNextRowIfAtEnd(char **posPtr, char *lastCharOfRow, int *at);

#endif
