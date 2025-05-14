#include "editorMotions.h"
#include "data.h"
#include "editor_operations.h"

void vimMotionO() {
  editorInsertNewline();
  E.currentMode = INSERT_MODE;
}
