#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>

#include "input.h"
#include "terminal.h"
#include "file_io.h"
#include "data.h"
#include "row_operations.h"
#include "output.h"

// This function is responsible for moving the cursor.
void editorMoveCursor(int key) {
  // Sets the current row.
  // If the current cursor y pos is greater than the number of rows then it sets the row to NULL. 
  erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

  // Switch case for checking where to move the cursor
  switch(key) {
    case ARROW_LEFT:
      if(E.cx != 0) {
        E.cx--;
      }
      // Moves the cursor to the end of the line above if the user pressed left at the start of a line.
      else if(E.cy > 0) {
        E.cy--;
        E.cx = E.row[E.cy].size;
      }
      break;
    case ARROW_RIGHT:
      if(row && E.cx < row->size) {
        E.cx++;
      }
      // Sets the cursor to the start of the line below if one exists.
      else if(row && E.cx == row->size) {
        E.cy++;
        E.cx = 0;
      }
      break;
    case ARROW_UP:
      if(E.cy != 0) {
        E.cy--;
      }
      break;
    case ARROW_DOWN:
      if(E.cy < E.numrows){
        E.cy++;
      }
      break;
  }
  
  // If the cursor is past the end of the file set it to NULL otherwise the its row to the current row.
  row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
  
  // If row is NULL then set rowlen to 0 otherwise set it to the size of the current row.
  int rowlen = row ? row->size : 0;

  // If the x pos of the cursor if beyond the rowlen then it sets the x pos to the current rows len.
  if(E.cx > rowlen){
    E.cx = rowlen;
  }
} 
  
  // Waits for editorReadKey() then it handles the keypresses that the function returns.
void editorProcessKeypress() {
  static int quit_times  = EVIM_QUIT_TIMES;
  int c = editorReadKey();

  switch (c) {
    case '\r':
      /* TODO*/
      break;
    // Quits if it gets ctrl-Q.
    case CTRL_KEY('q'):
      if(E.dirty && quit_times > 0){
        editorSetStatusMessage("WARING!!!! File has unsaved changes. Press ctrl-Q %d more times to quit.", quit_times);
        quit_times--;
        return;
      }
      write(STDOUT_FILENO, "\x1b[2J", 4);  
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;

    case CTRL_KEY('s'):
      editorSave();
      break;
    // Goes to start of line if HOME key is pressed.
    case HOME_KEY:
      E.cx = 0;
      break;

    // Goes to end of line if END key is pressed.
    case END_KEY:
      if (E.cy < E.numrows){
        E.cx = E.row[E.cy].size;
      }
      break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
      /* TODO*/
      break;
    // Goes to top of file if PAGE_UP is pressed
    // and bottom of file if PAGE_DOWN is pressed.
    case PAGE_UP:
    case PAGE_DOWN:
      {
        if (c == PAGE_UP){
          E.cy = E.rowoff;
        }
        else if (c == PAGE_DOWN ){
          E.cy = E.rowoff + E.screenrows - 1;
          if(E.cy > E.numrows) E.cy = E.numrows;
        }

        int times = E.screenrows;
        while (times--){
          editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
      }
      break;

    // Moves the cursor, se editorMoveCursor.
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      editorMoveCursor(c);
      break;
    
    case CTRL_KEY('l'):
    case '\x1b':
      break;

    default:
      editorInsertChar(c);
      break;
  }
  quit_times = EVIM_QUIT_TIMES;
}

