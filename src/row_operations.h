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
void editorRowAppendString(erow *row, char *s, size_t len);
int editorCheckIfRowIsBlank(erow *row, size_t len);

#endif
