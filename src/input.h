#ifndef INPUT_H
#define INPUT_H

void editorMoveCursor(int key);
void editorProcessKeypressInserMode();
void editorProcessKeypressNormalMode();
void editorProcessKeypressCommandMode();
char* editorPrompt(char* promt);

#endif