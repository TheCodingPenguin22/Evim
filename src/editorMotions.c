#include "editorMotions.h"
#include "data.h"
#include "editor_operations.h"
#include "output.h"
#include <stdlib.h>
#include <string.h>

void appendToVimMotionBuffer(char *c) {
  if (E.mBuffer.buffer == NULL) {
    E.mBuffer.buffer = malloc(sizeof(char));
  }

  E.mBuffer.buffer =
      realloc(E.mBuffer.buffer, sizeof(char) * E.mBuffer.bufferSize++);
  E.mBuffer.buffer[E.mBuffer.bufferSize] = *c;
}
void vimMotionO() {
  editorInsertNewline();
  E.currentMode = INSERT_MODE;
}

void vimMotionDD() {
  free(E.row[E.cy].chars);

  memmove(&E.row[E.cy], &E.row[E.cy + 1],
          sizeof(erow) * (E.numrows - E.cy - 1));
  editorRefreshScreen();
}
