/**
 * @author  Khalil El Henoud
 * @brief   Simulate a 3D pipeline transformation stack functionality.
 */
#ifndef CPROJ_PIPELINE
#define CPROJ_PIPELINE

#include "matrix.h"

enum {
  MODELVIEW,
  PROJECTION
};

void getViewportMatrix(Matrix4D output);

/**
 * Sets the viewport dimensions and location on the screen.
 * @param  x        The starting x position on the screen.
 * @param  y        The starting y position on the screen (y axis is downward, a positive will
 *                  push the viewport down).
 * @param  width    The width of the viewport (should match ratio of the frustum otherwise
 *                  distortion would occur).
 * @param  height   The height of the viewport (see above).
 */
void setViewport(float x, float y, float width, float height);

/**
 * Creates the frustrum (projection) matrix and post multiply the current stack with it. For
 * more info visit: 'http://www.songho.ca/opengl/gl_projectionmatrix.html'.
 * @param  right   The right frustum clipping plane on the near plane.
 * @param  left    The left frustum clipping plane on the near plane.
 * @param  bottom  The bottom frustum clipping plane on the near plane.
 * @param  top     The top frustum clipping plane on the near plane.
 * @param  near    The near clipping plane distance (positive value).
 * @param  far     The far clipping plane distance (positive value).
 */
void frustrum(float right, float left, float bottom, float top, float near, float far);

void getMatrix(Matrix4D output);

/**
 * Sets which stack we are currently affecting. Only the 2 values specified by the enum at the
 * top of the file are allowed.
 *
 * @param  mode  The mode 'MODELVIEW' or 'PROJECTION'.
 */
void setMatrixMode(int mode);

void pushMatrix(void);

void popMatrix(Matrix4D output);

void loadIdentity(void);

void replace(Matrix4D matrix);

void postRotateX(float angle);

void postRotateY(float angle);

void postRotateZ(float angle);

void postTranslate(float x, float y, float z);

/**
 * Post multiply the current matrix at the top of the stack with the new 'matrix', meaning the
 * transformation that 'matrix' represent will be applied after the transformation at the top of
 * the stack.
 * 
 * @param  matrix  The transformation to post apply to the top of the stack.
 */
void postMultiply(Matrix4D matrix);

void preRotateX(float angle);

void preRotateY(float angle);

void preRotateZ(float angle);

void preTranslate(float x, float y, float z);

void preMultiply(Matrix4D matrix);

#endif