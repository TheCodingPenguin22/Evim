#ifndef OUTPUT_H
#define OUTPUT_H

#include "data.h"

void editorScroll();
void editorDrawRows(struct abuf *ab);
void editorDrawSatusBar(struct abuf *ab);
void editorDrawMessageBar(struct abuf *ab);
void editorRefreshScreen();
void editorSetStatusMessage(const char* fmt, ...);


#endif