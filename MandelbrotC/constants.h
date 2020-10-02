#pragma once
#define TRUE 1
#define FALSE 0

#define WIDTH 1366
#define HEIGHT 768
#define SCALE 2
#define MAX_ITERATIONS 100
#define ESCAPE_CONDITION 100000
#define USE_SCREEN_SIZE TRUE

#define LOG2 0.69314718055994530941723212145817


#define DEFAULT_PALETTE 0

#define USE_OPENGL TRUE
#define FLOAT_TYPE double
#define USE_DYNAMIC_PALETTE TRUE

#define STARTING_DISTANCE 0.0

#define END_DISTANCE 1

#define USE_NORMALIZED_COUNT TRUE
#define USE_LOG10_NORMALIZED_COUNT FALSE
#define USE_WEIRD_NORMALIZED_COUNT FALSE

#define USE_LIBPNG TRUE

#define X_LOWER_BOUND -2.0
#define X_UPPER_BOUND 0.6

#define Y_LOWER_BOUND -1.0
#define Y_UPPER_BOUND 1.0

#define X_RANGE X_UPPER_BOUND - X_LOWER_BOUND
#define Y_RANGE Y_UPPER_BOUND - Y_LOWER_BOUND

#define SINGLE_IMAGE TRUE

//Defining a low ram mode
//In low ram mode, only the intensity of each pixel is kept in RAM
#define LOW_RAM_MODE TRUE

#define NB_CHANNELS 3
#define CHECK_PERIOD FALSE
#define COLORS_PER_STEP 3.0

#if LOW_RAM_MODE == TRUE
#define CURRENT data[index++]
#else
#define CURRENT data[y][x]->intensity
#endif