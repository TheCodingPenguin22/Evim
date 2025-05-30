#include <stdlib.h>

#include "data.h"
#include "init.h"
#include "terminal.h"

void initEditor() {
  E.cx = 0;
  E.cy = 0;
  E.rx = 0;
  E.rowoff = 0;
  E.coloff = 0;
  E.numrows = 0;
  E.row = NULL;
  E.dirty = 0;
  E.filename = NULL;
  E.statusmsg[0] = '\0';
  E.statusmsg_time = 0;
  E.isDeleting = 0;

  E.mBuffer.maxBufferSize = 128;
  E.mBuffer.buffer = malloc(E.mBuffer.maxBufferSize);
  E.mBuffer.bufferSize = 0;
  E.mBuffer.buffer[E.mBuffer.bufferSize] = '\0';

  E.modes = malloc(3 * sizeof(editorMode));
  E.modes[NORMAL_MODE].name = "Normal";
  E.modes[INSERT_MODE].name = "Insert";
  E.modes[COMMAND_MODE].name = "Command";

  E.currentMode = 0;

  if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    die("getWindowSize");
  E.screenrows -= 2;
}
