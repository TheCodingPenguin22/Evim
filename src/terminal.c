#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "data.h"
#include "terminal.h"

// Prints the error msg and exits the program.
void die(const char *s) {
    // Most C library function will set errno to indicate what the error was and perror reads that value and prints it.
    perror(s);
    exit(1); 
  }
  
  // Function to disable terminal raw mode
  void disableRawMode() {
    // Sets the current terminal attributes to the original.
    // Calls TCSAFLUSH to flush the leftover input after the terminal reads 'q'
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) 
      die("tcsetattr");
  }
  
  // Function to enable terminal raw mode.
  void enableRawMode() {
    // Gets the current attributes of the terminal and saves them to orig_termios
    if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  
    // Class disableRawMode() at exit.
    atexit(disableRawMode);
  
    // Declares a struct of type termios
    // Sets its initital value to the initial terminal attributes.
    struct termios raw = E.orig_termios;
  
    // Use tcgetattr to read arguments into the raw struct
    tcgetattr(STDIN_FILENO, &raw);
  
    // c_iflag is the input flag
    // IXON disables ctrl-s ctrl-q
    // ctrl-m is interpreted as a new line and ICRNL disables this interpretation
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  
    // c_oflag is the output flag
    // OPOST disables all output prossesing features
    raw.c_oflag &= ~(OPOST);
  
    // c_lflag is a flag for local flags, it is described as a "dumping ground for other state."
    // Other flas include c_iflag(input flag) and c_oflag(output flag).
    // ECHO the echoing of keypresses to the terminal
    // ICANON disable having to press enter for the program to read the input
    // ISIG disables ctrl-c and ctrl-z
    // IEXTEN disables ctrl-v
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  
    // The c_cc field stands for "controll characters" and is an array of bytes that controll various terminal settings.
    // c_cc[VMIN] sets the minimum number of bytes of input needed before read() can return.
    // In this case read() needs 0 bytes minimum in order to return. 
    raw.c_cc[VMIN] = 0;
  
    // c_cc[VTIME] sets the minimum amount of time before read() returns.
    // In this case it is set to 1 so that means it needs to wait 1/10 of a second before returning.
    raw.c_cc[VTIME] = 1;
  
  
    // Use tcsetattr to apply the arguments to the terminal
    // TCSAFLUSH means we want to apply the change
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
  }
  
  // Waits for one keypress then returns it.
  // Also handles the keypress and returns the appropriate values.
  int editorReadKey() {
    int nread;
    char c;
    // Loop continues to run while a has not been pressed.
    while((nread = read(STDIN_FILENO, &c, 1)) != 1){
      if(nread == -1 && errno != EAGAIN) die("read");
    }
    // If character is an escape character 
    if(c == '\x1b') {
      char seq[3];
  
      // Reads the next two characters into the seq array.
      if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
      if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    
      
      if(seq[0] == '[') {
        if(seq[1] >= 0 && seq[1] <= '9') {
          if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
          if(seq[2] == '~') {
            switch (seq[1]) {
              case '1': return HOME_KEY;
              case '3': return DEL_KEY;
              case '4': return END_KEY;
              case '5': return PAGE_UP;
              case '6': return PAGE_DOWN;
              case '7': return HOME_KEY;
              case '8': return END_KEY;
            }
          }
        }
        else {
          switch(seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            case 'H': return HOME_KEY;
            case 'F': return END_KEY;
          }
        }
        
      }
      else if( seq[0] == 'O') {
        switch(seq[1]) {
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
  
      return '\x1b';
    }
    else{
      return c;
    }
  }
  
  // Gets the current posistion of the cursor in the terminal
  int getCursorPosition(int *rows, int *cols) {
    char buff[32];
    unsigned int i = 0;
  
    // Tells the terminal to send the current cursor position to the terminal 
    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    
    // Reads the terminal response one character at a time until it this the buffer limit or it read 'R'.
    while(i < sizeof(buff) - 1){
      
      if (read(STDIN_FILENO, &buff[i], 1) != 1) break;
      if (buff[i] == 'R') break;
      i++;
    }
    // Makes the buffer into a string with the null terminaltion character
    buff[i] = '\0';
  
  
    // Makes sure the response starts with an escape sequence 'x1b['.
    if(buff[0] != '\x1b' || buff[1] != '[') return -1;
  
    // Scans the buffer for a string like "%d;%d", two numbers seperated by a semi-colon.
    // Then puts those values into rows and cols respektivly.
    if(sscanf(&buff[2], "%d;%d", rows, cols) != 2) return -1;
  
    return 0;
  }
  
  // Gets the current size of the terminal window
  int getWindowSize(int *rows, int *cols){
    struct winsize ws;
  
    // ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) sets ws to the size of the terminal
    // TIOCGWINSZ stands for Terminal Input/Output Control Get WINdow SiZe.
  
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
  
      // Puts cursor the the bottom right of the screen.
      if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
      return getCursorPosition(rows, cols);
      return -1;
    }
    else
    {
      *cols = ws.ws_col;
      *rows = ws.ws_row;
      return 0;
    }
  }