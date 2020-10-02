#pragma once

struct Complex {
	FLOAT_TYPE real;
	FLOAT_TYPE imaginary;
};


void add(struct Complex left, struct Complex right, struct Complex* result);
void complexPow(struct Complex* n, FLOAT_TYPE val);
FLOAT_TYPE normalize(struct Complex n);
void mult(struct Complex left, struct Complex right, struct Complex* result);
int isEqual(struct Complex left, struct Complex right);
inline int fEquals(FLOAT_TYPE left, FLOAT_TYPE right);
void duplicate(struct Complex original, struct Complex* copy);