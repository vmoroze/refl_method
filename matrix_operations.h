#ifndef _MATRIX_OPERATIONS
#define _MATRIX_OPERATIONS

#include <iostream>
#include <fstream>
#include <cmath>

double _max(double a, double b);
double _abs(double a, double b);
int createMatrixFromFile(char *filename, double *matrix, int len);
int createMatrixWithoutFile(double *matrix, int len, int option);
int createColumnB(double *matrix, double *B, int len);
int printMatrix(double *matrix, int len, int endOfOutput, int mode);
double checkDiscrepancy(double *temp, double *A, double *vector, double *B, int len);
double checkInaccuracy(double *vector, int len);
int multiplyMatrixVector(double *matrix, double *vector, double *result, int len);
int normMatrix(double *matrix, double *vector, int len);
long int get_time(void);
long int get_full_time(void);

#endif
