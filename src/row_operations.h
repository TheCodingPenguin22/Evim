#ifndef ROW_OPERATIONS_H
#define ROW_OPERATIONS_H

#include "data.h"

void editorUpdateRow(erow *row);
void editorRowInsertChar(erow* row, int at, int c);
int editorRowCxToRx(erow* row, int cx);
void editorAppendRow(char* s, size_t len);
void editorInsertChar(int c);

#endif