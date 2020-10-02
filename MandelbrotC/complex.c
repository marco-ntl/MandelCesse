#pragma once
#include "constants.h"
#include <math.h>
#include <stdlib.h>
#include "complex.h"

void add(struct Complex left, struct Complex right, struct Complex* result) {
	result->real = left.real + right.real;
	result->imaginary = left.imaginary + right.imaginary;
}

void mult(struct Complex left, struct Complex right, struct Complex* result) {
	result->real = left.real * right.real - left.imaginary * right.imaginary;
	result->imaginary = left.imaginary * right.real + left.real * right.imaginary;
}

void complexPow(struct Complex* n, FLOAT_TYPE val) {
	double r = n->real, i = n->imaginary;
	n->real = pow(r, val) - pow(i, val);
	n->imaginary = pow(r, val) * i;
}

FLOAT_TYPE normalize(struct Complex n) {
	return n.real * n.real + n.imaginary * n.imaginary;
}

#define EPSILON 0.00001f
int isEqual(struct Complex left, struct Complex right) {
	return (fEqual((double)left.real, (double)right.real) && fEqual((double)left.imaginary, (double)right.imaginary));
}

inline int fEqual(double left, double right) {
	return (fabs(left - right) < EPSILON);
}

void duplicate(struct Complex original, struct Complex *copy)
{
	copy->real = original.real;
	copy->imaginary = original.imaginary;
}