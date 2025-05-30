
#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <termios.h>
#include <time.h>

#define EVIM_VERSION "0.1.1"
#define EVIM_TAB_STOP 4
#define CTRL_KEY(k) ((k) & 0x1f)
#define EVIM_QUIT_TIMES 3

#define WRITE 'w'
#define QUIT 'q'

// An enumarator to keep track of the chosen values for each key.
enum editorkey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

enum modes {
  NORMAL_MODE = 0,
  INSERT_MODE,
  COMMAND_MODE,
};

/*** data ***/
// A struct to keep track of the current editor mode
typedef struct editorMode {
  char *name;
} editorMode;

// A struct that holds info about the current row.
typedef struct erow {
  int size;
  int rsize;
  char *chars;
  char *render;
} erow;

// A struct to hold the buffer for the vim motions
typedef struct motionBuffer {
  char *buffer;
  size_t bufferSize;
  size_t maxBufferSize;
} motionBuffer;

// A struct to hold all of the current configuration for the terminal.
struct editorConfig {
  int cx, cy;
  int rx;
  int rowoff;
  int coloff;
  int screenrows;
  int screencols;
  int numrows;
  erow *row;
  int dirty;
  struct termios orig_termios;
  char *filename;
  char statusmsg[80];
  time_t statusmsg_time;
  editorMode *modes;
  int currentMode;
  struct motionBuffer mBuffer;
  int isDeleting;
};

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}

// Creating a global instance of the editorConfig struct.
extern struct editorConfig E;

#endif
