#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "output.h"
#include "append_buffer.h"
#include "row_operations.h"

// Responsible for scrolling in the editor.
// Increments and decrements the rowoff or coloff when appropriate. 
void editorScroll() {
  E.rx = 0;
  if(E.cy < E.numrows) E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);

  if(E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if(E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if(E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if(E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }

}
// Draws all the rows that fit in E.screenrows.
void editorDrawRows(struct abuf *ab) {
  int y;
  for(y = 0; y < E.screenrows; y++) {
    // Handels the row offset.
    int filerow = y + E.rowoff;
    
    // This runs if the current row is is greater than the amount of rows in the file.
    if(filerow >= E.numrows) {
      
      // If there is no file then it prints the welcome message.
      if(E.numrows == 0 && y == E.screenrows / 3) {
      char welcome [80];
      
      // snprintf returns the size of the given array (welcome) plus the string literal and the arguments passed to it, in this case EVIM_VERSION.
      int welcomelen = snprintf(welcome, sizeof(welcome),
        "Evim editor -- version %s", EVIM_VERSION);
      
      // Sets the maximum length of the welcomemsg to the screen width.
      if(welcomelen > E.screencols ) welcomelen = E.screencols;

      // Calculates the padding to each side of the welcomemsg to center it.
      int padding = (E.screencols - welcomelen) / 2;
      // Adds a ~ to the start of the welcome text line.
      if(padding) {
        abAppend(ab, "~", 1);
        padding--;
      } 

      // Continusly adds spaces while padding-- is still possible
      while (padding--) abAppend(ab, " ", 1);
      abAppend(ab, welcome, welcomelen);
      }
      // If the file has one or more lines then it fills the rest of the lines with ~
      else{
        abAppend(ab, "~", 1);      
      }
    }
    // If the current row is less than the amount of rows in the file.
    // Aka it just prints the row.
    else {
      // Calculates the size of the row.
      int len = E.row[filerow].rsize - E.coloff;

      // Min size of 0;
      if(len < 0) len = 0;

      // Max size of screen width.
      if(len > E.screencols) len = E.screencols;

      // Adds the row to the buffer.
      // It starts the line from the point where the coluomn offset is.
      // So say a line is 10 chars long and the current coloumn offset is 6 then it would start printing the line from the 6th char.
      // This enables horizontal scrolling.
      abAppend(ab, &E.row[filerow].render[E.coloff], len);
    }
    
    // This clears the current line.
    abAppend(ab, "\x1b[K", 3);
    
    //This adds a new line to the end of the terminal.
    abAppend(ab, "\r\n", 2);
    
    
  }
}

// Draws the status bar to the buffer.
void editorDrawStatusBar(struct abuf *ab) {
  // Inverts the colours of the terminal for the current row.
  abAppend(ab, "\x1b[7m", 4);
  char status[80], posstatus[80];
  // Calculates the length of the status bar string. 
  // Sets the filename to [No Name] if there is no filename avaliable.
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s - %s mode", E.filename ? E.filename : "[No Name]", E.numrows, E.dirty ? "(modified)" : "", E.modes[E.currentMode].name);

  // Calculates the length of the posistion string.
  int poslen = snprintf(posstatus, sizeof(posstatus), "%d:%d", E.cy + 1, E.cx+1);

  // Sets max length of status to screen width.
  if(len > E.screencols) len = E.screencols;

  // Adds the status to the buffer.
  abAppend(ab, status, len);

  while (len < E.screencols){
    // Checks if the length of the posstatus string literal is equal to the amount of space left.
    // If it is then it adds the posistion status to the buffer.  
    if(E.screencols - len == poslen){
      abAppend(ab, posstatus, poslen);
      break;
    }
    // If not then it adds another space and increments the len;
    else{
      abAppend(ab, " ", 1);
      len++;
    }
  }
  // Reverst the colour inversion.
  abAppend(ab, "\x1b[m", 3);

  // Adds a new line at the bottom.
  abAppend(ab, "\r\n", 2);
}

// Draws the message bar to the bottom of the file.
void editorDrawMessageBar(struct abuf *ab){
  // Clears everyting to the right of the cursor.
  abAppend(ab, "\x1b[K", 3);

  // Gets the length of the statusmsg
  int msglen = strlen(E.statusmsg);
  // Sets the max length of the statusmsg to the screen width.
  if(msglen > E.screencols) msglen = E.screencols;

  // Checks if the msglen is non-zero and if the amount of time that has passed since the status msg has been displayed is less than 5 sec.
  // If true then add statusmsg to buffer.
  if(msglen && time(NULL) - E.statusmsg_time < 5) abAppend(ab, E.statusmsg, msglen);
}

// Clears the screen by writing an escape sequence
void editorRefreshScreen() {
  editorScroll();
  
  struct abuf ab = ABUF_INIT;
  
  // Hide the cursor during draw
  abAppend(&ab, "\x1b[?25l", 6);

  // Puts the cursor at the top-left corner
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  // Sets the current cursor position to the one specified in the editorconfig struct
  char buff[32];
  snprintf(buff, sizeof(buff), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.rx - E.coloff) + 1);
  abAppend(&ab, buff, strlen(buff));

  // Shows the cursor again
  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

// Sets the status message
// The "..." make this a varidic function and means it can take a variable amount of arguments.
// fmt is a formated string. Could be something like "Hello, %s!", the s would be passed as a extra argument.
void editorSetStatusMessage(const char* fmt, ...){
  // The extra arguments is stored in a va_list.
  va_list ap;

  // Initializes ap to take agruments after fmt.
  va_start(ap, fmt);
  // Sets the given string and adds any arguemts passed to the function.
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);

  // Cleans up the argument buffer.
  va_end(ap);

  // Sets the timestamp for when the msg was set.
  E.statusmsg_time = time(NULL);
}