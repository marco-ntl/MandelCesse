#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "constants.h"
#include "complex.h"
#include "structs.h"
#include "palettes.h"

#define GLFW_DLL
#include "libs/GLFW/glfw3.h"

#define length(a)	sizeof(a) / sizeof(a[0])
#define colors(i) PALETTES[i]
#define currPalette colors(world->settings.currentPaletteIndex)

struct MandelbrotSettings {
	int width;
	int height;
	unsigned long long maxIterations;
	unsigned long long escapeCondition;
	int currentPaletteIndex;
};

struct Global {
	FLOAT_TYPE* data;
	struct Pixel* dynamicPalette;
	int isPaletteInitialized;
	GLFWwindow* window;
	int gradientPoints;
	struct MandelbrotSettings settings;
	char title[100];
};

struct Global* world;

/*
	Linearly interpolates a value with a new range
	I.E : lerp(3, 5, 10) = 6
*/
FLOAT_TYPE lerp(FLOAT_TYPE val, FLOAT_TYPE max, FLOAT_TYPE newMax) {
	if (val <= 0)
		return 0;

	if (val > max)
		return max;

	FLOAT_TYPE absoluteScale = val / max;
	return newMax * absoluteScale;
}

FLOAT_TYPE rangedLerp(FLOAT_TYPE val, FLOAT_TYPE min, FLOAT_TYPE max, FLOAT_TYPE newMin, FLOAT_TYPE newMax) {
	FLOAT_TYPE range, newRange, result;
	range = max - min;
	newRange = newMax - newMin;
	return lerp(val, range, newRange) + newMin;
}

int isInMainCardioid(struct Complex c) {
	/*
		Checks Whether a complex number C lies within the main cardioid
		https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
	*/
	double p;
	p = sqrt(pow(c.real - 0.25, 2) + pow(c.imaginary, 2));
	return (c.real <= p - (2 * pow(p, 2)) + 0.25) ? TRUE : FALSE;
}

int isInPeriod2Bulb(struct Complex c) {
	/*
	Checks Whether a complex number C lies within the period-2 bulb
	https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
	*/
	return (pow(c.real + 1, 2) + pow(c.imaginary, 2) <= 0.0625) ? TRUE : FALSE;
}

struct Pixel colorLerp(struct Pixel low, struct Pixel high, FLOAT_TYPE distance) {
	struct Pixel result = { 0,0,0 };
	result.r = rangedLerp(distance, STARTING_DISTANCE, END_DISTANCE, low.r, high.r);
	result.g = rangedLerp(distance, STARTING_DISTANCE, END_DISTANCE, low.g, high.g);
	result.b = rangedLerp(distance, STARTING_DISTANCE, END_DISTANCE, low.b, high.b);
	return result;
}

struct Pixel GetPixelColorWithFractionalPartFromDynamicPalette(int nbIterations, FLOAT_TYPE fractionalPart) {
	if (nbIterations >= world->settings.maxIterations)
		return world->dynamicPalette[world->settings.maxIterations - 1];

	return colorLerp(world->dynamicPalette[nbIterations], world->dynamicPalette[nbIterations + 1], fractionalPart);
}

struct Pixel GetPixelColorWithFractionalPart(FLOAT_TYPE distance, FLOAT_TYPE fractionalPart) {
	for (size_t i = 1; i < world->gradientPoints; i++)
	{
		if (distance < currPalette[i].absPosition)
			return colorLerp(currPalette[i - 1].color, currPalette[i].color, fractionalPart);
	}
	return currPalette[world->gradientPoints - 1].color;
}

struct Pixel GetPixelColorFromGradient(FLOAT_TYPE distance) {
	FLOAT_TYPE rangeSize = 0.0, range = 0.0;
	for (size_t i = 1; i < world->gradientPoints; i++)
	{
		if (distance < currPalette[i].absPosition)
		{
			rangeSize = currPalette[i].absPosition - currPalette[i - 1].absPosition;
			range = distance - currPalette[i - 1].absPosition;

			return colorLerp(currPalette[i - 1].color, currPalette[i].color, lerp(range, rangeSize, 1.0));
		}
	}
	return currPalette[world->gradientPoints - 1].color;
}

#if CHECK_PERIOD == TRUE
#define PERIOD_LENGTH 50
#endif

double lowest = 1000, highest = -1000;

inline FLOAT_TYPE ComputeIntensity(struct Complex c, int maxIterations) {
	struct Complex z = { 0,0 }, tmp = { 0,0 }, pointCheck = { 0,0 };
	double iterations = 0, result = 0, mu = 0;
	unsigned short period = 0;

	if (isInMainCardioid(c) || isInPeriod2Bulb(c))
		iterations = world->settings.maxIterations - 1;

	do
	{
		//z = z^2
		mult(z, z, &tmp);
		//z += c	
		add(tmp, c, &z);
		iterations++;

#if CHECK_PERIOD == TRUE
		//If z is ast the same point it was before, it means the function cannot diverge and the point is in the set
		if (isEqual(z, pointCheck))
			return 0;

		period++;
		if (period >= PERIOD_LENGTH) {
			period = 0;
			duplicate(z, &pointCheck);
		}
#endif

	} while (normalize(z) < ESCAPE_CONDITION && iterations < maxIterations);
	if (USE_NORMALIZED_COUNT) {

		/*
			Source : http://linas.org/art-gallery/escape/escape.html
		*/

		/*
			We add a couple extra iterations to decrease the size of the error term
			See blog post above
		*/
		//mu = iterations + 1.0 - log(log(normalize(c))) / LOG2;
		if (USE_WEIRD_NORMALIZED_COUNT)
			mu = 1.0 - log10(log10(normalize(z))) / LOG2;
		else
			mu = 1.0 - log(log(normalize(z))) / LOG2;

		/*
		if (mu < lowest) {
			lowest = mu;
			printf("Low\t:\t%f\n", mu);
		}
		if (mu > highest) {
			highest = mu;
			printf("High\t:\t%f\n", mu);
		}
		*/
		return iterations + mu;
	}
	else {
		if (iterations == 1)
			result = 0;
		else
			result = iterations / maxIterations;
		//0 -> black, 1 -> white
		return 1.0 - result;
	}
}

#if USE_LIBPNG == FALSE
bmp_img img;
void setPixel(struct Point pos, int intensity) {
	bmp_pixel_init(&img.img_pixels[pos.x, pos.y], intensity, intensity, intensity);
}

void setRow(int row, int* intensity, size_t nbCol) {
	for (size_t i = 0; i < nbCol; i++)
	{
		bmp_pixel_init(&img.img_pixels[row, i], intensity, intensity, intensity);
	}
}

void writeBMP() {
	bmp_img_write(&img, "test123.bmp");
}

void freeBMP() {
	bmp_img_free(&img);
}
#endif //!USE_LIBPNG

#pragma warning(disable : 4996)
#if USE_LIBPNG == TRUE
#define STB_IMAGE_IMPLEMENTATION
#include "libs\\stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs\\stb_image_write.h"

#define TEST_HEIGHT 100
void testColorPalette() {
	uint8_t* pixels = malloc(sizeof(uint8_t) * world->settings.maxIterations * TEST_HEIGHT * NB_CHANNELS);
	int index = 0;
	for (size_t i = 0; i < TEST_HEIGHT; i++)
	{
		for (size_t y = 0; y < world->settings.maxIterations; y++)
		{
			pixels[index++] = world->dynamicPalette[y].r;
			pixels[index++] = world->dynamicPalette[y].g;
			pixels[index++] = world->dynamicPalette[y].b;
		}

	}
	stbi_write_png("testPalette.png", world->settings.maxIterations, TEST_HEIGHT, NB_CHANNELS, pixels, NB_CHANNELS * sizeof(uint8_t));
}


void generatePNG(char* name) {
	//int stbi_write_png(char const* filename, int w, int h, int comp, const void* data, int stride_in_bytes);
	uint8_t* pixels = malloc(world->settings.width * world->settings.height * NB_CHANNELS);
	struct Pixel tmp = { 0,0,0 };
	FILE* values = fopen("tmp", "rb");
	unsigned long long index = 0;
	FLOAT_TYPE currIntensity;

	for (size_t x = 0; x < world->settings.width; x++)
	{
		for (size_t y = 0; y < world->settings.height; y++)
		{
			if (USE_OPENGL)
				currIntensity = world->data[x * world->settings.width + y];
			else
				fread(&currIntensity, sizeof(FLOAT_TYPE), 1, values);

			if (USE_DYNAMIC_PALETTE) {
				tmp = GetPixelColorWithFractionalPartFromDynamicPalette(currIntensity, fmod(currIntensity, 1));
			}
			else if (!USE_NORMALIZED_COUNT || USE_WEIRD_NORMALIZED_COUNT)
				tmp = GetPixelColorFromGradient(currIntensity);
			else {
				tmp = GetPixelColorWithFractionalPart(lerp(currIntensity, world->gradientPoints, 1.0f), fmod(currIntensity, 1.0));
			}
			pixels[index++] = tmp.r;
			pixels[index++] = tmp.g;
			pixels[index++] = tmp.b;
		}
	}
	fclose(values);
	stbi_write_png(name, world->settings.width, world->settings.height, NB_CHANNELS, pixels, world->settings.width * NB_CHANNELS);
	free(pixels);
}

#else
#include "libs\\libbmp.h"
void generateBMP() {
	struct Pixel clr = { 0,0,0 };
	for (int x = 0; x < world->settings.width; x++)
	{
		for (int y = 0; y < world->settings.height; y++)
		{
			//intensity = floor((MAX_INTENSITY * pow(intensity, CONTRAST)));
			clr = GetPixelColorFromGradient(CURRENT);
			bmp_pixel_init(&img.img_pixels[x][y], clr.r, clr.g, clr.b);
		}
	}

	writeBMP();
	freeBMP();
}
#endif //!USE_LIBPNG

void saveIntensityToDisk() {
	FILE* file = fopen("tmp", "wb");
	fwrite(world->data, sizeof(FLOAT_TYPE), world->settings.width * world->settings.height, file);
	fclose(file);
}

void computeIntensities(int width, int height) {
	FLOAT_TYPE scaledX, scaledY;
	FLOAT_TYPE intensity = 0;
	int index = 0;

	for (int x = 0; x < width; x++)
	{
		scaledX = lerp(x, width, X_RANGE) + X_LOWER_BOUND;

		for (int y = 0; y < height; y++)
		{
			scaledY = lerp(y, height, Y_RANGE) + Y_LOWER_BOUND;
			world->data[x * height + y] = ComputeIntensity((struct Complex) { scaledX, scaledY }, world->settings.maxIterations);
		}

		if (x % 100 == 0)
			printf("%d\\%d columns done\n", x, world->settings.width);
	}
}

void computeAndSaveIntensitiesToDisk() {
	world->data = malloc(sizeof(FLOAT_TYPE) * world->settings.width * world->settings.height * 100);
	computeIntensities(world->settings.width, world->settings.height);
	printf("Done sampling\n");
	if (!USE_OPENGL) {
		printf("Saving intensities to disk\n");
		saveIntensityToDisk();
	}
}

void computeColorPalette() {
	FLOAT_TYPE step = 1.0 / world->settings.maxIterations, curr = 0.0;
	realloc(world->dynamicPalette, sizeof(struct Pixel) * world->settings.maxIterations);
	for (int i = 0; i < world->settings.maxIterations; i++)
	{
		world->dynamicPalette[i] = GetPixelColorFromGradient(curr);
		curr += step;
	}
	//testColorPalette();
}

void renderMandelbrot(int width, int height) {
	struct Pixel tmp = { 0,0,0 };
	FLOAT_TYPE currIntensity = 0.0;
	glPointSize(1);
	for (size_t x = 0; x < width; x++)
	{
		glBegin(GL_POINTS);
		for (size_t y = 0; y < height; y++)
		{
			currIntensity = world->data[x * height + y];
			if (isnan(currIntensity) || currIntensity > world->settings.maxIterations )
				currIntensity = world->settings.maxIterations;

			if (USE_DYNAMIC_PALETTE) {
				tmp = GetPixelColorWithFractionalPartFromDynamicPalette(currIntensity, fmod(currIntensity, 1));
			}
			else if (!USE_NORMALIZED_COUNT || USE_WEIRD_NORMALIZED_COUNT)
				tmp = GetPixelColorFromGradient(currIntensity);
			else {
				tmp = GetPixelColorWithFractionalPart(lerp(currIntensity, world->gradientPoints, 1.0f), fmod(currIntensity, 1.0));
			}

			glColor3f((float)lerp(tmp.r, 255, 1.0), (float)lerp(tmp.g, 255, 1.0), (float)lerp(tmp.b, 255, 1.0));
			glVertex2i(x, y);
		}
		glEnd();
	}
}

void computeTranslatedCoordinates(float** buffer, int width, int height) {
	int index = 0;
	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{
			buffer[index] = malloc(sizeof(float) * 2);
			//RANGE -> -1...1
			buffer[index][0] = lerp(x, width, 2.0) - 1;
			buffer[index][1] = lerp(y, height, 2.0) - 1;
			index++;
		}
	}
}

void reDraw(int recompute) {
	if (recompute)
		computeIntensities(world->settings.width, world->settings.height);

	sprintf(world->title, "Mandelbrot - Iterations : %d", world->settings.maxIterations);
	glfwSetWindowTitle(world->window, world->title);
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int shouldRedraw = FALSE;
	int shouldComputePalette = FALSE;
	int shouldRecompute = FALSE;
	if (action == GLFW_PRESS) {
		shouldRedraw = TRUE;
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_UP:
			world->settings.maxIterations += 20;
			shouldRecompute = TRUE;
			break;
		case GLFW_KEY_DOWN:
			world->settings.maxIterations -= 20;
			shouldRecompute = TRUE;
			break;
		case GLFW_KEY_LEFT:
			if (world->settings.currentPaletteIndex > 0)
				world->settings.currentPaletteIndex--;
			else
				world->settings.currentPaletteIndex = NB_PALETTES - 1;
			shouldComputePalette = TRUE;
			break;
		case GLFW_KEY_RIGHT:
			if (world->settings.currentPaletteIndex < NB_PALETTES - 1)
				world->settings.currentPaletteIndex++;
			else
				world->settings.currentPaletteIndex = 0;

			shouldComputePalette = TRUE;
			break;
		case GLFW_KEY_SPACE:
			generatePNG("mabite.png");
			break;
		default:
			shouldRedraw = FALSE;
			break;

		}
	}
	if (shouldComputePalette) {
		world->gradientPoints = world->settings.maxIterations;
		computeColorPalette();
	}
	if (shouldRedraw)
		reDraw(shouldRecompute);
}


void openGLStart() {
	int width, height;
	world->data = malloc(sizeof(FLOAT_TYPE) * world->settings.width * world->settings.height);
	struct GLFWmonitor* primaryScreen = glfwGetPrimaryMonitor();
	struct GLFWvidmode* primarySettings = glfwGetVideoMode(primaryScreen);

	if (primarySettings) {
		width = primarySettings->width;
		height = primarySettings->height;
	}
	else {
		width = WIDTH;
		height = HEIGHT;
	}

	world->gradientPoints = world->settings.maxIterations;
	computeIntensities(world->settings.width, world->settings.height);

	if (!glfwInit())
		return;

	if (width % 2 != 0)
		width -= 1;
	if (height % 2 != 0)
		height -= 1;

	world->window = glfwCreateWindow(width, height, "Mandelbrot", primaryScreen, NULL);
	sprintf(world->title, "Mandelbrot - Iterations : %d", world->settings.maxIterations);
	glfwSetWindowTitle(world->window, world->title);
	if (!world->window) {
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(world->window);
	glfwGetFramebufferSize(world->window, &width, &height);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, world->settings.width, world->settings.height, 0, 1, 0);

	glfwSetKeyCallback(world->window, onKeyPress);

	while (!glfwWindowShouldClose(world->window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		renderMandelbrot(world->settings.width, world->settings.height);

		// Swap front and back buffers*/
		glfwSwapBuffers(world->window);

		// Poll for and process events
		glfwPollEvents();
	}
	glfwTerminate(world->window);
}

void main() {
	double exponent = 3.0;
	world = malloc(sizeof(struct Global));
	world->settings.maxIterations = 0;
	world->settings.width = WIDTH;
	world->settings.height = HEIGHT;
	world->settings.currentPaletteIndex = DEFAULT_PALETTE;
	world->isPaletteInitialized = FALSE;

	if (USE_OPENGL) {
		world->settings.width *= SCALE;
		world->settings.height *= SCALE;
	}
	if (!SINGLE_IMAGE)
		world->settings.maxIterations = pow(2, exponent);
	else
		world->settings.maxIterations = MAX_ITERATIONS;

	printf("Computing color palette\n");
	if (!world->isPaletteInitialized) {
		world->dynamicPalette = malloc(sizeof(struct Pixel) * world->settings.maxIterations);
		world->isPaletteInitialized = TRUE;
	}
	if (USE_DYNAMIC_PALETTE) {
		computeColorPalette(world->settings.maxIterations);
	}

	printf("Starting sampling\n");
	printf("---------------------------------\n");
	printf("| Width\t\t:\t%d\t|\n", world->settings.width);
	printf("| Height\t:\t%d\t|\n", world->settings.height);
	printf("| Iterations\t:\t%d\t|\n", world->settings.maxIterations);
	printf("---------------------------------\n");

	if (USE_OPENGL) {
		openGLStart();
	}
	else {
		computeAndSaveIntensitiesToDisk();

		printf("Done saving\nGenerating image\n");

		char* str = malloc(sizeof(char) * 100);
		sprintf(str, "Result%dx%dx%d.png", world->settings.width, world->settings.height, world->settings.maxIterations);


#if USE_LIBPNG == TRUE
		generatePNG(str);
		if (SINGLE_IMAGE)
			system(str);
#else
		bmp_img_init_df(&img, world->settings.width, world->settings.height);
		generateBMP();
#endif
		free(str);
		printf("Image done\n");
#ifndef SINGLE_IMAGE
		remove("tmp");
		exponent += 1;
	}
#endif
	}
}
/*
#ifdef USE_LIBPNG
	system(str);
#else
	system("test123.bmp");
#endif
	*/
