#pragma once
#include "structs.h";

const struct GradientPoint TWO_COLORS_PALETTE[] = {
	{0, {0,0,0}},
	{1, {255,255,255}}
};

const struct GradientPoint FIRST_PALETTE[] = {
	{0, {0,7,100}},
	{0.16, {32,107,203}},
	{0.42, {237,255,255}},
	{0.6425, {255,170,0}},
	{0.80, {230,200,25}},
	{0.9575, {0,2,0}}
};


const struct GradientPoint SECOND_PALETTE[] = {
	{0,{217, 186, 184}},
	{0.9,{209, 87, 79}},
	{0.18,{153, 120, 20}},
	{0.27,{79, 138, 11}},
	{0.36,{11, 138, 96}},
	{0.45,{7, 30, 112}},
	{0.54,{78, 4, 112}},
	{0.65, {150, 11, 122}},
	{0.74,{84, 9, 40}},
	{0.85,{48, 48, 2}},
	{0.9,{0,2,0}}
};

const struct GradientPoint THIRD_PALETTE[] = {
	{0, {0,7,100}},
	{0.16, {0,2,0}},
	{0.42, {32,107,203}},
	{0.6425, {237,255,255}},
	{0.8575, {255,170,0}}
};

const struct GradientPoint FIFTH_PALETTE[] = {
	{0, {149,17,189}},
	{0.162,{255,0,83}},
	{0.42, {153,14, 16}},
	{0.6425, {16,107,81} },
	{0.8575, {41,1,30}}
};

const struct GradientPoint FOURTH_PALETTE[] = {
	{0, {0,0,0}},
	{0.16,{199,38,201}},
	{0.42, {81,222,16} },
	{0.6425, {201,204,8}},
	{0.8575, {8,11,12}}
};

const struct GradientPoint ZEBRA_PALETTE[] = {
	{0, {255,255,255}},
	{0.20, {50,50,50}},
	{0.40, {200,200,200}},
	{0.65, {100,100,100}},
	{0.90, {150,150,150}},
	{0.99, {0,0,0}},
};

const struct GradientPoint* PALETTES[] = {
	FIRST_PALETTE, SECOND_PALETTE, THIRD_PALETTE, FOURTH_PALETTE,
	FIFTH_PALETTE, ZEBRA_PALETTE, TWO_COLORS_PALETTE
};

const size_t NB_PALETTES = 7;