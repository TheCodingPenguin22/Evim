#ifndef INPUT_H
#define INPUT_H

void editorProcessKeypressVimMotion(char c);
void editorMoveCursor(int key);
void editorProcessKeypressInserMode();
void editorProcessKeypressNormalMode();
void editorProcessKeypressCommandMode();
char *editorPrompt(char *promt);

#endif
