#include "editorMotions.h"
#include "data.h"
#include "editor_operations.h"
#include "output.h"
#include "row_operations.h"
#include <ctype.h>
#include <stdlib.h>

void resetVimMotionBuffer() {
  E.mBuffer.buffer[0] = '\0';
  E.mBuffer.bufferSize = 0;
  E.isDeleting = 0;
}
/*
 * This is a function for processing the motion buffer.
 * Really simple since it is only parsing one char at a time.
 *
 * If the char that is in the buffer is not a char then it just clears the
 * buffer, this is to prevent a situation where a user can enter 'bdd' for
 * example. Then the 'dd' command whould not execute.
 *
 * TODO: This should really be refactored at some point to make clearer if
 * statements.
 */
int processVimMotionBuffer() {
  int hasExecutedMotion = 0;
  if (E.mBuffer.bufferSize == 1) {
    switch (E.mBuffer.buffer[0]) {
    case 'o':
      vimMotiono();
      hasExecutedMotion = 1;
      break;
    case 'w':
      vimMotionw(E.cx);
      hasExecutedMotion = 1;
      break;
    case 'b':
      vimMotionb(E.cx);
      hasExecutedMotion = 1;
      break;
    case 'd':
      E.isDeleting = 1;
      break;
    default:
      resetVimMotionBuffer();
    }
  } else if (E.mBuffer.bufferSize == 2) {
    if (E.mBuffer.buffer[0] == 'd') {
      if (E.mBuffer.buffer[1] == 'd') {
        vimMotiondd(E.cy);
        hasExecutedMotion = 1;
      } else {
        resetVimMotionBuffer();
      }
    } else {
      resetVimMotionBuffer();
    }
  }
  return hasExecutedMotion;
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
 * If there are no rows just enter insert mode.
 *
 */
void vimMotiono() {

  if (E.row != NULL) {
    E.cx = E.row[E.cy].size;
    editorInsertNewline();
  }

  E.currentMode = INSERT_MODE;
}
/*
 * This function is for the vim motion 'dd' which deletes the current line.
 *
 *
 */
void vimMotiondd(int at) {
  editorDelRow(at);
  E.cx = 0;
  if (E.cy > 0)
    E.cy--;
  editorRefreshScreen();
}

void vimMotionw(int at) {

  if (E.row != NULL) {
    char *posPtr = &E.row[E.cy].chars[at];
    char lastCharOfRow = E.row[E.cy].chars[E.row[E.cy].size];
    /*
     * TODO: Fix seg fault when pressing w at last char of row
     */
    if (*posPtr + 1 != ' ' && *posPtr != lastCharOfRow) {
      at++;
      posPtr++;
    }
    if (*posPtr == lastCharOfRow && E.cy < E.numrows - 1) {
      E.cy++;
      at = 0;
      posPtr = &E.row[E.cy].chars[at];
    }
    while (posPtr != &E.row[E.cy].chars[E.row[E.cy].size - 1] &&
           !iscntrl(*posPtr)) {
      if (*posPtr == ' ') {
        E.cx = at;
        return;
      }
      posPtr++;
      at++;
    }
    E.cx = E.row[E.cy].rsize;
  }
}

void vimMotionb(int at) {
  if (E.row != NULL) {

    char *posPtr = &E.row[E.cy].chars[at];

    if (*posPtr == ' ' && *posPtr != 0) {
      at--;
      posPtr--;
    } else if (*(posPtr - 1) == ' ' && *(posPtr - 2) > 0) {
      at--;
      posPtr--;
    }
    if (at == 0 && E.cy > 0) {
      E.cy--;
      at = E.row[E.cy].size;
      E.cx = at;
    } else {

      while (posPtr != &E.row[E.cy].chars[0]) {
        if (*(posPtr - 1) == ' ') {
          E.cx = at;
          return;
        }
        posPtr--;
        at--;
      }

      if (at == 0) {
        E.cx = at;
      }
    }
  }
}
