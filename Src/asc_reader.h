#ifndef ASC_READER_H
#define ASC_READER_H

#include <stddef.h>
#include <stdio.h>

int
readLine(FILE *f, char *buffer, const size_t buffersize, size_t *line_number);

short **
readASC(const char *filename);

short **
convert(double** coords, int nRows, int nCols);

#endif // ASC_READER_H
