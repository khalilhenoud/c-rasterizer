#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "platform.h"
#include "matrix.h"
#include "pipeline.h"

static int g_WindowWidth;
static int g_WindowHeight;
static float *zbuffer;
static unsigned char *colorbuffer;

/// Homogeneous space clipping planes.
static float hnormals[6][4] = {0};

#define FOG_RGB 0x55

#define FOG()  (z > (1.f - 0.01f) ? ((1.f - z)/0.01f) : 1.f)

#define RenderPixel(x, y, red, green, blue, fog) \
  colorbuffer[(y) * g_WindowWidth * 3 + (x) * 3 + 0] = (unsigned char)(red * fog * 255) + (unsigned char)(FOG_RGB * (1.f - fog));    \
  colorbuffer[(y) * g_WindowWidth * 3 + (x) * 3 + 1] = (unsigned char)(green * fog * 255) + (unsigned char)(FOG_RGB * (1.f - fog));    \
  colorbuffer[(y) * g_WindowWidth * 3 + (x) * 3 + 2] = (unsigned char)(blue * fog * 255) + (unsigned char)(FOG_RGB * (1.f - fog));
  
static __inline long Ceil(float g)
{
  long l = (long)g;  // Discards fraction.
  return ((l == g) | (g < 0)) ? l : l + 1;
}

void setupRenderer(int width, int height)
{
  int i;
  g_WindowWidth = width;
  g_WindowHeight = height;

  // Creating the Z and color buffers. Far z is 1.f, near is 0.f.
  zbuffer = (float *)malloc(sizeof(float) * g_WindowHeight * g_WindowWidth);
  for (i = 0; i < g_WindowHeight * g_WindowWidth; ++i)
    zbuffer[i] = 1.f;

  colorbuffer = (unsigned char *)malloc(sizeof(unsigned char) * 3 * g_WindowHeight * g_WindowWidth);
  memset(colorbuffer, 0, sizeof(unsigned char) * 3 * g_WindowHeight * g_WindowWidth);

  // Initialize homogenous planes.
  hnormals[0][0] = -1;  hnormals[0][1] = 0;   hnormals[0][2] = 0;   hnormals[0][3] = 1;   // x = w plane.
  hnormals[1][0] = 1;   hnormals[1][1] = 0;   hnormals[1][2] = 0;   hnormals[1][3] = 1;   // x = -w plane.
  hnormals[2][0] = 0;   hnormals[2][1] = -1;  hnormals[2][2] = 0;   hnormals[2][3] = 1;   // y = w plane.
  hnormals[3][0] = 0;   hnormals[3][1] = 1;   hnormals[3][2] = 0;   hnormals[3][3] = 1;   // y = -w plane.
  hnormals[4][0] = 0;   hnormals[4][1] = 0;   hnormals[4][2] = -1;  hnormals[4][3] = 1;   // z = w plane.
  hnormals[5][0] = 0;   hnormals[5][1] = 0;   hnormals[5][2] = 1;   hnormals[5][3] = 1;   // z = -w plane.
}

void releaseRenderer(void)
{
  free((void *)zbuffer);
  free((void *)colorbuffer);
}

void swapBuffer(void)
{
  blitAndSwap(colorbuffer);
}

void clearColorAndDepthBuffer(void)
{
  int i;
  for (i = 0; i < g_WindowHeight * g_WindowWidth; ++i)
    zbuffer[i] = 1.f;

  memset(colorbuffer, FOG_RGB, sizeof(unsigned char) * 3 * (g_WindowHeight) * (g_WindowWidth));
}

void clipPolygonHomogeneousAgainstPlane(float *polygons, float *polycolors, int *drawpolygon, int *polycount, const float *normal)
{
  int i, j, k, currentcount = *polycount, additionalcount = 0;
  float dotproduct[3], t;
  int back, front, on, index0, index1, index2;
  float newpolygon[12], newpolycolor[9];

  for (i = 0; i < *polycount; ++i) {
    if (!drawpolygon[i])
      continue;

    // if dot > 0 vertex is to the front, dot < 0 to the back, dot == 0 indicates planar vertex.
    for (k = 0; k < 3; ++k)
      dotproduct[k] = polygons[i * 12 + k * 4 + 0] * normal[0] + polygons[i * 12 + k * 4 + 1] * normal[1] + polygons[i * 12 + k * 4 + 2] * normal[2] + polygons[i * 12 + k * 4 + 3] * normal[3];

    // Check if we need to clip.
    back = front = on = 0;

    for (j = 0; j < 3; ++j) {
      if (dotproduct[j] > 0)
        ++front;
      else if (dotproduct[j] < 0)
        ++back;
      else
        on++;
    }

    // Triangle to the front or/and have a shared edge.
    if (front && !back)
      continue;

    // If strictly to the back or colinear, skip and don't draw.
    if ((back && !front) || (!front && !back && on)) {
      drawpolygon[i] = 0;
      continue;
    }

    // Intersection: rearrange indices so that vertex[index0] is tot he front of the clip plane.
    for (j = 0; j < 3; ++j) {
      if (front == 1) {
        if (dotproduct[j] > 0) {
          index0 = j;
          index1 = (index0 + 1) % 3;
          index2 = (index1 + 1) % 3;
          break;
        }
      }
      else if (front == 2) {
        if (dotproduct[j] > 0 && dotproduct[(j + 1) % 3] <= 0) {
          index0 = j;
          index1 = (index0 + 1) % 3;
          index2 = (index1 + 1) % 3;
          break;
        }
      }
    }

    // Case 1: one vertex is on the clip plane.
    if (front == back && back == on) {
      // Find the edge that crosses the clipping plane (2 cases). Calculate intersection point.
      if (dotproduct[index1] < 0) {
        t = dotproduct[index0] / (dotproduct[index0] - dotproduct[index1]);

        // Replace the old polygon with the new clipped one polygon.
        for (k = 0; k < 4; ++k) {
          newpolygon[0 * 4 + k] = polygons[i * 12 + index0 * 4 + k];
          newpolygon[2 * 4 + k] = polygons[i * 12 + index2 * 4 + k];
          newpolygon[1 * 4 + k] = polygons[i * 12 + index0 * 4 + k] + (polygons[i * 12 + index1 * 4 + k] - polygons[i * 12 + index0 * 4 + k]) * t;
        }

        for (k = 0; k < 3; ++k) {
          newpolycolor[0 * 3 + k] = polycolors[i * 9 + index0 * 3 + k];
          newpolycolor[2 * 3 + k] = polycolors[i * 9 + index2 * 3 + k];
          newpolycolor[1 * 3 + k] = polycolors[i * 9 + index0 * 3 + k] + (polycolors[i * 9 + index1 * 3 + k] - polycolors[i * 9 + index0 * 3 + k]) * t;
        }

        for (j = 0; j < 12; ++j)
          polygons[i * 12 + j] = newpolygon[j];
        for (j = 0; j < 9; ++j)
          polycolors[i * 9 + j] = newpolycolor[j];
      }
      else {
        t = dotproduct[index2] / (dotproduct[index2] - dotproduct[index0]);

        // Replace the old polygon with the new clipped one polygon.
        for (k = 0; k < 4; ++k) {
          newpolygon[0 * 4 + k] = polygons[i * 12 + index0 * 4 + k];
          newpolygon[1 * 4 + k] = polygons[i * 12 + index1 * 4 + k];
          newpolygon[2 * 4 + k] = polygons[i * 12 + index2 * 4 + k] + (polygons[i * 12 + index0 * 4 + k] - polygons[i * 12 + index2 * 4 + k]) * t;
        }

        for (k = 0; k < 3; ++k) {
          newpolycolor[0 * 3 + k] = polycolors[i * 9 + index0 * 3 + k];
          newpolycolor[1 * 3 + k] = polycolors[i * 9 + index1 * 3 + k];
          newpolycolor[2 * 3 + k] = polycolors[i * 9 + index2 * 3 + k] + (polycolors[i * 9 + index0 * 3 + k] - polycolors[i * 9 + index2 * 3 + k]) * t;
        }

        for (j = 0; j < 12; ++j)
          polygons[i * 12 + j] = newpolygon[j];
        for (j = 0; j < 9; ++j)
          polycolors[i * 9 + j] = newpolycolor[j];
      }
    }

    // Case 2: 1 vertex is to the front (no extra triangle is created).
    if (front == 1) {
      t = dotproduct[index0] / (dotproduct[index0] - dotproduct[index1]);

      for (k = 0; k < 4; ++k)
        newpolygon[1 * 4 + k] = polygons[i * 12 + index0 * 4 + k] + (polygons[i * 12 + index1 * 4 + k] - polygons[i * 12 + index0 * 4 + k]) * t;

      for (k = 0; k < 3; ++k)
        newpolycolor[1 * 3 + k] = polycolors[i * 9 + index0 * 3 + k] + (polycolors[i * 9 + index1 * 3 + k] - polycolors[i * 9 + index0 * 3 + k]) * t;

      t = dotproduct[index2] / (dotproduct[index2] - dotproduct[index0]);

      for (k = 0; k < 4; ++k)
        newpolygon[2 * 4 + k] = polygons[i * 12 + index2 * 4 + k] + (polygons[i * 12 + index0 * 4 + k] - polygons[i * 12 + index2 * 4 + k]) * t;

      for (k = 0; k < 3; ++k)
        newpolycolor[2 * 3 + k] = polycolors[i * 9 + index2 * 3 + k] + (polycolors[i * 9 + index0 * 3 + k] - polycolors[i * 9 + index2 * 3 + k]) * t;

      for (k = 0; k < 4; ++k)
        newpolygon[0 * 4 + k] = polygons[i * 12 + index0 * 4 + k];

      for (k = 0; k < 3; ++k)
        newpolycolor[0 * 3 + k] = polycolors[i * 9 + index0 * 3 + k];

      for (j = 0; j < 12; ++j)
        polygons[i * 12 + j] = newpolygon[j];
      for (j = 0; j < 9; ++j)
        polycolors[i * 9 + j] = newpolycolor[j];
    }

    // Case 3: 2 vertices are to the front. An extra polygon needs to be created.
    if (front == 2) {
      float tmppos[4], tmpclr[3];
      t = dotproduct[index0] / (dotproduct[index0] - dotproduct[index1]);

      for (k = 0; k < 4; ++k)
        newpolygon[1 * 4 + k] = tmppos[k] = polygons[i * 12 + index0 * 4 + k] + (polygons[i * 12 + index1 * 4 + k] - polygons[i * 12 + index0 * 4 + k]) * t;

      for (k = 0; k < 3; ++k)
        newpolycolor[1 * 3 + k] = tmpclr[k] = polycolors[i * 9 + index0 * 3 + k] + (polycolors[i * 9 + index1 * 3 + k] - polycolors[i * 9 + index0 * 3 + k]) * t;

      t = dotproduct[index1] / (dotproduct[index1] - dotproduct[index2]);

      // Create and save the new polygon.
      for (k = 0; k < 4; ++k)
        newpolygon[2 * 4 + k] = polygons[i * 12 + index1 * 4 + k] + (polygons[i * 12 + index2 * 4 + k] - polygons[i * 12 + index1 * 4 + k]) * t;

      for (k = 0; k < 3; ++k)
        newpolycolor[2 * 3 + k] = polycolors[i * 9 + index1 * 3 + k] + (polycolors[i * 9 + index2 * 3 + k] - polycolors[i * 9 + index1 * 3 + k]) * t;

      for (k = 0; k < 4; ++k)
        newpolygon[0 * 4 + k] = polygons[i * 12 + index2 * 4 + k];

      for (k = 0; k < 3; ++k)
        newpolycolor[0 * 3 + k] = polycolors[i * 9 + index0 * 3 + k];

      for (j = 0; j < 12; ++j)
        polygons[currentcount * 12 + j] = newpolygon[j];
      for (j = 0; j < 9; ++j)
        polycolors[currentcount * 9 + j] = newpolycolor[j];
      drawpolygon[currentcount] = 1;
      ++currentcount;
      ++additionalcount;

      // Replace the old polygon with the new polygon.
      for (k = 0; k < 4; ++k) {
        newpolygon[0 * 4 + k] = polygons[i * 12 + index0 * 4 + k];
        newpolygon[2 * 4 + k] = polygons[i * 12 + index2 * 4 + k];
        newpolygon[1 * 4 + k] = tmppos[k];
      }

      for (k = 0; k < 3; ++k) {
        newpolycolor[0 * 3 + k] = polycolors[i * 9 + index0 * 3 + k];
        newpolycolor[2 * 3 + k] = polycolors[i * 9 + index2 * 3 + k];
        newpolycolor[1 * 3 + k] = tmpclr[k];
      }

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

void drawTriangle(const float vertices[12], const float colors[9])
{
  int topvtx, midvtx, botvtx, midisleft = 0, leftedge, rightedge;
  float invslope[3], invzslope[3], invcolrslope[3], invcolgslope[3], invcolbslope[3];
  int y, yend, x, xend;
  float xleft, xright, z0, z1, r0, g0, b0, r1, g1, b1;

  // We are dealing with case 4, 5, 6 consult CS200 for details
  if (vertices[0 * 4 + 1] < vertices[1 * 4 + 1]) {
    // Case 4.
    if (vertices[2 * 4 + 1] < vertices[0 * 4 + 1]) {
      topvtx = 2;
      midvtx = 0;
      botvtx = 1;
      midisleft = 1;
    }
    else {
      // Case 5.
      if (vertices[1 * 4 + 1] < vertices[2 * 4 + 1]) {
        topvtx = 0;
        midvtx = 1;
        botvtx = 2;
        midisleft = 1;
      }
      else {  // Case 6.
        topvtx = 0;
        midvtx = 2;
        botvtx = 1;
        midisleft = 0;
      }
    }
  }
  else {  // Cases 1, 2, 3.
    // Case 2:
    if (vertices[2 * 4 + 1] < vertices[1 * 4 + 1]) {
      topvtx = 2;
      midvtx = 1;
      botvtx = 0;
      midisleft = 0;
    }
    else {
      topvtx = 1;
      // Case 3.
      if (vertices[0 * 4 + 1] < vertices[2 * 4 + 1]) {
        midvtx = 0;
        botvtx = 2;
        midisleft = 0;
      }
      else {  // Case 1.
        midvtx = 2;
        botvtx = 0;
        midisleft = 1;
      }
    }
  }

  invslope[0] = (vertices[botvtx * 4 + 0] - vertices[topvtx * 4 + 0]) / (vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to bottom. 
  invslope[1] = (vertices[midvtx * 4 + 0] - vertices[topvtx * 4 + 0]) / (vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to middle. 
  invslope[2] = (vertices[botvtx * 4 + 0] - vertices[midvtx * 4 + 0]) / (vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);  // Middle to bottom. 
  invzslope[0] = (vertices[botvtx * 4 + 2] - vertices[topvtx * 4 + 2]) / (vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]); // Top to bottom. 
  invzslope[1] = (vertices[midvtx * 4 + 2] - vertices[topvtx * 4 + 2]) / (vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]); // Top to middle. 
  invzslope[2] = (vertices[botvtx * 4 + 2] - vertices[midvtx * 4 + 2]) / (vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]); // Middle to bottom. 
  invcolrslope[0] = (colors[botvtx * 3 + 0] - colors[topvtx * 3 + 0]) / (vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to bottom. 
  invcolrslope[1] = (colors[midvtx * 3 + 0] - colors[topvtx * 3 + 0]) / (vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to middle. 
  invcolrslope[2] = (colors[botvtx * 3 + 0] - colors[midvtx * 3 + 0]) / (vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);  // Middle to bottom. 
  invcolgslope[0] = (colors[botvtx * 3 + 1] - colors[topvtx * 3 + 1]) / (vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to bottom. 
  invcolgslope[1] = (colors[midvtx * 3 + 1] - colors[topvtx * 3 + 1]) / (vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to middle. 
  invcolgslope[2] = (colors[botvtx * 3 + 1] - colors[midvtx * 3 + 1]) / (vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);  // Middle to bottom. 
  invcolbslope[0] = (colors[botvtx * 3 + 2] - colors[topvtx * 3 + 2]) / (vertices[botvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to bottom. 
  invcolbslope[1] = (colors[midvtx * 3 + 2] - colors[topvtx * 3 + 2]) / (vertices[midvtx * 4 + 1] - vertices[topvtx * 4 + 1]);  // Top to middle. 
  invcolbslope[2] = (colors[botvtx * 3 + 2] - colors[midvtx * 3 + 2]) / (vertices[botvtx * 4 + 1] - vertices[midvtx * 4 + 1]);  // Middle to bottom. 

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
    dz = (z1 - z0) / (xend - x);
    dr = (r1 - r0) / (xend - x);
    dg = (g1 - g0) / (xend - x);
    db = (b1 - b0) / (xend - x);

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
  }
  else {
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
    dz = (z1 - z0) / (xend - x);
    dr = (r1 - r0) / (xend - x);
    dg = (g1 - g0) / (xend - x);
    db = (b1 - b0) / (xend - x);
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

/**
 * 64 triangles with 3 vertices each and each vertex is made of 4 components. These structures
 * are needed with homogeneous polygon clipping as each triangle can (in the worst case) turn
 * into 64 triangles when clipped with the frustrum planes.
 */
static float polygons[64 * 3 * 4];
static float polycolors[64 * 3 * 3];
static int drawpolygon[64];
static int polycount;

void render(const float *vertices, const float *colors, int vertexNumber, const int *indices, int indexNumber)
{
  Matrix4D viewport, projection, modelviewtransformation, modelviewprojection;
  int i, j, k, m;
  float fvertices[3 * 4];
  float fcolors[3 * 3];

  getViewportMatrix(viewport);
  setMatrixMode(PROJECTION);
  getMatrix(projection);
  setMatrixMode(MODELVIEW);
  getMatrix(modelviewtransformation);

  multiplyMatrices(projection, modelviewtransformation, modelviewprojection);

  for (i = 0; i < indexNumber; i += 3) {
    // Copy triangle/colors into 'fvertices' and 'colors'.
    for (m = i, k = 0; k < 3; ++m, ++k) {
      for (j = 0; j < 4; ++j)
        fvertices[j + k * 4] = vertices[indices[m] * 4 + j];

      for (j = 0; j < 3; ++j)
        fcolors[j + k * 3] = colors[indices[m] * 3 + j];
    }

    memset(drawpolygon, 0, sizeof(int) * 64);

    // Transform into clip coordinates.
    for (j = 0; j < 3; ++j)
      transformVectorByMatrix(modelviewprojection, &fvertices[j * 4]);

    // A mutable copy of the polygon is sent through the pipeline (subject to decomposition).
    for (m = 0; m < 3 * 4; ++m)
      polygons[m] = fvertices[m];
    for (m = 0; m < 3 * 3; ++m)
      polycolors[m] = fcolors[m];
    drawpolygon[0] = 1;
    polycount = 1;

    // Clip in homogeneous space (this operation generates additional polygons).
    clipPolygonHomogeneousAgainstAllPlanes(polygons, polycolors, drawpolygon, &polycount);

    for (j = 0; j < polycount; ++j) {
      // Skip a polygon not in frustum.
      if (!drawpolygon[j])
        continue;

      for (m = 0; m < 3; ++m) {    // For each vertex.
        polygons[j * 3 * 4 + m * 4 + 0] /= polygons[j * 3 * 4 + m * 4 + 3];
        polygons[j * 3 * 4 + m * 4 + 1] /= polygons[j * 3 * 4 + m * 4 + 3];
        polygons[j * 3 * 4 + m * 4 + 2] /= polygons[j * 3 * 4 + m * 4 + 3];
        polygons[j * 3 * 4 + m * 4 + 3] = 1.f;

        transformVectorByMatrix(viewport, &polygons[j * 3 * 4 + m * 4]);
      }

      drawTriangle(&polygons[j * 3 * 4], &polycolors[j * 3 * 3]);
    }
  }
}
