// Based on https://github.com/estebanpw/quickhits

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <string.h>
#include "structs.h"

/*
  Print the error message 's' and exit(-1)
*/
void terror(char *s) {
    printf("ERR**** %s ****\n", s);
    exit(-1);
}

/*
  Function to read char by char buffered from a FILE
*/
char buffered_fgetc(char *buffer, uint64_t *pos, uint64_t *read, FILE *f) {
    if (*pos >= READBUF) {
        *pos = 0;
        memset(buffer, 0, READBUF);
        *read = fread(buffer, 1, READBUF, f);
    }
    *pos = *pos + 1;
    return buffer[*pos-1];
}
