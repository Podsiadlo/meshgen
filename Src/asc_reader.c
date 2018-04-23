#include "asc_reader.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
readLine(FILE *f, char *buffer, const size_t buffersize, size_t *line_number)
{

    do {
        if (fgets(buffer, buffersize, f) != NULL) {
            ++(*line_number);
            char *p = strchr(buffer, '\n');
            if (p) {
                *p = '\0';
            } else {
                fprintf(stderr,
                        "Line %zu longer than buffer\n"
                        "line content: %s\n",
                        *line_number, buffer);
                return 1;
            }
        } else {
            *buffer = '\0';
            return 2;
        }
    } while (((buffer[0] == '#') || (buffer[0] == '\0')));

    return 0;
}

double **
readASC(size_t *cols, size_t *rows, const char *filename)
{
    const size_t tambuf = 256;
    char buf[tambuf];
    size_t line_number = 0;

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    double dtm_data[6] = {INFINITY, INFINITY, INFINITY,
                          INFINITY, INFINITY, INFINITY};

    for (size_t data_idx = 0; data_idx < 6; ++data_idx) {
        if (readLine(fp, buf, tambuf, &line_number) == 0) {
            const char space = ' ';
            const char *result = strchr(buf, space);
            if (result == NULL) {
                fprintf(stderr,
                        "Header line has no space\n"
                        "%s:%zu\n",
                        filename, line_number);
                exit(EXIT_FAILURE);
            }
            char *p;
            dtm_data[data_idx] = strtod(result, &p);
            if (buf == p) {
                fprintf(stderr,
                        "%s: not a decimal number\n"
                        "%s:%zu\n",
                        buf, filename, line_number);
                exit(EXIT_FAILURE);
            }

        } else {
            fprintf(stderr,
                    "Problem reading ASC file\n"
                    "%s:%zu\n",
                    filename, line_number);
            exit(EXIT_FAILURE);
        }
    }

    const size_t nCols = dtm_data[0];
    const size_t nRows = dtm_data[1];
    const double xMin = dtm_data[2];
    const double yMin = dtm_data[3];
    const double cellSize = dtm_data[4];
    const double noData = dtm_data[5];

    const size_t numOfPoints = nCols * nRows;

    double **coords = (double **)malloc(sizeof(double *) * numOfPoints);

    for (size_t i = 0; i < numOfPoints; ++i) {
        coords[i] = (double *)malloc(sizeof(double) * 3);
    }

    for (size_t j = 0; j < nRows; ++j) {
        const double y = yMin + (cellSize * (nRows - (j + 1)));

        if (readLine(fp, buf, tambuf, &line_number) != 0) {
            fprintf(stderr,
                    "Problem reading ASC file\n"
                    "%s:%zu\n",
                    filename, line_number);
            exit(EXIT_FAILURE);
        }

        char *buf_dummy = buf;
        for (size_t i = 0; i < nCols; ++i) {

            const double x = xMin + (cellSize * i);

            char *p;
            const double z = strtod(buf_dummy, &p);
            if (buf_dummy == p) {
                fprintf(stderr,
                        "%s: not a decimal number\n"
                        "%s:%zu\n",
                        buf_dummy, filename, line_number);
                exit(EXIT_FAILURE);
            } else {
                buf_dummy = p + 1;
            }

            coords[i + (nCols * j)][0] = x;
            coords[i + (nCols * j)][1] = y;
            coords[i + (nCols * j)][2] = z;
        }
    }

    double **map = convert(coords, nRows, nCols);
    *cols = nCols;
    *rows = nRows;

    for (int k = 0; k < numOfPoints; ++k) {
        free(coords[k]);
    }
    free(coords);
    return map;
}

double **
convert(double** coords, size_t nRows, size_t nCols) {
    double ** map = (double **)malloc(nRows*sizeof(double*));
    for (size_t k = 0; k < nRows; ++k) {
        map[k] = (double *)malloc(nCols* sizeof(double));
        for (size_t i = 0; i < nCols; ++i) {
            map[k][i] = coords[k*nCols+i][2];
        }
    }
    return map;
}
