
#include "terminal.h"
#include "init.h"
#include "file_io.h"
#include "output.h"
#include "input.h"

// Main function, takes in arguments from the cli. 
int main(int argc, char *argv[])
{
  enableRawMode();
  initEditor();

  if(argc >= 2) {
    editorOpen(argv[1]);
  }

  editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");
  
  while (1){
    editorRefreshScreen();
    if(E.currentMode == 0) editorProcessKeypressNormalMode();
    if(E.currentMode == 1) editorProcessKeypressInserMode();
  }
  return 0;
}


