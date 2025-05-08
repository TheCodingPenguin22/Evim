#include "editor_operations.h"

#include "data.h"
#include "row_operations.h"

void editorInsertChar(int c){
    if (E.cy == E.numrows) {
      editorAppendRow("", 0);
    }
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    E.cx++;
  }