#include <stdlib.h>

#include "init.h"
#include "data.h"
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

  E.modes = malloc(2 * sizeof(editorMode));
  E.modes[0].name = "Normal";
  E.modes[1].name = "Insert";

  E.currentMode = 0;

  if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
  E.screenrows -= 2;
}