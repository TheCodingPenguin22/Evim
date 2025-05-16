#include "editorMotions.h"
#include "data.h"
#include "editor_operations.h"
#include "output.h"
#include "row_operations.h"
#include <stdlib.h>

void processVimMotionBuffer() {
  if (E.mBuffer.bufferSize == 2) {
    if (E.mBuffer.buffer[0] == 'd') {
      if (E.mBuffer.buffer[1] == 'd') {
        vimMotionDD(E.cy);
      }
    }
  }
  editorRefreshScreen();
}

void freeVimMotionBuffer() {
  free(E.mBuffer.buffer);
  E.mBuffer.buffer = malloc(E.mBuffer.maxBufferSize);
  E.mBuffer.buffer[0] = '\0';
  E.mBuffer.bufferSize = 0;
}

void appendToVimMotionBuffer(char c) {

  if (E.mBuffer.bufferSize == E.mBuffer.maxBufferSize - 1) {
    E.mBuffer.maxBufferSize *= 2;
    E.mBuffer.buffer = realloc(E.mBuffer.buffer, E.mBuffer.maxBufferSize);
  }

  E.mBuffer.buffer[E.mBuffer.bufferSize++] = c;
  E.mBuffer.buffer[E.mBuffer.bufferSize] = '\0';

  editorSetStatusMessage("E.mBuffer.bufferSize: %d, E.mBuffer.buffer: %s",
                         E.mBuffer.bufferSize, E.mBuffer.buffer);
  // editorSetStatusMessage(E.mBuffer.buffer);
}
void vimMotionO() {
  editorInsertNewline();
  E.currentMode = INSERT_MODE;
}

void vimMotionDD(int at) {
  editorDelRow(at);
  E.cx = 0;
  E.mBuffer.buffer[0] = '\0';
  E.mBuffer.bufferSize = 0;
  editorRefreshScreen();
}
