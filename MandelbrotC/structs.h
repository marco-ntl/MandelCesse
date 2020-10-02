#pragma once
#include "constants.h"

struct PointF {
	double x, y;
};

struct Point {
	unsigned short x, y;
};

struct GraphPoint {
	struct Point pos;
	struct Complex *scaledPos;
	FLOAT_TYPE intensity;
};

struct Pixel {
	uint8_t r, g, b;
};

struct GradientPoint {
	FLOAT_TYPE absPosition;
	struct Pixel color;
};
