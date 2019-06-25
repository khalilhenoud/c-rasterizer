#include <windows.h>
#include "platform.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "renderer.h"

static int g_WindowWidth;
static int g_WindowHeight;
static HDC hWindowDC;
static HGLRC hWindowRC;
static float *zbuffer;
static unsigned char *colorbuffer;

/* 6 planes to clip against in homogeneous space. */
static float hnormals[6][4] = {0};

#define FOG_RGB 0x55

#define RenderPixel(x, y, red, green, blue, fog) \
	colorbuffer[(y) * g_WindowWidth * 3 + (x) * 3 + 0] = (unsigned char)(red * fog * 255) + (unsigned char)(FOG_RGB * (1.f - fog));		\
	colorbuffer[(y) * g_WindowWidth * 3 + (x) * 3 + 1] = (unsigned char)(green * fog * 255) + (unsigned char)(FOG_RGB * (1.f - fog));		\
	colorbuffer[(y) * g_WindowWidth * 3 + (x) * 3 + 2] = (unsigned char)(blue * fog * 255) + (unsigned char)(FOG_RGB * (1.f - fog));
	
static __inline long Ceil(float g)
{
	long l = (long)g;	/* Discards fraction. */
	return ((l == g) | (g < 0)) ? l : l + 1;
}

void initializeRenderer(void)
{
	extern HWND g_hWnd;
	int i;

  g_WindowWidth = getWindowWidth();
  g_WindowHeight = getWindowHeight();

	PIXELFORMATDESCRIPTOR kPFD = {0};
	int iPixelFormat;

	hWindowDC = GetDC(g_hWnd);
	kPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	kPFD.nVersion = 1;
	kPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
	kPFD.dwLayerMask = PFD_MAIN_PLANE; 
	kPFD.iPixelType = PFD_TYPE_RGBA;
	kPFD.cColorBits = 32; 
	kPFD.cDepthBits = 24;

	iPixelFormat = ChoosePixelFormat(hWindowDC, &kPFD);
	SetPixelFormat(hWindowDC, iPixelFormat, &kPFD);
	hWindowRC = wglCreateContext(hWindowDC);
	wglMakeCurrent(hWindowDC, hWindowRC);

	/* Creating the Z and color buffers. The farthest value along the z axis is 1.f. */
	zbuffer = (float *)malloc(sizeof(float) * g_WindowHeight * g_WindowWidth);
	for (i = 0; i < g_WindowHeight * g_WindowWidth; ++i)
		zbuffer[i] = 1.f;

	colorbuffer = (unsigned char *)malloc(sizeof(unsigned char) * 3 * g_WindowHeight * g_WindowWidth);
	memset(colorbuffer, 0, sizeof(unsigned char) * 3 * g_WindowHeight * g_WindowWidth);

	/* Filling the homogeneous planes hnormals, x = w plane. */
	hnormals[0][0] = -1; hnormals[0][1] = 0; hnormals[0][2] = 0; hnormals[0][3] = 1;
	/* The x = -w plane. */
	hnormals[1][0] = 1; hnormals[1][1] = 0; hnormals[1][2] = 0; hnormals[1][3] = 1;
	/* The y = w plane. */
	hnormals[2][0] = 0; hnormals[2][1] = -1; hnormals[2][2] = 0; hnormals[2][3] = 1;
	/* The y = -w plane. */
	hnormals[3][0] = 0; hnormals[3][1] = 1; hnormals[3][2] = 0; hnormals[3][3] = 1;
	/* The z = w plane. */
	hnormals[4][0] = 0; hnormals[4][1] = 0; hnormals[4][2] = -1; hnormals[4][3] = 1;
	/* The z = -w plane. */
	hnormals[5][0] = 0; hnormals[5][1] = 0; hnormals[5][2] = 1; hnormals[5][3] = 1;
}

void releaseRenderer(void)
{
	extern HWND g_hWnd;

	free((void *)zbuffer);
	free((void *)colorbuffer);

	wglDeleteContext(hWindowRC);
    ReleaseDC(g_hWnd, hWindowDC);
}

void swapBuffer(void)
{
	glDrawPixels(g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE, (void *)colorbuffer);
	SwapBuffers(hWindowDC);
}

void clearColorAndDepthBuffer(void)
{
	int i;

	/* Clear color buffer. */
	memset(colorbuffer, FOG_RGB, sizeof(unsigned char) * 3 * (g_WindowHeight) * (g_WindowWidth));

	/* Clear z buffer. */
	for (i = 0; i < g_WindowHeight * g_WindowWidth; ++i)
		zbuffer[i] = 1.f;
}

#define FOG()  (z > (1.f - 0.01f) ? ((1.f - z)/0.01f) : 1.f)

void drawTriangle(const float vertices[12], const float colors[9])
{
	int topvtx, midvtx, botvtx, midisleft = 0, leftedge, rightedge;
	float invslope[3], invzslope[3], invcolrslope[3], invcolgslope[3], invcolbslope[3];
	int y, yend, x, xend;
	float xleft, xright, z0, z1, r0, g0, b0, r1, g1, b1;

	/* We are dealing with case 4, 5, 6 consult CS200 for details. */
	if (vertices[0 * 4 + 1] < vertices[1 * 4 + 1]) {
		/* Case 4. */
		if (vertices[2 * 4 + 1] < vertices[0 * 4 + 1]) {
			topvtx = 2;
			midvtx = 0;
			botvtx = 1;
			midisleft = 1;
		} else {
			/* Case 5. */
			if (vertices[1 * 4 + 1] < vertices[2 * 4 + 1]) {
				topvtx = 0;
				midvtx = 1;
				botvtx = 2;
				midisleft = 1;
			} else {	/* Case 6. */
				topvtx = 0;
				midvtx = 2;
				botvtx = 1;
				midisleft = 0;
			}
		}
	} else {	/* Cases 1, 2, 3. */
		/* Case 2. */
		if (vertices[2 * 4 + 1] < vertices[1 * 4 + 1]) {
			topvtx = 2;
			midvtx = 1;
			botvtx = 0;
			midisleft = 0;
		} else {
			topvtx = 1;
			/* Case 3. */
			if (vertices[0 * 4 + 1] < vertices[2 * 4 + 1]) {
				midvtx = 0;
				botvtx = 2;
				midisleft = 0;
			} else {	/* Case 1. */
				midvtx = 2;
				botvtx = 0;
				midisleft = 1;
			}
		}
	}
	
	invslope[0] = (vertices[botvtx * 4 + 0] - vertices[topvtx * 4 + 0])/(vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to bottom. */	
	invslope[1] = (vertices[midvtx * 4 + 0] - vertices[topvtx * 4 + 0])/(vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to middle. */
	invslope[2] = (vertices[botvtx * 4 + 0] - vertices[midvtx * 4 + 0])/(vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);	/* Middle to bottom. */
	invzslope[0] = (vertices[botvtx * 4 + 2] - vertices[topvtx * 4 + 2])/(vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to bottom. */
	invzslope[1] = (vertices[midvtx * 4 + 2] - vertices[topvtx * 4 + 2])/(vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to middle. */
	invzslope[2] = (vertices[botvtx * 4 + 2] - vertices[midvtx * 4 + 2])/(vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);	/* Middle to bottom. */
	invcolrslope[0] = (colors[botvtx * 3 + 0] - colors[topvtx * 3 + 0])/(vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to bottom. */
	invcolrslope[1] = (colors[midvtx * 3 + 0] - colors[topvtx * 3 + 0])/(vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to middle. */
	invcolrslope[2] = (colors[botvtx * 3 + 0] - colors[midvtx * 3 + 0])/(vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);	/* Middle to bottom. */
	invcolgslope[0] = (colors[botvtx * 3 + 1] - colors[topvtx * 3 + 1])/(vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to bottom. */
	invcolgslope[1] = (colors[midvtx * 3 + 1] - colors[topvtx * 3 + 1])/(vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to middle. */
	invcolgslope[2] = (colors[botvtx * 3 + 1] - colors[midvtx * 3 + 1])/(vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);	/* Middle to bottom. */
	invcolbslope[0] = (colors[botvtx * 3 + 2] - colors[topvtx * 3 + 2])/(vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to bottom. */
	invcolbslope[1] = (colors[midvtx * 3 + 2] - colors[topvtx * 3 + 2])/(vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);	/* Top to middle. */
	invcolbslope[2] = (colors[botvtx * 3 + 2] - colors[midvtx * 3 + 2])/(vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);	/* Middle to bottom. */

	leftedge = midisleft;
	rightedge = !midisleft;

	y = Ceil(vertices[topvtx * 4 + 1]);
	yend = Ceil(vertices[midvtx * 4 + 1]) - 1;
	xleft = invslope[leftedge] * ((float)y - vertices[topvtx * 4 + 1]) + vertices[topvtx * 4 + 0];
	xright = invslope[rightedge] * ((float)y - vertices[topvtx * 4 + 1]) + vertices[topvtx * 4 + 0];
	z0 = invzslope[leftedge] * ((float)y - vertices[topvtx * 4 + 1]) + vertices[topvtx * 4 + 2];
	z1 = invzslope[rightedge] * ((float)y - vertices[topvtx * 4 + 1]) + vertices[topvtx * 4 + 2];
	r0 = invcolrslope[leftedge] * ((float)y - vertices[topvtx * 4 + 1]) + colors[topvtx * 3 + 0];
	g0 = invcolgslope[leftedge] * ((float)y - vertices[topvtx * 4 + 1]) + colors[topvtx * 3 + 1];
	b0 = invcolbslope[leftedge] * ((float)y - vertices[topvtx * 4 + 1]) + colors[topvtx * 3 + 2];
	r1 = invcolrslope[rightedge] * ((float)y - vertices[topvtx * 4 + 1]) + colors[topvtx * 3 + 0];
	g1 = invcolgslope[rightedge] * ((float)y - vertices[topvtx * 4 + 1]) + colors[topvtx * 3 + 1];
	b1 = invcolbslope[rightedge] * ((float)y - vertices[topvtx * 4 + 1]) + colors[topvtx * 3 + 2];

	for (; y <= yend; ++y) {
		float dz, dr, dg, db, z = z0, r = r0, g = g0, b = b0;
		x = Ceil(xleft);
		xend = Ceil(xright) - 1;
		dz = (z1 - z0)/(xend - x);
		dr = (r1 - r0)/(xend - x);
		dg = (g1 - g0)/(xend - x);
		db = (b1 - b0)/(xend - x);
    
		for (; x <= xend; ++x) {
			if (z < zbuffer[y * g_WindowWidth + x]) {
				zbuffer[y * g_WindowWidth + x] = z;
				RenderPixel(x, (g_WindowHeight - 1) - y, r, g, b, FOG());
			}
			z += dz;
			r += dr;
			g += dg;
			b += db;
		}

		xleft += invslope[leftedge];
		xright += invslope[rightedge];
		z0 += invzslope[leftedge];
		z1 += invzslope[rightedge];
		r0 += invcolrslope[leftedge];
		r1 += invcolrslope[rightedge];
		g0 += invcolgslope[leftedge];
		g1 += invcolgslope[rightedge];
		b0 += invcolbslope[leftedge];
		b1 += invcolbslope[rightedge];
	}

	leftedge *= 2;
	rightedge *= 2;

	if (midisleft) {
		xleft = invslope[leftedge] * ((float)y - vertices[midvtx * 4 + 1]) + vertices[midvtx * 4 + 0];
		z0 = invzslope[leftedge] * ((float)y - vertices[midvtx * 4 + 1]) + vertices[midvtx * 4 + 2];
		r0 = invcolrslope[leftedge] * ((float)y - vertices[midvtx * 4 + 1]) + colors[midvtx * 3 + 0];
		g0 = invcolgslope[leftedge] * ((float)y - vertices[midvtx * 4 + 1]) + colors[midvtx * 3 + 1];
		b0 = invcolbslope[leftedge] * ((float)y - vertices[midvtx * 4 + 1]) + colors[midvtx * 3 + 2];
	} else {
		xright = invslope[rightedge] * ((float)y - vertices[midvtx * 4 + 1]) + vertices[midvtx * 4 + 0];
		z1 = invzslope[rightedge] * ((float)y - vertices[midvtx * 4 + 1]) + vertices[midvtx * 4 + 2];
		r1 = invcolrslope[rightedge] * ((float)y - vertices[midvtx * 4 + 1]) + colors[midvtx * 3 + 0];
		g1 = invcolgslope[rightedge] * ((float)y - vertices[midvtx * 4 + 1]) + colors[midvtx * 3 + 1];
		b1 = invcolbslope[rightedge] * ((float)y - vertices[midvtx * 4 + 1]) + colors[midvtx * 3 + 2];
	}

	yend = Ceil(vertices[botvtx * 4 + 1]) - 1;

	for (; y <= yend; ++y) {
		float dz, dr, dg, db, z = z0, r = r0, g = g0, b = b0;
		x = Ceil(xleft);
		xend = Ceil(xright) - 1;
		dz = (z1 - z0)/(xend - x);
		dr = (r1 - r0)/(xend - x);
		dg = (g1 - g0)/(xend - x);
		db = (b1 - b0)/(xend - x);
		for (; x <= xend; ++x) {
			if (z < zbuffer[y * g_WindowWidth + x]) {
				zbuffer[y * g_WindowWidth + x] = z;
				RenderPixel(x, (g_WindowHeight - 1) - y, r, g, b, FOG());
			}
			z += dz;
			r += dr;
			g += dg;
			b += db;
		}

		xleft += invslope[leftedge];
		xright += invslope[rightedge];
		z0 += invzslope[leftedge];
		z1 += invzslope[rightedge];
		r0 += invcolrslope[leftedge];
		r1 += invcolrslope[rightedge];
		g0 += invcolgslope[leftedge];
		g1 += invcolgslope[rightedge];
		b0 += invcolbslope[leftedge];
		b1 += invcolbslope[rightedge];
	}
}

void clipPolygonHomogeneousAgainstPlane(float *polygons, float *polycolors, int *drawpolygon, int *polycount, const float *normal)
{
	int i, j, currentcount = *polycount, additionalcount = 0;
	float dotproduct[3], d0, d1, t;
	int back, front, on, index0, index1, index2;
	float newx, newy, newz, neww, newr, newg, newb;
	float newx2, newy2, newz2, neww2, newr2, newg2, newb2;
	float newpolygon[12], newpolycolor[9];

	for (i = 0; i < *polycount; ++i) {
		if (!drawpolygon[i])
			continue;

		/* Get the dot product of the three vertices, if it is positive we are in front else if 
		negative we are to the back and if it is zero with are on the plane. */
		dotproduct[0] = polygons[i * 12 + 0 * 4 + 0] * normal[0] + polygons[i * 12 + 0 * 4 + 1] * normal[1] + polygons[i * 12 + 0 * 4 + 2] * normal[2] + polygons[i * 12 + 0 * 4 + 3] * normal[3];
		dotproduct[1] = polygons[i * 12 + 1 * 4 + 0] * normal[0] + polygons[i * 12 + 1 * 4 + 1] * normal[1] + polygons[i * 12 + 1 * 4 + 2] * normal[2] + polygons[i * 12 + 1 * 4 + 3] * normal[3];
		dotproduct[2] = polygons[i * 12 + 2 * 4 + 0] * normal[0] + polygons[i * 12 + 2 * 4 + 1] * normal[1] + polygons[i * 12 + 2 * 4 + 2] * normal[2] + polygons[i * 12 + 2 * 4 + 3] * normal[3];

		/* Check if we need to clip or is it interior or exterior. */
		back = front = on = 0;

		for (j = 0; j < 3; ++j) {
			if (dotproduct[j] > 0)
				++front;
			else if (dotproduct[j] < 0)
				++back;
			else 
				on++;
		}

		/* If it is interior do nothing else if it is exterior flag it as culled using the 
		drawpolygon[] array. */
		if (front && !back)
			continue;	/* We are to the front or/and have a shared edge. */

		/* If it is to the back or colinear with the clip plane. */
		if ((back && !front) || (!front && !back && on)) {	
			drawpolygon[i] = 0;	/* 0 means do not draw that polygon. */
			continue;
		}

		/* Rearrange the indices in a way that the first index 'index0' points to the first 
		vertex in front of the clip plane (at this point we have a collision). */
		for (j = 0; j < 3; ++j) {
			if (front == 1) {
				if (dotproduct[j] > 0) {
					index0 = j;
					index1 = (index0 + 1) % 3;
					index2 = (index1 + 1) % 3;
					break;
				}
			} else if (front == 2) {
				if (dotproduct[j] > 0 && dotproduct[(j + 1) % 3] <= 0) {
					index0 = j;
					index1 = (index0 + 1) % 3;
					index2 = (index1 + 1) % 3;
					break;
				}
			}
		}
		
		/* If we need to clip then we have three cases, this is case 1 where one vertex lies on 
		the clip plane. */
		if (front == back && back == on) {
			/* Find the edge where the intersection occurs, here we also have 2 cases. Case where 
			the third vertex is coplanar, meaning the first segment interescts the clipping 
			plane. */
			if (dotproduct[index1] < 0)	{	
				/* We need to find the intersection point. */
				d0 = dotproduct[index0];
				d1 = dotproduct[index1];
				t = d0/(d0 - d1);

				newx = polygons[i * 12 + index0 * 4 + 0] + (polygons[i * 12 + index1 * 4 + 0] - polygons[i * 12 + index0 * 4 + 0]) * t;
				newy = polygons[i * 12 + index0 * 4 + 1] + (polygons[i * 12 + index1 * 4 + 1] - polygons[i * 12 + index0 * 4 + 1]) * t;
				newz = polygons[i * 12 + index0 * 4 + 2] + (polygons[i * 12 + index1 * 4 + 2] - polygons[i * 12 + index0 * 4 + 2]) * t;
				neww = polygons[i * 12 + index0 * 4 + 3] + (polygons[i * 12 + index1 * 4 + 3] - polygons[i * 12 + index0 * 4 + 3]) * t;
				newr = polycolors[i * 9 + index0 * 3 + 0] + (polycolors[i * 9 + index1 * 3 + 0] - polycolors[i * 9 + index0 * 3 + 0]) * t;
				newg = polycolors[i * 9 + index0 * 3 + 1] + (polycolors[i * 9 + index1 * 3 + 1] - polycolors[i * 9 + index0 * 3 + 1]) * t;
				newb = polycolors[i * 9 + index0 * 3 + 2] + (polycolors[i * 9 + index1 * 3 + 2] - polycolors[i * 9 + index0 * 3 + 2]) * t;

				/* Replace the old polygon with the new polygon. */
				newpolygon[0 * 4 + 0] = polygons[i * 12 + index0 * 4 + 0];
				newpolygon[0 * 4 + 1] = polygons[i * 12 + index0 * 4 + 1];
				newpolygon[0 * 4 + 2] = polygons[i * 12 + index0 * 4 + 2];
				newpolygon[0 * 4 + 3] = polygons[i * 12 + index0 * 4 + 3];
				newpolygon[1 * 4 + 0] = newx;
				newpolygon[1 * 4 + 1] = newy;
				newpolygon[1 * 4 + 2] = newz;
				newpolygon[1 * 4 + 3] = neww;
				newpolygon[2 * 4 + 0] = polygons[i * 12 + index2 * 4 + 0];
				newpolygon[2 * 4 + 1] = polygons[i * 12 + index2 * 4 + 1];
				newpolygon[2 * 4 + 2] = polygons[i * 12 + index2 * 4 + 2];
				newpolygon[2 * 4 + 3] = polygons[i * 12 + index2 * 4 + 3];
				newpolycolor[0 * 3 + 0] = polycolors[i * 9 + index0 * 3 + 0];
				newpolycolor[0 * 3 + 1] = polycolors[i * 9 + index0 * 3 + 1];
				newpolycolor[0 * 3 + 2] = polycolors[i * 9 + index0 * 3 + 2];
				newpolycolor[1 * 3 + 0] = newr;
				newpolycolor[1 * 3 + 1] = newg;
				newpolycolor[1 * 3 + 2] = newb;
				newpolycolor[2 * 3 + 0] = polycolors[i * 9 + index2 * 3 + 0];
				newpolycolor[2 * 3 + 1] = polycolors[i * 9 + index2 * 3 + 1];
				newpolycolor[2 * 3 + 2] = polycolors[i * 9 + index2 * 3 + 2];

				for (j = 0; j < 12; ++j)
					polygons[i * 12 + j] = newpolygon[j];
				for (j = 0; j < 9; ++j)
					polycolors[i * 9 + j] = newpolycolor[j];
			} else {
				/* Case where the second vertex is coplanar. We need to find the intersection 
				point. */
				d0 = dotproduct[index2];
				d1 = dotproduct[index0];
				t = d0/(d0 - d1);

				newx = polygons[i * 12 + index2 * 4 + 0] + (polygons[i * 12 + index0 * 4 + 0] - polygons[i * 12 + index2 * 4 + 0]) * t;
				newy = polygons[i * 12 + index2 * 4 + 1] + (polygons[i * 12 + index0 * 4 + 1] - polygons[i * 12 + index2 * 4 + 1]) * t;
				newz = polygons[i * 12 + index2 * 4 + 2] + (polygons[i * 12 + index0 * 4 + 2] - polygons[i * 12 + index2 * 4 + 2]) * t;
				neww = polygons[i * 12 + index2 * 4 + 3] + (polygons[i * 12 + index0 * 4 + 3] - polygons[i * 12 + index2 * 4 + 3]) * t;
				newr = polycolors[i * 9 + index2 * 3 + 0] + (polycolors[i * 9 + index0 * 3 + 0] - polycolors[i * 9 + index2 * 3 + 0]) * t;
				newg = polycolors[i * 9 + index2 * 3 + 1] + (polycolors[i * 9 + index0 * 3 + 1] - polycolors[i * 9 + index2 * 3 + 1]) * t;
				newb = polycolors[i * 9 + index2 * 3 + 2] + (polycolors[i * 9 + index0 * 3 + 2] - polycolors[i * 9 + index2 * 3 + 2]) * t;

				/* Replace the old polygon with the new polygon. */
				newpolygon[0 * 4 + 0] = polygons[i * 12 + index0 * 4 + 0];
				newpolygon[0 * 4 + 1] = polygons[i * 12 + index0 * 4 + 1];
				newpolygon[0 * 4 + 2] = polygons[i * 12 + index0 * 4 + 2];
				newpolygon[0 * 4 + 3] = polygons[i * 12 + index0 * 4 + 3];
				newpolygon[1 * 4 + 0] = polygons[i * 12 + index1 * 4 + 0];
				newpolygon[1 * 4 + 1] = polygons[i * 12 + index1 * 4 + 1];
				newpolygon[1 * 4 + 2] = polygons[i * 12 + index1 * 4 + 2];
				newpolygon[1 * 4 + 3] = polygons[i * 12 + index1 * 4 + 3];
				newpolygon[2 * 4 + 0] = newx;
				newpolygon[2 * 4 + 1] = newy;
				newpolygon[2 * 4 + 2] = newz;
				newpolygon[2 * 4 + 3] = neww;
				newpolycolor[0 * 3 + 0] = polycolors[i * 9 + index0 * 3 + 0];
				newpolycolor[0 * 3 + 1] = polycolors[i * 9 + index0 * 3 + 1];
				newpolycolor[0 * 3 + 2] = polycolors[i * 9 + index0 * 3 + 2];
				newpolycolor[1 * 3 + 0] = polycolors[i * 9 + index1 * 3 + 0];
				newpolycolor[1 * 3 + 1] = polycolors[i * 9 + index1 * 3 + 1];
				newpolycolor[1 * 3 + 2] = polycolors[i * 9 + index1 * 3 + 2];
				newpolycolor[2 * 3 + 0] = newr;
				newpolycolor[2 * 3 + 1] = newb;
				newpolycolor[2 * 3 + 2] = newb;

				for (j = 0; j < 12; ++j)
					polygons[i * 12 + j] = newpolygon[j];
				for (j = 0; j < 9; ++j)
					polycolors[i * 9 + j] = newpolycolor[j];
			}
		}

		/* Case 2 where only 1 vertex is to the front of the clip plane (no creation of extra 
		polygon). */
		if (front == 1) {
			/* We need to find the intersection point. */
			d0 = dotproduct[index0];
			d1 = dotproduct[index1];
			t = d0/(d0 - d1);

			newx = polygons[i * 12 + index0 * 4 + 0] + (polygons[i * 12 + index1 * 4 + 0] - polygons[i * 12 + index0 * 4 + 0]) * t;
			newy = polygons[i * 12 + index0 * 4 + 1] + (polygons[i * 12 + index1 * 4 + 1] - polygons[i * 12 + index0 * 4 + 1]) * t;
			newz = polygons[i * 12 + index0 * 4 + 2] + (polygons[i * 12 + index1 * 4 + 2] - polygons[i * 12 + index0 * 4 + 2]) * t;
			neww = polygons[i * 12 + index0 * 4 + 3] + (polygons[i * 12 + index1 * 4 + 3] - polygons[i * 12 + index0 * 4 + 3]) * t;
			newr = polycolors[i * 9 + index0 * 3 + 0] + (polycolors[i * 9 + index1 * 3 + 0] - polycolors[i * 9 + index0 * 3 + 0]) * t;
			newg = polycolors[i * 9 + index0 * 3 + 1] + (polycolors[i * 9 + index1 * 3 + 1] - polycolors[i * 9 + index0 * 3 + 1]) * t;
			newb = polycolors[i * 9 + index0 * 3 + 2] + (polycolors[i * 9 + index1 * 3 + 2] - polycolors[i * 9 + index0 * 3 + 2]) * t;

			d0 = dotproduct[index2];
			d1 = dotproduct[index0];
			t = d0/(d0 - d1);

			newx2 = polygons[i * 12 + index2 * 4 + 0] + (polygons[i * 12 + index0 * 4 + 0] - polygons[i * 12 + index2 * 4 + 0]) * t;
			newy2 = polygons[i * 12 + index2 * 4 + 1] + (polygons[i * 12 + index0 * 4 + 1] - polygons[i * 12 + index2 * 4 + 1]) * t;
			newz2 = polygons[i * 12 + index2 * 4 + 2] + (polygons[i * 12 + index0 * 4 + 2] - polygons[i * 12 + index2 * 4 + 2]) * t;
			neww2 = polygons[i * 12 + index2 * 4 + 3] + (polygons[i * 12 + index0 * 4 + 3] - polygons[i * 12 + index2 * 4 + 3]) * t;
			newr2 = polycolors[i * 9 + index2 * 3 + 0] + (polycolors[i * 9 + index0 * 3 + 0] - polycolors[i * 9 + index2 * 3 + 0]) * t;
			newg2 = polycolors[i * 9 + index2 * 3 + 1] + (polycolors[i * 9 + index0 * 3 + 1] - polycolors[i * 9 + index2 * 3 + 1]) * t;
			newb2 = polycolors[i * 9 + index2 * 3 + 2] + (polycolors[i * 9 + index0 * 3 + 2] - polycolors[i * 9 + index2 * 3 + 2]) * t;

			/* Replace the old polygon with the new polygon. */
			newpolygon[0 * 4 + 0] = polygons[i * 12 + index0 * 4 + 0];
			newpolygon[0 * 4 + 1] = polygons[i * 12 + index0 * 4 + 1];
			newpolygon[0 * 4 + 2] = polygons[i * 12 + index0 * 4 + 2];
			newpolygon[0 * 4 + 3] = polygons[i * 12 + index0 * 4 + 3];
			newpolygon[1 * 4 + 0] = newx;
			newpolygon[1 * 4 + 1] = newy;
			newpolygon[1 * 4 + 2] = newz;
			newpolygon[1 * 4 + 3] = neww;
			newpolygon[2 * 4 + 0] = newx2;
			newpolygon[2 * 4 + 1] = newy2;
			newpolygon[2 * 4 + 2] = newz2;
			newpolygon[2 * 4 + 3] = neww2;
			newpolycolor[0 * 3 + 0] = polycolors[i * 9 + index0 * 3 + 0];
			newpolycolor[0 * 3 + 1] = polycolors[i * 9 + index0 * 3 + 1];
			newpolycolor[0 * 3 + 2] = polycolors[i * 9 + index0 * 3 + 2];
			newpolycolor[1 * 3 + 0] = newr;
			newpolycolor[1 * 3 + 1] = newg;
			newpolycolor[1 * 3 + 2] = newb;
			newpolycolor[2 * 3 + 0] = newr2;
			newpolycolor[2 * 3 + 1] = newg2;
			newpolycolor[2 * 3 + 2] = newb2;

			for (j = 0; j < 12; ++j)
				polygons[i * 12 + j] = newpolygon[j];
			for (j = 0; j < 9; ++j)
				polycolors[i * 9 + j] = newpolycolor[j];
		}

		/* Case 3 where 2 vertices are to the front, an extra polygon is created in this case 
		and it is stored in the polygons array with a convenient index. */
		if (front == 2) {
			/* We need to find the intersection point. */
			d0 = dotproduct[index0];
			d1 = dotproduct[index1];
			t = d0/(d0 - d1);

			newx = polygons[i * 12 + index0 * 4 + 0] + (polygons[i * 12 + index1 * 4 + 0] - polygons[i * 12 + index0 * 4 + 0]) * t;
			newy = polygons[i * 12 + index0 * 4 + 1] + (polygons[i * 12 + index1 * 4 + 1] - polygons[i * 12 + index0 * 4 + 1]) * t;
			newz = polygons[i * 12 + index0 * 4 + 2] + (polygons[i * 12 + index1 * 4 + 2] - polygons[i * 12 + index0 * 4 + 2]) * t;
			neww = polygons[i * 12 + index0 * 4 + 3] + (polygons[i * 12 + index1 * 4 + 3] - polygons[i * 12 + index0 * 4 + 3]) * t;
			newr = polycolors[i * 9 + index0 * 3 + 0] + (polycolors[i * 9 + index1 * 3 + 0] - polycolors[i * 9 + index0 * 3 + 0]) * t;
			newg = polycolors[i * 9 + index0 * 3 + 1] + (polycolors[i * 9 + index1 * 3 + 1] - polycolors[i * 9 + index0 * 3 + 1]) * t;
			newb = polycolors[i * 9 + index0 * 3 + 2] + (polycolors[i * 9 + index1 * 3 + 2] - polycolors[i * 9 + index0 * 3 + 2]) * t;

			d0 = dotproduct[index1];
			d1 = dotproduct[index2];
			t = d0/(d0 - d1);

			newx2 = polygons[i * 12 + index1 * 4 + 0] + (polygons[i * 12 + index2 * 4 + 0] - polygons[i * 12 + index1 * 4 + 0]) * t;
			newy2 = polygons[i * 12 + index1 * 4 + 1] + (polygons[i * 12 + index2 * 4 + 1] - polygons[i * 12 + index1 * 4 + 1]) * t;
			newz2 = polygons[i * 12 + index1 * 4 + 2] + (polygons[i * 12 + index2 * 4 + 2] - polygons[i * 12 + index1 * 4 + 2]) * t;
			neww2 = polygons[i * 12 + index1 * 4 + 3] + (polygons[i * 12 + index2 * 4 + 3] - polygons[i * 12 + index1 * 4 + 3]) * t;
			newr2 = polycolors[i * 9 + index1 * 3 + 0] + (polycolors[i * 9 + index2 * 3 + 0] - polycolors[i * 9 + index1 * 3 + 0]) * t;
			newg2 = polycolors[i * 9 + index1 * 3 + 1] + (polycolors[i * 9 + index2 * 3 + 1] - polycolors[i * 9 + index1 * 3 + 1]) * t;
			newb2 = polycolors[i * 9 + index1 * 3 + 2] + (polycolors[i * 9 + index2 * 3 + 2] - polycolors[i * 9 + index1 * 3 + 2]) * t;

			/* Create and save the new polygon. */
			newpolygon[0 * 4 + 0] = polygons[i * 12 + index2 * 4 + 0];
			newpolygon[0 * 4 + 1] = polygons[i * 12 + index2 * 4 + 1];
			newpolygon[0 * 4 + 2] = polygons[i * 12 + index2 * 4 + 2];
			newpolygon[0 * 4 + 3] = polygons[i * 12 + index2 * 4 + 3];
			newpolygon[1 * 4 + 0] = newx;
			newpolygon[1 * 4 + 1] = newy;
			newpolygon[1 * 4 + 2] = newz;
			newpolygon[1 * 4 + 3] = neww;
			newpolygon[2 * 4 + 0] = newx2;
			newpolygon[2 * 4 + 1] = newy2;
			newpolygon[2 * 4 + 2] = newz2;
			newpolygon[2 * 4 + 3] = neww2;
			newpolycolor[0 * 3 + 0] = polycolors[i * 9 + index0 * 3 + 0];
			newpolycolor[0 * 3 + 1] = polycolors[i * 9 + index0 * 3 + 1];
			newpolycolor[0 * 3 + 2] = polycolors[i * 9 + index0 * 3 + 2];
			newpolycolor[1 * 3 + 0] = newr;
			newpolycolor[1 * 3 + 1] = newg;
			newpolycolor[1 * 3 + 2] = newb;
			newpolycolor[2 * 3 + 0] = newr2;
			newpolycolor[2 * 3 + 1] = newg2;
			newpolycolor[2 * 3 + 2] = newb2;

			for (j = 0; j < 12; ++j)
				polygons[currentcount * 12 + j] = newpolygon[j];
			for (j = 0; j < 9; ++j)
				polycolors[currentcount * 9 + j] = newpolycolor[j];
			drawpolygon[currentcount] = 1;
			++currentcount;
			++additionalcount;

			/* Replace the old polygon with the new polygon. */
			newpolygon[0 * 4 + 0] = polygons[i * 12 + index0 * 4 + 0];
			newpolygon[0 * 4 + 1] = polygons[i * 12 + index0 * 4 + 1];
			newpolygon[0 * 4 + 2] = polygons[i * 12 + index0 * 4 + 2];
			newpolygon[0 * 4 + 3] = polygons[i * 12 + index0 * 4 + 3];
			newpolygon[1 * 4 + 0] = newx;
			newpolygon[1 * 4 + 1] = newy;
			newpolygon[1 * 4 + 2] = newz;
			newpolygon[1 * 4 + 3] = neww;
			newpolygon[2 * 4 + 0] = polygons[i * 12 + index2 * 4 + 0];
			newpolygon[2 * 4 + 1] = polygons[i * 12 + index2 * 4 + 1];
			newpolygon[2 * 4 + 2] = polygons[i * 12 + index2 * 4 + 2];
			newpolygon[2 * 4 + 3] = polygons[i * 12 + index2 * 4 + 3];
			newpolycolor[0 * 3 + 0] = polycolors[i * 9 + index0 * 3 + 0];
			newpolycolor[0 * 3 + 1] = polycolors[i * 9 + index0 * 3 + 1];
			newpolycolor[0 * 3 + 2] = polycolors[i * 9 + index0 * 3 + 2];
			newpolycolor[1 * 3 + 0] = newr;
			newpolycolor[1 * 3 + 1] = newg;
			newpolycolor[1 * 3 + 2] = newb;
			newpolycolor[2 * 3 + 0] = polycolors[i * 9 + index2 * 3 + 0];
			newpolycolor[2 * 3 + 1] = polycolors[i * 9 + index2 * 3 + 1];
			newpolycolor[2 * 3 + 2] = polycolors[i * 9 + index2 * 3 + 2];

			for (j = 0; j < 12; ++j)
				polygons[i * 12 + j] = newpolygon[j];
			for (j = 0; j < 9; ++j)
				polycolors[i * 9 + j] = newpolycolor[j];
		}
	}

	*polycount += additionalcount;
}

void clipPolygonHomogeneousAgainstAllPlanes(float *polygons, float *polycolors, int *drawpolygon, int *polycount)
{
	clipPolygonHomogeneousAgainstPlane(polygons, polycolors, drawpolygon, polycount, hnormals[0]);
	clipPolygonHomogeneousAgainstPlane(polygons, polycolors, drawpolygon, polycount, hnormals[1]);
	clipPolygonHomogeneousAgainstPlane(polygons, polycolors, drawpolygon, polycount, hnormals[2]);
	clipPolygonHomogeneousAgainstPlane(polygons, polycolors, drawpolygon, polycount, hnormals[3]);
	clipPolygonHomogeneousAgainstPlane(polygons, polycolors, drawpolygon, polycount, hnormals[4]);
	clipPolygonHomogeneousAgainstPlane(polygons, polycolors, drawpolygon, polycount, hnormals[5]);
}