#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include "matrix.h"
#include "pipeline.h"
#include "renderer.h"

typedef struct Model {
	/** The vertices, each vertex is made of 4 floats. */
	float *vertices;
	/** The vertices colors, 3 floats each. */
	float *colors;
	/** The vertices number. */
	int vertexNumber;

	/** The indices. */
	int *indices;
	/** The indices number. */
	int indexNumber;
} Model;

/**
 * Creates an empty Model structure with the appropriate number of vertices and indices and
 * returns it to the user.
 * @param	vertexNumber	The vertex number.
 * @param	indexNumber 	The index number.
 * @return	a Model pointer allocated on the heap, vertices and indices are also allocated on the
 * 			heap (will be modified to use custom allocator on pre-allocated memory).
 */
static Model *createStructure(int vertexNumber, int indexNumber)
{
	Model *object = (Model *)malloc(sizeof(Model) * 1);
	object->vertices = (float *)malloc(sizeof(float) * 4 * vertexNumber);
	object->colors = (float *)malloc(sizeof(float) * 3 * vertexNumber);
	object->vertexNumber = vertexNumber;

	object->indices = (int *)malloc(sizeof(int) * indexNumber);
	object->indexNumber = indexNumber;

	return object;
}

void freeModel(Model *model)
{
	if (!model) {
		printf("Model was null.");
		return;
	}

	free((void *)model->vertices);
	free((void *)model->colors);
	free((void *)model->indices);
	free((void *)model);
}

Model *createCube(float n)
{
	float x1 = -n, x2 = n, y1 = -n, y2 = n, z1 = -n, z2 = n;
	int i;
	Model *model = createStructure(36, 36);
	float *vertices = model->vertices;
	float *colors = model->colors;

	/* Specify the indices. */
	for (i = 0; i < 36; ++i)
		model->indices[i] = i;

	/* i represents the faces. */
	for (i = 0; i < 6; ++i) {
		if (i == 0) {
			vertices[i * 24 + 0 * 4 + 0] = x1;		/* Vertex 1. */
			vertices[i * 24 + 0 * 4 + 1] = y2;
			vertices[i * 24 + 0 * 4 + 2] = z2;
			vertices[i * 24 + 0 * 4 + 3] = 1.f;
			colors[i * 18 + 0 * 3 + 0] = 1.f;
			colors[i * 18 + 0 * 3 + 1] = 0.f;
			colors[i * 18 + 0 * 3 + 2] = 0.f;

			vertices[i * 24 + 1 * 4 + 0] = x1;		/* Vertex 2. */
			vertices[i * 24 + 1 * 4 + 1] = y1;
			vertices[i * 24 + 1 * 4 + 2] = z2;
			vertices[i * 24 + 1 * 4 + 3] = 1.f;
			colors[i * 18 + 1 * 3 + 0] = 1.f;
			colors[i * 18 + 1 * 3 + 1] = 0.f;
			colors[i * 18 + 1 * 3 + 2] = 0.f;

			vertices[i * 24 + 2 * 4 + 0] = x2;		/* Vertex 3. */
			vertices[i * 24 + 2 * 4 + 1] = y1;
			vertices[i * 24 + 2 * 4 + 2] = z2;
			vertices[i * 24 + 2 * 4 + 3] = 1.f;
			colors[i * 18 + 2 * 3 + 0] = 1.f;
			colors[i * 18 + 2 * 3 + 1] = 0.f;
			colors[i * 18 + 2 * 3 + 2] = 0.f;

			vertices[i * 24 + 3 * 4 + 0] = x1;		/* Vertex 4. */
			vertices[i * 24 + 3 * 4 + 1] = y2;
			vertices[i * 24 + 3 * 4 + 2] = z2;
			vertices[i * 24 + 3 * 4 + 3] = 1.f;
			colors[i * 18 + 3 * 3 + 0] = 1.f;
			colors[i * 18 + 3 * 3 + 1] = 0.f;
			colors[i * 18 + 3 * 3 + 2] = 0.f;

			vertices[i * 24 + 4 * 4 + 0] = x2;		/* Vertex 5. */
			vertices[i * 24 + 4 * 4 + 1] = y1;
			vertices[i * 24 + 4 * 4 + 2] = z2;
			vertices[i * 24 + 4 * 4 + 3] = 1.f;
			colors[i * 18 + 4 * 3 + 0] = 1.f;
			colors[i * 18 + 4 * 3 + 1] = 0.f;
			colors[i * 18 + 4 * 3 + 2] = 0.f;

			vertices[i * 24 + 5 * 4 + 0] = x2;		/* Vertex 6. */
			vertices[i * 24 + 5 * 4 + 1] = y2;
			vertices[i * 24 + 5 * 4 + 2] = z2;
			vertices[i * 24 + 5 * 4 + 3] = 1.f;
			colors[i * 18 + 5 * 3 + 0] = 1.f;
			colors[i * 18 + 5 * 3 + 1] = 0.f;
			colors[i * 18 + 5 * 3 + 2] = 0.f;
		} else if (i == 1) {
			vertices[i * 24 + 0 * 4 + 0] = x2;		/* Vertex 1. */
			vertices[i * 24 + 0 * 4 + 1] = y2;
			vertices[i * 24 + 0 * 4 + 2] = z1;
			vertices[i * 24 + 0 * 4 + 3] = 1.f;
			colors[i * 18 + 0 * 3 + 0] = 0.f;
			colors[i * 18 + 0 * 3 + 1] = 1.f;
			colors[i * 18 + 0 * 3 + 2] = 0.f;

			vertices[i * 24 + 1 * 4 + 0] = x2;		/* Vertex 2. */
			vertices[i * 24 + 1 * 4 + 1] = y1;
			vertices[i * 24 + 1 * 4 + 2] = z1;
			vertices[i * 24 + 1 * 4 + 3] = 1.f;
			colors[i * 18 + 1 * 3 + 0] = 0.f;
			colors[i * 18 + 1 * 3 + 1] = 1.f;
			colors[i * 18 + 1 * 3 + 2] = 0.f;

			vertices[i * 24 + 2 * 4 + 0] = x1;		/* Vertex 3. */
			vertices[i * 24 + 2 * 4 + 1] = y1;
			vertices[i * 24 + 2 * 4 + 2] = z1;
			vertices[i * 24 + 2 * 4 + 3] = 1.f;
			colors[i * 18 + 2 * 3 + 0] = 0.f;
			colors[i * 18 + 2 * 3 + 1] = 1.f;
			colors[i * 18 + 2 * 3 + 2] = 0.f;

			vertices[i * 24 + 3 * 4 + 0] = x2;		/* Vertex 4. */
			vertices[i * 24 + 3 * 4 + 1] = y2;
			vertices[i * 24 + 3 * 4 + 2] = z1;
			vertices[i * 24 + 3 * 4 + 3] = 1.f;
			colors[i * 18 + 3 * 3 + 0] = 0.f;
			colors[i * 18 + 3 * 3 + 1] = 1.f;
			colors[i * 18 + 3 * 3 + 2] = 0.f;

			vertices[i * 24 + 4 * 4 + 0] = x1;		/* Vertex 5. */
			vertices[i * 24 + 4 * 4 + 1] = y1;
			vertices[i * 24 + 4 * 4 + 2] = z1;
			vertices[i * 24 + 4 * 4 + 3] = 1.f;
			colors[i * 18 + 4 * 3 + 0] = 0.f;
			colors[i * 18 + 4 * 3 + 1] = 1.f;
			colors[i * 18 + 4 * 3 + 2] = 0.f;

			vertices[i * 24 + 5 * 4 + 0] = x1;		/* Vertex 6. */
			vertices[i * 24 + 5 * 4 + 1] = y2;
			vertices[i * 24 + 5 * 4 + 2] = z1;
			vertices[i * 24 + 5 * 4 + 3] = 1.f;
			colors[i * 18 + 5 * 3 + 0] = 0.f;
			colors[i * 18 + 5 * 3 + 1] = 1.f;
			colors[i * 18 + 5 * 3 + 2] = 0.f;
		} else if (i == 2) {
			vertices[i * 24 + 0 * 4 + 0] = x2;		/* Vertex 1. */
			vertices[i * 24 + 0 * 4 + 1] = y2;
			vertices[i * 24 + 0 * 4 + 2] = z2;
			vertices[i * 24 + 0 * 4 + 3] = 1.f;
			colors[i * 18 + 0 * 3 + 0] = 0.f;
			colors[i * 18 + 0 * 3 + 1] = 0.f;
			colors[i * 18 + 0 * 3 + 2] = 1.f;

			vertices[i * 24 + 1 * 4 + 0] = x2;		/* Vertex 2. */
			vertices[i * 24 + 1 * 4 + 1] = y1;
			vertices[i * 24 + 1 * 4 + 2] = z2;
			vertices[i * 24 + 1 * 4 + 3] = 1.f;
			colors[i * 18 + 1 * 3 + 0] = 0.f;
			colors[i * 18 + 1 * 3 + 1] = 0.f;
			colors[i * 18 + 1 * 3 + 2] = 1.f;

			vertices[i * 24 + 2 * 4 + 0] = x2;		/* Vertex 3. */
			vertices[i * 24 + 2 * 4 + 1] = y1;
			vertices[i * 24 + 2 * 4 + 2] = z1;
			vertices[i * 24 + 2 * 4 + 3] = 1.f;
			colors[i * 18 + 2 * 3 + 0] = 0.f;
			colors[i * 18 + 2 * 3 + 1] = 0.f;
			colors[i * 18 + 2 * 3 + 2] = 1.f;

			vertices[i * 24 + 3 * 4 + 0] = x2;		/* Vertex 4. */
			vertices[i * 24 + 3 * 4 + 1] = y2;
			vertices[i * 24 + 3 * 4 + 2] = z2;
			vertices[i * 24 + 3 * 4 + 3] = 1.f;
			colors[i * 18 + 3 * 3 + 0] = 0.f;
			colors[i * 18 + 3 * 3 + 1] = 0.f;
			colors[i * 18 + 3 * 3 + 2] = 1.f;

			vertices[i * 24 + 4 * 4 + 0] = x2;		/* Vertex 5. */
			vertices[i * 24 + 4 * 4 + 1] = y1;
			vertices[i * 24 + 4 * 4 + 2] = z1;
			vertices[i * 24 + 4 * 4 + 3] = 1.f;
			colors[i * 18 + 4 * 3 + 0] = 0.f;
			colors[i * 18 + 4 * 3 + 1] = 0.f;
			colors[i * 18 + 4 * 3 + 2] = 1.f;

			vertices[i * 24 + 5 * 4 + 0] = x2;		/* Vertex 6. */
			vertices[i * 24 + 5 * 4 + 1] = y2;
			vertices[i * 24 + 5 * 4 + 2] = z1;
			vertices[i * 24 + 5 * 4 + 3] = 1.f;
			colors[i * 18 + 5 * 3 + 0] = 0.f;
			colors[i * 18 + 5 * 3 + 1] = 0.f;
			colors[i * 18 + 5 * 3 + 2] = 1.f;
		} else if (i == 3) {
			vertices[i * 24 + 0 * 4 + 0] = x1;		/* Vertex 1. */
			vertices[i * 24 + 0 * 4 + 1] = y2;
			vertices[i * 24 + 0 * 4 + 2] = z1;
			vertices[i * 24 + 0 * 4 + 3] = 1.f;
			colors[i * 18 + 0 * 3 + 0] = 1.f;
			colors[i * 18 + 0 * 3 + 1] = 0.f;
			colors[i * 18 + 0 * 3 + 2] = 1.f;

			vertices[i * 24 + 1 * 4 + 0] = x1;		/* Vertex 2. */
			vertices[i * 24 + 1 * 4 + 1] = y1;
			vertices[i * 24 + 1 * 4 + 2] = z1;
			vertices[i * 24 + 1 * 4 + 3] = 1.f;
			colors[i * 18 + 1 * 3 + 0] = 1.f;
			colors[i * 18 + 1 * 3 + 1] = 0.f;
			colors[i * 18 + 1 * 3 + 2] = 1.f;

			vertices[i * 24 + 2 * 4 + 0] = x1;		/* Vertex 3. */
			vertices[i * 24 + 2 * 4 + 1] = y1;
			vertices[i * 24 + 2 * 4 + 2] = z2;
			vertices[i * 24 + 2 * 4 + 3] = 1.f;
			colors[i * 18 + 2 * 3 + 0] = 1.f;
			colors[i * 18 + 2 * 3 + 1] = 0.f;
			colors[i * 18 + 2 * 3 + 2] = 1.f;

			vertices[i * 24 + 3 * 4 + 0] = x1;		/* Vertex 4. */
			vertices[i * 24 + 3 * 4 + 1] = y2;
			vertices[i * 24 + 3 * 4 + 2] = z1;
			vertices[i * 24 + 3 * 4 + 3] = 1.f;
			colors[i * 18 + 3 * 3 + 0] = 1.f;
			colors[i * 18 + 3 * 3 + 1] = 0.f;
			colors[i * 18 + 3 * 3 + 2] = 1.f;

			vertices[i * 24 + 4 * 4 + 0] = x1;		/* Vertex 5. */
			vertices[i * 24 + 4 * 4 + 1] = y1;
			vertices[i * 24 + 4 * 4 + 2] = z2;
			vertices[i * 24 + 4 * 4 + 3] = 1.f;
			colors[i * 18 + 4 * 3 + 0] = 1.f;
			colors[i * 18 + 4 * 3 + 1] = 0.f;
			colors[i * 18 + 4 * 3 + 2] = 1.f;

			vertices[i * 24 + 5 * 4 + 0] = x1;		/* Vertex 6. */
			vertices[i * 24 + 5 * 4 + 1] = y2;
			vertices[i * 24 + 5 * 4 + 2] = z2;
			vertices[i * 24 + 5 * 4 + 3] = 1.f;
			colors[i * 18 + 5 * 3 + 0] = 1.f;
			colors[i * 18 + 5 * 3 + 1] = 0.f;
			colors[i * 18 + 5 * 3 + 2] = 1.f;
		} else if (i == 4) {
			vertices[i * 24 + 0 * 4 + 0] = x1;		/* Vertex 1. */
			vertices[i * 24 + 0 * 4 + 1] = y2;
			vertices[i * 24 + 0 * 4 + 2] = z1;
			vertices[i * 24 + 0 * 4 + 3] = 1.f;
			colors[i * 18 + 0 * 3 + 0] = 1.f;
			colors[i * 18 + 0 * 3 + 1] = 1.f;
			colors[i * 18 + 0 * 3 + 2] = 0.f;

			vertices[i * 24 + 1 * 4 + 0] = x1;		/* Vertex 2. */
			vertices[i * 24 + 1 * 4 + 1] = y2;
			vertices[i * 24 + 1 * 4 + 2] = z2;
			vertices[i * 24 + 1 * 4 + 3] = 1.f;
			colors[i * 18 + 1 * 3 + 0] = 1.f;
			colors[i * 18 + 1 * 3 + 1] = 1.f;
			colors[i * 18 + 1 * 3 + 2] = 0.f;

			vertices[i * 24 + 2 * 4 + 0] = x2;		/* Vertex 3. */
			vertices[i * 24 + 2 * 4 + 1] = y2;
			vertices[i * 24 + 2 * 4 + 2] = z2;
			vertices[i * 24 + 2 * 4 + 3] = 1.f;
			colors[i * 18 + 2 * 3 + 0] = 1.f;
			colors[i * 18 + 2 * 3 + 1] = 1.f;
			colors[i * 18 + 2 * 3 + 2] = 0.f;

			vertices[i * 24 + 3 * 4 + 0] = x1;		/* Vertex 4. */
			vertices[i * 24 + 3 * 4 + 1] = y2;
			vertices[i * 24 + 3 * 4 + 2] = z1;
			vertices[i * 24 + 3 * 4 + 3] = 1.f;
			colors[i * 18 + 3 * 3 + 0] = 1.f;
			colors[i * 18 + 3 * 3 + 1] = 1.f;
			colors[i * 18 + 3 * 3 + 2] = 0.f;

			vertices[i * 24 + 4 * 4 + 0] = x2;		/* Vertex 5. */
			vertices[i * 24 + 4 * 4 + 1] = y2;
			vertices[i * 24 + 4 * 4 + 2] = z2;
			vertices[i * 24 + 4 * 4 + 3] = 1.f;
			colors[i * 18 + 4 * 3 + 0] = 1.f;
			colors[i * 18 + 4 * 3 + 1] = 1.f;
			colors[i * 18 + 4 * 3 + 2] = 0.f;

			vertices[i * 24 + 5 * 4 + 0] = x2;		/* Vertex 6. */
			vertices[i * 24 + 5 * 4 + 1] = y2;
			vertices[i * 24 + 5 * 4 + 2] = z1;
			vertices[i * 24 + 5 * 4 + 3] = 1.f;
			colors[i * 18 + 5 * 3 + 0] = 1.f;
			colors[i * 18 + 5 * 3 + 1] = 1.f;
			colors[i * 18 + 5 * 3 + 2] = 0.f;
		} else if (i == 5) {
			vertices[i * 24 + 0 * 4 + 0] = x2;		/* Vertex 1. */
			vertices[i * 24 + 0 * 4 + 1] = y1;
			vertices[i * 24 + 0 * 4 + 2] = z1;
			vertices[i * 24 + 0 * 4 + 3] = 1.f;
			colors[i * 18 + 0 * 3 + 0] = 0.f;
			colors[i * 18 + 0 * 3 + 1] = 1.f;
			colors[i * 18 + 0 * 3 + 2] = 1.f;

			vertices[i * 24 + 1 * 4 + 0] = x2;		/* Vertex 2. */
			vertices[i * 24 + 1 * 4 + 1] = y1;
			vertices[i * 24 + 1 * 4 + 2] = z2;
			vertices[i * 24 + 1 * 4 + 3] = 1.f;
			colors[i * 18 + 1 * 3 + 0] = 0.f;
			colors[i * 18 + 1 * 3 + 1] = 1.f;
			colors[i * 18 + 1 * 3 + 2] = 1.f;

			vertices[i * 24 + 2 * 4 + 0] = x1;		/* Vertex 3. */
			vertices[i * 24 + 2 * 4 + 1] = y1;
			vertices[i * 24 + 2 * 4 + 2] = z2;
			vertices[i * 24 + 2 * 4 + 3] = 1.f;
			colors[i * 18 + 2 * 3 + 0] = 0.f;
			colors[i * 18 + 2 * 3 + 1] = 1.f;
			colors[i * 18 + 2 * 3 + 2] = 1.f;

			vertices[i * 24 + 3 * 4 + 0] = x2;		/* Vertex 4. */
			vertices[i * 24 + 3 * 4 + 1] = y1;
			vertices[i * 24 + 3 * 4 + 2] = z1;
			vertices[i * 24 + 3 * 4 + 3] = 1.f;
			colors[i * 18 + 3 * 3 + 0] = 0.f;
			colors[i * 18 + 3 * 3 + 1] = 1.f;
			colors[i * 18 + 3 * 3 + 2] = 1.f;

			vertices[i * 24 + 4 * 4 + 0] = x1;		/* Vertex 5. */
			vertices[i * 24 + 4 * 4 + 1] = y1;
			vertices[i * 24 + 4 * 4 + 2] = z2;
			vertices[i * 24 + 4 * 4 + 3] = 1.f;
			colors[i * 18 + 4 * 3 + 0] = 0.f;
			colors[i * 18 + 4 * 3 + 1] = 1.f;
			colors[i * 18 + 4 * 3 + 2] = 1.f;

			vertices[i * 24 + 5 * 4 + 0] = x1;		/* Vertex 6. */
			vertices[i * 24 + 5 * 4 + 1] = y1;
			vertices[i * 24 + 5 * 4 + 2] = z1;
			vertices[i * 24 + 5 * 4 + 3] = 1.f;
			colors[i * 18 + 5 * 3 + 0] = 0.f;
			colors[i * 18 + 5 * 3 + 1] = 1.f;
			colors[i * 18 + 5 * 3 + 2] = 1.f;
		}
	}

	return model;
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

void renderModel(Model *model)
{
	Matrix4D viewport, projection, modelviewtransformation, modelviewprojection;
	int i, j, k, m;
	float fvertices[3 * 4];
	float colors[3 * 3];

	getViewportMatrix(viewport);
	setMatrixMode(PROJECTION);
	getMatrix(projection);
	setMatrixMode(MODELVIEW);
	getMatrix(modelviewtransformation);

	multiplyMatrices(projection, modelviewtransformation, modelviewprojection);

	/* For every triangle making up the object. */
	for (i = 0; i < model->indexNumber; i += 3) {
		/* Copy every triangle and its colors into 'fvertices' and 'colors'. */
		for (m = i, k = 0; k < 3; ++m, ++k) {
			for (j = 0; j < 4; ++j) 
				fvertices[j + k * 4] = model->vertices[model->indices[m] * 4 + j];

			for (j = 0; j < 3; ++j)
				colors[j + k * 3] = model->colors[model->indices[m] * 3 + j];
		}	

		memset(drawpolygon, 0, sizeof(int) * 64);

		/* Transform into clip coordinates. */
		for (j = 0; j < 3; ++j)
			transformVectorByMatrix(modelviewprojection, &fvertices[j * 4]);

		/* Copy the polygon into the polygons array which will be instead sent through the 
		pipeline (with its colors). */
		for (m = 0; m < 3 * 4; ++m)
			polygons[m] = fvertices[m];
		for (m = 0; m < 3 * 3; ++m)
			polycolors[m] = colors[m];
		drawpolygon[0] = 1;
		polycount = 1;

		/* Clip in homogeneous space (this operation generates additional polygons). */
		clipPolygonHomogeneousAgainstAllPlanes(polygons, polycolors, drawpolygon, &polycount);

		for (j = 0; j < polycount; ++j) {
			/* If the polygon was totally outside the frustum then do not consider it (it was 
			totally clipped). */
			if (!drawpolygon[j])
				continue;

			for (m = 0; m < 3; ++m) {		/* For each vertex. */
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

static Model *parseModel_FBX_ASCII(const char *data);
static const char *readChunk_FBX_ASCII(const char *data, int *length, char *chunkPath);
static const char *readSubChunk_FBX_ASCII(const char *data, int *length, const char *token);
static const char *reachField_FBX_ASCII(const char *chunk, int *length, const char *field);

Model *loadModelFromFile_FBX_ASCII(const char *filename)
{
	FILE *stream = NULL;
	char *pfile = NULL;
	Model *model = NULL;
	
	if (stream = fopen(filename, "rt")) {
		unsigned long filesize;
		fseek(stream, 0, SEEK_END);
		filesize = ftell(stream);
		fseek(stream, 0, SEEK_SET);

		if (filesize) {
			size_t read = 0;
			pfile = (char *)malloc(sizeof(char) * (filesize + 1));
			memset(pfile, 0, filesize + 1);
			
			read = fread(pfile, sizeof(char), filesize, stream);

			/* An error occured while reading. */
			if (read != filesize) {
				printf("File reading error.");
				free((void *)pfile);
				fclose(stream);
				return NULL;
			} else 
				pfile[filesize] = '\0';

			model = parseModel_FBX_ASCII(pfile);
			free((void *)pfile);
			return model;
		}

		fclose(stream);
	} else {
		printf("File open error, check if the file exists.");
		return NULL;
	}

	return NULL;
}

/**
 * Parses the FBX ASCII file for the first model/mesh it finds.
 * @remarks	Incomplete function.
 * @param	data	The file string content.
 * @return	NULL if it fails, else a pointer to a Model.
 */
static Model *parseModel_FBX_ASCII(const char *data)
{
	int i, j, k, length, vertexDataLength, indicesDataLength;
	char path[] = "Objects.Model";
	const char *modelData;
	const char *verticesData;
	char *vertices;
	const char *indicesData;
	char *indices;
	char *ptr;
	Model *model;
	
	vertexDataLength = length = strlen(data);
	modelData = readChunk_FBX_ASCII(data, &length, path);
	
	/* Get the vertices chunck. */
	verticesData = reachField_FBX_ASCII(modelData, &vertexDataLength, "Vertices");
	vertices = malloc(sizeof(char) * (vertexDataLength + 1));
	strncpy(vertices, verticesData, vertexDataLength);
	vertices[vertexDataLength] = '\0';

	/* Find how many vertices we have. */
	for (i = 0, ptr = vertices; ptr[i]; ptr[i] == ',' ? ++i : i, ++ptr);

	/* Get the indices. */
	indicesData = reachField_FBX_ASCII(modelData, &indicesDataLength, "PolygonVertexIndex");
	indices = malloc(sizeof(char) * (indicesDataLength + 1));
	strncpy(indices, indicesData, indicesDataLength);
	indices[indicesDataLength] = '\0';

	/* Find how many indices we have. */
	for (j = 0, ptr = indices; ptr[j]; ptr[j] == ',' ? ++j : j, ++ptr);

	/* Create and fill the object. */
	model = createStructure((i + 1)/3, j + 1);
	
	k = 0;
	ptr = strtok(vertices, " \n\t,");
	while (ptr) {
		sscanf(ptr, "%f", &(model->vertices[k]));
		k++;
		if (!((k + 1) % 4)) {
			model->vertices[k] = 1.f;
			++k;
		}
		ptr = strtok(NULL, " \n\t,");
	}

	k = 0;
	ptr = strtok(indices, " \n\t,");
	while (ptr) {
		sscanf(ptr, "%i", &(model->indices[k]));
		model->indices[k] = (model->indices[k] < 0) ? ~(model->indices[k]): model->indices[k];
		k++;
		ptr = strtok(NULL, " \n\t,");
	}

	return model;
}

/**
 * Reads a specified field in provided chunk. A field in an FBX ASCII file is delimited by a
 * newline not preceeded by a comma.
 * @remarks	Incomplete function.
 * @param	chunk		  	A pointer to the begining of the chunk to consider.
 * @param [in,out]	length	The length of said chunk.
 * @param	field		  	The field we are searching for.
 * @return	NULL if it fails, else a pointer to a const char.
 */
static const char *reachField_FBX_ASCII(const char *chunk, int *length, const char *field)
{
	const char *ptr = strstr(chunk, field);
	const char *columnPtr = strstr(ptr, ":");
	const char *endline = columnPtr++;
	const char *start = endline;
	char *lastComma;
	char line[2056];

	/* Keep getting the lines, if a line ends with a newline preceeded by a comma, 
	it means there is still more data to get. */
	do {
		start = ++endline;
		endline = strstr(start, "\n");
		memset(line, 0, sizeof(char) * 2056);
		assert(endline - start < 2056);
		strncpy(line, start, endline - start);
		lastComma = strrchr(line, ',');
	} while (!strtok(lastComma + 1, " \t\n"));

	*length = endline - columnPtr;
	return columnPtr;
}

/**
 * Given a chunk path (such as "Objects.Model") the function decomposes the path into its
 * smaller nested parts, in this case it is chunk "Objects" and the chunk "Model" within the
 * preceeding one. Afterwards it search for the innermost chunk and returns that.
 * @remarks	Incomplete function.
 * @param 		  	data	 	A pointer to the file content.
 * @param [in,out]	length   	The length of the chunk, this coupled with the returned pointer
 * 								will determine the string that represents our chunk.
 * @param [in,out]	chunkPath	The path to the inner most chunk we want, such as "Objects.Model".
 * @return	NULL if it fails, else a pointer to the chunk start, the length parameter is modified
 * 			to match the length of the chunk we are concerned about.
 */
static const char *readChunk_FBX_ASCII(const char *data, int *length, char *chunkPath)
{
	char *word = strtok(chunkPath, ".");
	const char *subChunk = data;
	while (word) {
		subChunk = readSubChunk_FBX_ASCII(subChunk, length, word);
		word = strtok(NULL, ".");
	}

	return subChunk;
}

/**
 * Similar to 'readChunk_FBX_ASCII' but only concern itselfs with a single chunk (no nested
 * chunks).
 * @remarks	Incomplete function.
 * @param	data		  	A pointer to the string we are searching into.
 * @param [in,out]	length	Will be modified to show the length of the chunk if found.
 * @param	token		  	The chunk we are looking for.
 * @return	NULL if it fails, else a pointer to the chunk start in the data param.
 */
static const char *readSubChunk_FBX_ASCII(const char *data, int *length, const char *token)
{
	const char *ptr = strstr(data, token);

	if (ptr)
	{
		const char *openBrace = strstr(ptr, "{");
		const char *newLine = strstr(ptr, "\n");
		const char *cptr;
		int count = 1;

		/* Newline is found before an open brace (meaning it is not a node). */
		assert(newLine > openBrace);

		cptr = openBrace;
		while (count)
		{
			++cptr;
			if (*cptr == '{')
				++count;
			if (*cptr == '}')
				--count;
		}

		*length = cptr - openBrace;
		return openBrace + 1;
	}
	
	return data;
}
