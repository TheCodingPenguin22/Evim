#ifndef APPEND_BUFFER_H
#define APPEND_BUFFER_H
#include "data.h"

void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

#endif
