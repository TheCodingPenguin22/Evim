#include "data.h"
#include "row_operations.h"
#include "file_io.h"
#include "terminal.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>


char* editorRowsToString(int *buflen){
  int totlen = 0;
  int j;
  // Add up the total length of all the rows plus 1 for the new line char at the end of each line.
  for(j = 0; j < E.numrows; j++){
    totlen += E.row[j].size + 1;
  }
  *buflen = totlen;

  // Allocating the requierd amount of space.
  char *buf = malloc(totlen);

  // p acts as a writer head. 
  // It means we can increment p to go over the buffer without incrementing the buf pointer which we are returning.
  char *p = buf;

  // Loop over all the rows.
  for (j = 0; j < E.numrows; j++){
    // Copy each row into p. (Which copies it into buf, because pointers)
    memcpy(p, E.row[j].chars, E.row[j].size);

    // Sets the pointer to the end of the current row.
    p += E.row[j].size;

    // Adds a new line char at the end.
    *p = '\n';

    // Increments to the new end of the row.
    p++;
  }

  return buf;
}

// Opens the file that has been passed as a argument to the cli exe
void editorOpen(char *filename) {
  // Gets the name of the file
  free(E.filename);
  E.filename = strdup(filename);

  // Opens the file
  FILE *fp = fopen(filename, "r");
  if(!fp) die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;

  // Loops over each line of the file until the getline function returns -1 which means that there are no more lines.
  while((linelen = getline(&line, &linecap, fp)) != -1) {
    // Removes '\n' or '\r' from the line.
    while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r')) 
    linelen--;

    editorAppendRow(line, linelen);
  }

  free(line);
  fclose(fp);
  E.dirty = 0; 
}

void editorSave() {
  if (E.filename == NULL) return;

  // The length of the total file.
  int len;

  // Gets the entier file as a string.
  char *buf = editorRowsToString(&len);

  // Opens the file for reading and writing (O_RDWR), if there is no file, then it creats a new one (O_CREAT)
  // 0644 is the standard permission you want to text files.
  int fd = open(E.filename, O_RDWR | O_CREAT, 0644);

  // Error handeling
  if (fd != -1) {
    if(ftruncate(fd, len) != -1){
      if(write(fd, buf, len) == len) {
        close(fd);
        free(buf);
        editorSetStatusMessage("%d bytes writen to disk!", len);
        E.dirty = 0;
        return;
      }
    }
    close(fd);
  }
  free(buf);
  editorSetStatusMessage("Can't save I/O eror: %s", strerror(errno));
}