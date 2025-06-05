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
    case 'e':
      vimMotione(E.cx);
      hasExecutedMotion = 1;
      break;
    case 'I':
      vimMotionI();
      hasExecutedMotion = 1;
      break;
    case 'A':
      vimMotionA();
      hasExecutedMotion = 1;
      break;
    case 'G':
      vimMotionG();
      hasExecutedMotion = 1;
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
}

// Sets the cursor to the empty space after the current word.
// :param int at -- The current E pos of the cursor.
void vimMotionw(int at) {
  // Returns if the cursor is on a row that does not exist
  // or if the whole row struct is NULL.
  if (E.cy >= E.numrows || E.row == NULL) {
    return;
  }

  // Sets the pointer to the current char the cursor is at.
  char *posPtr = &E.row[E.cy].chars[at];

  // Gets the last variable of the row.
  // Makes checking against it easier.
  char lastCharOfRow = E.row[E.cy].chars[E.row[E.cy].size];

  // While the cursor is at a blank space or a controll character.
  // This puts the cursor at the start of the next word.
  while (*posPtr == ' ' || iscntrl(*posPtr)) {

    // Goes to the next row if posPtr is at the end.
    editorGoToNextRowIfAtEnd(*posPtr, &lastCharOfRow, &at);

    // Sets at to 0 if the row is empty.
    // It also returns, because if the
    // row is empty then there is no next word to go to.
    if (editorCheckIfRowIsBlank(&E.row[E.cy], E.row[E.cy].size)) {
      at = 0;
    }

    // If the posPtr is at the end of the row,
    // then is just breaks.
    if (*posPtr == lastCharOfRow) {
      break;
    }

    at++;
    posPtr++;
  }

  // While the posPtr is not at the end of the current row.
  while (posPtr != &E.row[E.cy].chars[E.row[E.cy].size]) {
    // Stops if the posPtr is a blank space.
    // This is what makes the motion work.
    if (*posPtr == ' ' || iscntrl(*posPtr)) {
      break;
    }
    posPtr++;
    at++;
  }
  // Sets the cursor position to the current at.
  E.cx = at;

  // If the row is blank then it sets x to 0;
  // This is to prevent a situation where the cursor is beyond the end of a
  // row.
}

// Sets the cursor to the last char of the current word.
void vimMotione(int at) {
  if (E.row != NULL) {
    char *posPtr = &E.row[E.cy].chars[at];
    char lastCharOfRow = E.row[E.cy].chars[E.row[E.cy].size];

    while (*(posPtr + 1) == ' ' || iscntrl(*(posPtr + 1))) {
      editorGoToNextRowIfAtEnd(*posPtr, &lastCharOfRow, &at);
      if (editorCheckIfRowIsBlank(&E.row[E.cy], E.row[E.cy].size)) {
        at = 0;
      }

      if (*posPtr == lastCharOfRow)
        break;
      at++;
      posPtr++;
    }
    while (posPtr != &E.row[E.cy].chars[E.row[E.cy].size]) {
      if (*(posPtr + 1) == ' ' || iscntrl(*(posPtr + 1))) {
        E.cx = at;
        return;
      }
      at++;
      posPtr++;
    }
  }
}

// Sets the cursor to the start of the word that comes before the current
// word.
void vimMotionb(int at) {
  if (E.row != NULL) {

    char *posPtr = &E.row[E.cy].chars[at];

    if (at - 1 < 0 && E.cy > 0) {
      E.cy--;
      at = E.row[E.cy].size;
      posPtr = &E.row[E.cy].chars[at];
    }

    if (*(posPtr - 1) == ' ' || iscntrl(*(posPtr - 1))) {
      at--;
      posPtr--;
      while (*(posPtr - 1) == ' ' || iscntrl(*(posPtr - 1))) {
        if (at == 0) {
          return;
        }
        posPtr--;
        at--;
      }
    }

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

// Sets the cursor at the first char in the current row.
void vimMotionI() {
  if (E.row != NULL) {
    if (!editorCheckIfRowIsBlank(&E.row[E.cy], E.row[E.cy].size)) {

      int at = 0;
      char *posPtr = &E.row[E.cy].chars[at];

      while (*posPtr == ' ') {
        posPtr++;
        at++;
      }

      E.cx = at;
    }
  }
  E.currentMode = INSERT_MODE;
}

// Sets the cursor at the last char in the current row.
void vimMotionA() {
  if (E.row != NULL) {
    if (!editorCheckIfRowIsBlank(&E.row[E.cy], E.row[E.cy].size)) {

      int at = E.row[E.cy].size;
      char *posPtr = &E.row[E.cy].chars[at];

      while (*posPtr == ' ') {
        at--;
        posPtr--;
      }
      E.cx = at;
    }
  }

  E.currentMode = INSERT_MODE;
}

// Sets the cursor to the end of the file
void vimMotionG() {
  if (E.row != NULL) {
    E.cy = E.numrows - 1;
  }
}
