#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "data.h"
#include "editorMotions.h"
#include "editor_operations.h"
#include "file_io.h"
#include "input.h"
#include "output.h"
#include "row_operations.h"
#include "terminal.h"

// This function is responsible for moving the cursor.
void editorMoveCursor(int key) {
  // Sets the current row.
  // If the current cursor y pos is greater than the number of rows then it sets
  // the row to NULL.
  erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

  // Switch case for checking where to move the cursor
  switch (key) {
  case ARROW_LEFT:
    if (E.cx != 0) {
      E.cx--;
    }
    // Moves the cursor to the end of the line above if the user pressed left at
    // the start of a line.
    else if (E.cy > 0) {
      E.cy--;
      E.cx = E.row[E.cy].size;
    }
    break;
  case ARROW_RIGHT:
    if (row && E.cx < row->size) {
      E.cx++;
    }
    // Sets the cursor to the start of the line below if one exists.
    else if (row && E.cx == row->size) {
      E.cy++;
      E.cx = 0;
    }
    break;
  case ARROW_UP:
    if (E.cy != 0) {
      E.cy--;
    }
    break;
  case ARROW_DOWN:
    if (E.cy < E.numrows) {
      E.cy++;
    }
    break;
  }

  // If the cursor is oast the end of the file set it to NULL otherwise the its
  // row to the current row.
  row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

  // If row is NULL then set rowlen to 0 otherwise set it to the size of the
  // current row.
  int rowlen = row ? row->size : 0;

  // If the x pos of the cursor if beyond the rowlen then it sets the x pos to
  // the current rows len.
  if (E.cx > rowlen) {
    E.cx = rowlen;
  }
}

// Waits for editorReadKey() then it handles the keypresses that the function
// returns.
void editorProcessKeypressNormalMode() {
  if (E.mBuffer.bufferSize) {
    if (processVimMotionBuffer() >= 1) {
      resetVimMotionBuffer();
      return;
    }
  }
  editorRefreshScreen();

  int c = editorReadKey();

  editorSetStatusMessage("char key: %d", c);
  switch (c) {
  // Goes into command mode when pressing ':'
  case 1000:
    editorMoveCursor(ARROW_LEFT);
    break;
  case 1003:
    editorMoveCursor(ARROW_DOWN);
    break;
  case 1002:
    editorMoveCursor(ARROW_UP);
    break;
  case 1001:
    editorMoveCursor(ARROW_RIGHT);
    break;
  case 58:
    E.currentMode = COMMAND_MODE;
    break;
  // Goes into insert mode
  // 105 == i
  case 105:
    E.currentMode = INSERT_MODE;
    break;
  // Also goes into insert mode but sets the cursor behind the current char.
  // 97 == a
  case 97:
    if (E.cx != E.row[E.cy].size) {
      editorMoveCursor(ARROW_RIGHT);
    }
    E.currentMode = INSERT_MODE;
    break;
  // Saves the file
  case CTRL_KEY('s'):
    editorSave();
    break;
  // Goes to start of line if HOME key is pressed.
  case HOME_KEY:
    E.cx = 0;
    break;

  // Goes to end of line if END key is pressed.
  case END_KEY:
    if (E.cy < E.numrows) {
      E.cx = E.row[E.cy].size;
    }
    break;

  // Goes to top of file if PAGE_UP is pressed
  // and bottom of file if PAGE_DOWN is pressed.
  case PAGE_UP:
  case PAGE_DOWN: {
    if (c == PAGE_UP) {
      E.cy = E.rowoff;
    } else if (c == PAGE_DOWN) {
      E.cy = E.rowoff + E.screenrows - 1;
      if (E.cy > E.numrows)
        E.cy = E.numrows;
    }

    int times = E.screenrows;
    while (times--) {
      editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
  } break;

  // Frees the motion input buffer if the escape key is pressed.
  case CTRL_KEY('l'):
  case '\x1b':
    freeVimMotionBuffer();
    break;
  default:
    appendToVimMotionBuffer(c);
    break;
  }
}

// Handels key input when in insert mode.
void editorProcessKeypressInserMode() {
  editorRefreshScreen();
  int c = editorReadKey();
  switch (c) {
  // Delete chars
  case BACKSPACE:
  case CTRL_KEY('h'):
  case DEL_KEY:
    if (c == DEL_KEY)
      editorMoveCursor(ARROW_RIGHT);
    editorDelChar();
    break;
  // Go back to normal mode
  case CTRL_KEY('l'):
  case '\x1b':
    E.currentMode = NORMAL_MODE;
    editorRefreshScreen();
    break;
  // Insert a new line when pressing enter
  case '\r':
    editorInsertNewline();
    break;
  // Insert char.
  default:
    editorInsertChar(c);
    break;
  }
}

// Handels key input when in command mode.

// There is not a lot to handle, it just appends to the command buffer,
// exits if user presses esc and removes the char before the cursor when
// pressed backspace.
void editorProcessKeypressCommandMode() {
  char *command = editorPrompt(":%s");
  if (command == NULL) {
    E.currentMode = NORMAL_MODE;
    return;
  }
  int commandlen = strlen(command);

  if (commandlen == 1) {
    if (command[0] == WRITE) {
      editorSave();
    } else if (command[0] == QUIT) {
      if (E.dirty > 0) {
        editorSetStatusMessage(
            "Warning! File has unsaved changes. Type 'q!' to force quit");
      } else {
        editorExit();
      }
    }
  } else if (commandlen == 2) {
    if (command[0] == WRITE && command[1] == QUIT) {
      editorSave();
      editorExit();
    }
    if (command[0] == QUIT && command[1] == '!') {
      editorExit();
    }
  }

  E.currentMode = NORMAL_MODE;
}

// Function for writing a promt to the status bar
char *editorPrompt(char *promt) {
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  size_t buflen = 0;
  buf[0] = '\0';

  while (1) {
    editorSetStatusMessage(promt, buf);
    editorRefreshScreen();

    int c = editorReadKey();

    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      if (buflen != 0)
        buf[--buflen] = '\0';
    } else if (c == '\x1b') {
      editorSetStatusMessage("", 0);
      free(buf);
      return NULL;
    } else if (c == '\r') {
      if (buflen != 0) {
        editorSetStatusMessage("");
        return buf;
      }
    } else if (!iscntrl(c) && c < 128) {
      if (buflen == bufsize - 1) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }

      buf[buflen++] = c;
      buf[buflen] = '\0';
    }
  }
}
