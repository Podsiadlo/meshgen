#ifndef ASC_READER_H
#define ASC_READER_H

#include <stddef.h>
#include <stdio.h>

int
readLine(FILE *f, char *buffer, const size_t buffersize, size_t *line_number);

double **
readASC(size_t *cols, size_t *rows, const char *filename);

double **
convert(double** coords, const size_t nRows, const size_t nCols);

#endif // ASC_READER_H
