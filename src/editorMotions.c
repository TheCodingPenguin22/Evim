#include "editorMotions.h"
#include "data.h"
#include "editor_operations.h"
#include "output.h"
#include "row_operations.h"
#include <stdlib.h>

void resetVimMotionBuffer() {
  E.mBuffer.buffer[0] = '\0';
  E.mBuffer.bufferSize = 0;
}
/*
 * This is a function for processing the motion buffer.
 * Really simple since it is only parsing one char at a time
 *
 * TODO: This should really be refactored at some point to make clearer if
 * statements.
 */
void processVimMotionBuffer() {
  if (E.mBuffer.bufferSize == 1) {
    if (E.mBuffer.buffer[0] == 'o') {
      vimMotionO();
    }
  }
  if (E.mBuffer.bufferSize == 2) {
    if (E.mBuffer.buffer[0] == 'd') {
      if (E.mBuffer.buffer[1] == 'd') {
        vimMotionDD(E.cy);
      }
    }
  }
  editorRefreshScreen();
}
/*
 * This is a function for freeing and resetting the buffer.
 *
 * This gets called after every successfull vim motion execution
 * but I don't know if that is nessecary.
 */
void freeVimMotionBuffer() {
  free(E.mBuffer.buffer);
  E.mBuffer.buffer = malloc(E.mBuffer.maxBufferSize);
  E.mBuffer.buffer[0] = '\0';
  E.mBuffer.bufferSize = 0;
}
/*
 * This is a function for appeding to the motion buffer.
 *
 */
void appendToVimMotionBuffer(char c) {

  if (E.mBuffer.bufferSize == E.mBuffer.maxBufferSize - 1) {
    E.mBuffer.maxBufferSize *= 2;
    E.mBuffer.buffer = realloc(E.mBuffer.buffer, E.mBuffer.maxBufferSize);
  }

  E.mBuffer.buffer[E.mBuffer.bufferSize++] = c;
  E.mBuffer.buffer[E.mBuffer.bufferSize] = '\0';
  /*
   * This is a debug message for the motion buffer.
   editorSetStatusMessage("E.mBuffer.bufferSize: %d, E.mBuffer.buffer: %s",
                         E.mBuffer.bufferSize, E.mBuffer.buffer);
  */
}

/*
 * This is a function for the vim motion 'o'.
 * That motion simply inserts a new line after the current line and sets the
 * cursor to that line.
 */
void vimMotionO() {
  editorInsertNewline();
  E.currentMode = INSERT_MODE;
  resetVimMotionBuffer();
}
/*
 * This function is for the vim motion 'dd' which deletes the current line.
 *
 *
 */
void vimMotionDD(int at) {
  editorDelRow(at);
  E.cx = 0;
  resetVimMotionBuffer();
  editorRefreshScreen();
}
