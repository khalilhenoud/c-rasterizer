#include <assert.h>
#include "pipeline.h"

#define MODELVIEWSTACKSIZE 256
#define PROJECTIONSTACKSIZE 10

static int stackmode = MODELVIEW;
static Matrix4D modelviewstack[MODELVIEWSTACKSIZE] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
static int modelviewindex = 0;
static Matrix4D projectionstack[PROJECTIONSTACKSIZE] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
static int projectionindex = 0;

static float viewportheightminus1 = 0.f;
static float viewportxorigin = 0.f;
static float viewportyorigin = 0.f;
static float viewportheight = 0.f;
static float viewportwidth = 0.f;

void getViewportMatrix(Matrix4D output)
{
  // First we need to scale the x and y coordinates (which were -1 < x < 1, -1 < y < 1) to the 
	// screen width/2 and height/2, then move the values into the 0 to width and 0 to height range 
	// by adding half the width and height, -width/2 < x < width/2 -height/2 < y < height/2 which 
	// now becomes 0 < x < width, 0 < y < height.
	Matrix4D m1, m2, m3;
	setIdentity(m1);
	m1[0] = (viewportwidth - 1)/2;
	m1[5] = (viewportheight - 1)/2;
	m1[12] = (viewportwidth - 1)/2;
	m1[13] = (viewportheight - 1)/2;

	// Since screen space y is downward we need to flip the image on the y and then translate it 
	// downward to produce positive values.
	setIdentity(m2);
	m2[5] = -1;
	m2[13] = viewportheightminus1;

	// Offset translating all points to the starting location of the new viewport.
	setIdentity(m3);
	m3[12] = viewportxorigin;
	m3[13] = viewportyorigin;

	multiplyMatrices(m3, m2, m3);
	multiplyMatrices(m3, m1, m3);
	copyMatrix(m3, output);
}

void setViewport(float x, float y, float width, float height)
{
	viewportxorigin = x;
	viewportyorigin = y;
	viewportheight = height;
	viewportwidth = width;
	viewportheightminus1 = height - 1;
}

void frustrum(float right, float left, float bottom, float top, float near, float far)
{
	Matrix4D projection;
	float A, B, C, D, E, F, G;
	A = (2 * near)/(right - left);
	B = (2 * near)/(top - bottom);
	C = (right + left)/(right - left);
	D = (top + bottom)/(top - bottom);
	E = - (far + near)/(far - near);
	F = - (2 * far * near)/(far - near);
	G = -1;

	setIdentity(projection);
	projection[0] = A;
	projection[5] = B;
	projection[8] = C;
	projection[9] = D;
	projection[10] = E;
	projection[11] = G;
	projection[14] = F;

	postMultiply(projection);
}

void getMatrix(Matrix4D output)
{
	if (stackmode == MODELVIEW)
		copyMatrix(modelviewstack[modelviewindex], output);
	else
		copyMatrix(projectionstack[projectionindex], output);
}

void setMatrixMode(int mode)
{
	stackmode = mode;
}

void pushMatrix(void)
{
	if (stackmode == MODELVIEW) {
		assert(modelviewindex + 1 < MODELVIEWSTACKSIZE);
		copyMatrix(modelviewstack[modelviewindex], modelviewstack[modelviewindex + 1]);
		++modelviewindex;
	} else {
		assert(projectionindex + 1 < PROJECTIONSTACKSIZE);
		copyMatrix(projectionstack[projectionindex], projectionstack[projectionindex + 1]);
		++projectionindex;
	}
}

void popMatrix(Matrix4D output)
{
	if (stackmode == MODELVIEW) {
		assert(modelviewindex > 0);
		copyMatrix(modelviewstack[modelviewindex--], output);
	} else {
		assert(projectionindex > 0);
		copyMatrix(projectionstack[projectionindex--], output);
	}
}

void loadIdentity(void)
{
	if (stackmode == MODELVIEW)
		setIdentity(modelviewstack[modelviewindex]);
	else
		setIdentity(projectionstack[projectionindex]);
}

void replace(Matrix4D matrix)
{
	if (stackmode == MODELVIEW)
		copyMatrix(matrix, modelviewstack[modelviewindex]);
	else 
		copyMatrix(matrix, projectionstack[projectionindex]);
}

void postRotateX(float angle)
{
	Matrix4D matrix;
	rotationMatrixX(matrix, angle);
	
	postMultiply(matrix);
}

void postRotateY(float angle)
{
	Matrix4D matrix;
	rotationMatrixY(matrix, angle);
	
	postMultiply(matrix);
}

void postRotateZ(float angle)
{
	Matrix4D matrix;
	rotationMatrixZ(matrix, angle);
	
	postMultiply(matrix);
}

void postTranslate(float x, float y, float z)
{
	Matrix4D matrix;
	translationMatrix(matrix, x, y, z);

	postMultiply(matrix);
}

void postMultiply(Matrix4D matrix)
{
	if (stackmode == MODELVIEW)
		multiplyMatrices(matrix, modelviewstack[modelviewindex], modelviewstack[modelviewindex]);
	else
		multiplyMatrices(matrix, projectionstack[projectionindex], projectionstack[projectionindex]);
}

void preRotateX(float angle)
{
	Matrix4D matrix;
	rotationMatrixX(matrix, angle);

	preMultiply(matrix);
}

void preRotateY(float angle)
{
	Matrix4D matrix;
	rotationMatrixY(matrix, angle);

	preMultiply(matrix);
}

void preRotateZ(float angle)
{
	Matrix4D matrix;
	rotationMatrixZ(matrix, angle);

	preMultiply(matrix);
}

void preTranslate(float x, float y, float z)
{
	Matrix4D matrix;
	translationMatrix(matrix, x, y, z);

	preMultiply(matrix);
}

void preMultiply(Matrix4D matrix)
{
	if (stackmode == MODELVIEW)
		multiplyMatrices(modelviewstack[modelviewindex], matrix, modelviewstack[modelviewindex]);
	else
		multiplyMatrices(projectionstack[projectionindex], matrix, projectionstack[projectionindex]);
}