
#include "append_buffer.h"
#include <stdlib.h>
#include <string.h>

// Appends the given char to the end of the buffer
void abAppend(
    struct abuf *ab, const char *s,
    int len) { // Reallocates the memory to a new char pointer called char
  // The new length is the current length plus the length of the new char
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL)
    return;
  // Copys the new character to the end of the new buffer.
  memcpy(&new[ab->len], s, len);
  // Sets the old char pointer to the new
  ab->b = new;
  // Set the old length to the new
  ab->len += len;
}

void abFree(struct abuf *ab) {
  // Frees the space taken up by the char pointer.
  free(ab->b);
}
