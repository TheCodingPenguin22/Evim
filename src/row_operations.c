#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "row_operations.h"

// Creates a new row with the appropriate amount of space instead of tabs('\t').
void editorUpdateRow(erow* row) {
  int tabs = 0;
  int j;
  for(j = 0; j < row->size; j++) {
    if(row->chars[j] == '\t') tabs++;
  }

  // Allocates the space.
  free(row->render);
  row->render = malloc(row->size + tabs*(EVIM_TAB_STOP - 1) + 1);

  // Adds the spaces if a tabs is found.
  // Otherwise it just adds the current char.
  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if(row->chars[j] == '\t'){
      row->render[idx++] = ' ';
      while (idx % EVIM_TAB_STOP != 0) row->render[idx++] = ' ';
    }
    else {
      row->render[idx++] = row->chars[j];
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;
}
  
// Inserts a given character at the given row
void editorRowInsertChar(erow* row, int at, int c) {
  // Makes sure at is within the size of the row.
  if ( at < 0 || at > row->size) at = row->size;

  // Makes the chars array two bytes bigger. A char is two bytes big.
  row->chars = realloc(row->chars, row->size + 2);

  // Copies row->size - at + 1 bytes starting from row->chars[at] to row->chars[at + 1]
  // Basically it creates space for the new character to be inserted.
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  // Increases the size of the row.
  row->size++;

  // Insers the character.
  row->chars[at] = c;

  // Updates the row.
  editorUpdateRow(row);
  E.dirty++;
}


// This converts Cx to Rx, meaning it it takes the current cursor posistion and and any tabs('\t') to it.
// The size of a tab is currently defined by EVIM_TAB_STOP
int editorRowCxToRx(erow* row, int cx){
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++){
    if(row->chars[j] == '\t'){
      rx += (EVIM_TAB_STOP - 1) - (rx % EVIM_TAB_STOP);
    }
    rx++;
  }
  return rx;
}



// Adds a new row to the editorconfig struct.
void editorAppendRow(char *s, size_t len) {
  E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));

  int at = E.numrows;
  E.row[at].size = len;
  E.row[at].chars = malloc(len + 1);
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';

  E.row[at].rsize = 0;
  E.row[at].render = NULL;
  editorUpdateRow(&E.row[at]);

  E.numrows++;
  E.dirty++;
}


void editorRowDelChar(erow* row, int at){
  if (at < 0 || at >= row->size) return;
  // Moves the array one index to the left.
  // chars[at] becomes chars[at + 1 ] and the rest move with it.
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  E.dirty++;
}
