#include <math.h>
#include <assert.h>

void copyMatrix(float src[], float dst[])
{
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
  dst[4] = src[4];
  dst[5] = src[5];
  dst[6] = src[6];
  dst[7] = src[7];
  dst[8] = src[8];
  dst[9] = src[9];
  dst[10] = src[10];
  dst[11] = src[11];
  dst[12] = src[12];
  dst[13] = src[13];
  dst[14] = src[14];
  dst[15] = src[15];
}

void setIdentity(float matrix[])
{
  matrix[1] = matrix[2] = matrix[3] = matrix[4] = matrix[6] = matrix[7] = matrix[8] = matrix[9] = matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.f;
  matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.f;
}

#define DETERMINANT2D(m0, m1, m2, m3, m4, m5, m6, m7, m8)  ((m0 * m4 * m8) - (m0 * m7 * m5) - (m3 * m1 * m8) + (m3 * m7 * m2) + (m6 * m1 * m5) - (m6 * m4 * m2))

float getDeterminant(float m[])
{
  float det0, det1, det2, det3;

  det0 = DETERMINANT2D(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
  det1 = DETERMINANT2D(m[1], m[2], m[3], m[9], m[10], m[11], m[13], m[14], m[15]);
  det2 = DETERMINANT2D(m[1], m[2], m[3], m[5], m[6], m[7], m[13], m[14], m[15]);
  det3 = DETERMINANT2D(m[1], m[2], m[3], m[5], m[6], m[7], m[9], m[10], m[11]);

  return m[0] * det0 - m[4] * det1 + m[8] * det2 - m[12] * det3;
}

void setTranspose(float matrix[])
{
  float tmp = matrix[1];
  matrix[1] = matrix[4];
  matrix[4] = tmp;
  tmp = matrix[2];
  matrix[2] = matrix[8];
  matrix[8] = tmp;
  tmp = matrix[3];
  matrix[3] = matrix[12];
  matrix[12] = tmp;
  tmp = matrix[6];
  matrix[6] = matrix[9];
  matrix[9] = tmp;
  tmp = matrix[7];
  matrix[7] = matrix[13];
  matrix[13] = tmp;
  tmp = matrix[11];
  matrix[11] = matrix[14];
  matrix[14] = tmp;
}

void setInverse(float m[])
{
  int i;
  float result[16], factor;

  factor = getDeterminant(m);
  assert(factor);
  factor = 1.f/factor;

  result[0] = DETERMINANT2D(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
  result[1] = -DETERMINANT2D(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]);
  result[2] = DETERMINANT2D(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]);
  result[3] = -DETERMINANT2D(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);
  result[4] = -DETERMINANT2D(m[1], m[2], m[3], m[9], m[10], m[11], m[13], m[14], m[15]);
  result[5] = DETERMINANT2D(m[0], m[2], m[3], m[8], m[10], m[11], m[12], m[14], m[15]);
  result[6] = -DETERMINANT2D(m[0], m[1], m[3], m[8], m[9], m[11], m[12], m[13], m[15]);
  result[7] = DETERMINANT2D(m[0], m[1], m[2], m[8], m[9], m[10], m[12], m[13], m[14]);
  result[8] = DETERMINANT2D(m[1], m[2], m[3], m[5], m[6], m[7], m[13], m[14], m[15]);
  result[9] = -DETERMINANT2D(m[0], m[2], m[3], m[4], m[6], m[7], m[12], m[14], m[15]);
  result[10] = DETERMINANT2D(m[0], m[1], m[3], m[4], m[5], m[7], m[12], m[13], m[15]);
  result[11] = -DETERMINANT2D(m[0], m[1], m[2], m[4], m[5], m[6], m[12], m[13], m[14]);
  result[12] = -DETERMINANT2D(m[1], m[2], m[3], m[5], m[6], m[7], m[9], m[10], m[11]);
  result[13] = DETERMINANT2D(m[0], m[2], m[3], m[4], m[6], m[7], m[8], m[10], m[11]);
  result[14] = -DETERMINANT2D(m[0], m[1], m[3], m[4], m[5], m[7], m[8], m[9], m[11]);
  result[15] = DETERMINANT2D(m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]);

  setTranspose(result);
  for (i = 0; i < 16; ++i)
    m[i] = factor * result[i];
}

void rotationMatrixX(float matrix[], float angle)
{
  setIdentity(matrix);
  matrix[5] = matrix[10] = cosf(angle);
  matrix[6] = sinf(angle);
  matrix[9] = -matrix[6];
}

void rotationMatrixY(float matrix[], float angle)
{
  setIdentity(matrix);
  matrix[0] = matrix[10] = cosf(angle);
  matrix[8] = sinf(angle);
  matrix[2] = -matrix[8];
}

void rotationMatrixZ(float matrix[], float angle)
{
  setIdentity(matrix);
  matrix[0] = matrix[5] = cosf(angle);
  matrix[1] = sinf(angle);
  matrix[4] = -matrix[1];
}

void axisAngleMatrix(float matrix[], float axis[], float angle)
{
  float a = cosf(angle);
  float b = 1 - a;
  float c = sinf(angle);
  float m1[9] = {0}, m2[9] = {0}, m3[9] = {0}, m4[9] = {0};
  int i;

  float len = sqrtf(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2] + axis[3] * axis[3]);
  axis[0] /= len;
  axis[1] /= len;
  axis[2] /= len;
  axis[3] /= len;

  // See "Orientation.doc" in cs 250 transformations chapter and xin li notes.
  m1[0] = m1[4] = m1[8] = a;

  m2[0] = axis[0] * axis[0] * b;
  m2[1] = axis[0] * axis[1] * b;
  m2[2] = axis[0] * axis[2] * b;
  m2[3] = axis[0] * axis[1] * b;
  m2[4] = axis[1] * axis[1] * b;
  m2[5] = axis[1] * axis[2] * b;
  m2[6] = axis[0] * axis[2] * b;
  m2[7] = axis[1] * axis[2] * b;
  m2[8] = axis[2] * axis[2] * b;

  m3[0] = 0;
  m3[1] = axis[2] * c;
  m3[2] = -axis[1] * c;
  m3[3] = -axis[2] * c;
  m3[4] = 0;
  m3[5] = axis[0] * c;
  m3[6] = axis[1] * c;
  m3[7] = -axis[0] * c;
  m3[8] = 0;

  for (i = 0; i < 9; ++i)
    m4[i] = m1[i] + m2[i] + m3[i];

  // Setting the final matrix.
  setIdentity(matrix);
  matrix[0] = m4[0];
  matrix[1] = m4[1];
  matrix[2] = m4[2];
  matrix[4] = m4[3];
  matrix[5] = m4[4];
  matrix[6] = m4[5];
  matrix[8] = m4[6];
  matrix[9] = m4[7];
  matrix[10] = m4[8];
}

void translationMatrix(float matrix[], float tx, float ty, float tz)
{
  setIdentity(matrix);
  matrix[12] = tx;
  matrix[13] = ty;
  matrix[14] = tz;
}

void scaleMatrix(float matrix[], float sx, float sy, float sz)
{
  setIdentity(matrix);
  matrix[0] = sx;
  matrix[5] = sy;
  matrix[10] = sz;
}

void crossProductMatrix(float matrix[], float u[])
{
  float length = sqrtf(u[0] * u[0] + u[1] * u[1] + u[2] * u[2] + u[3] * u[3]);
  u[0] /= length;
  u[1] /= length;
  u[2] /= length;
  u[3] /= length;

  setIdentity(matrix);
  matrix[0] = 0;
  matrix[1] = u[2];
  matrix[2] = -u[1];
  matrix[3] = 0;
  matrix[4] = -u[2];
  matrix[5] = 0;
  matrix[6] = u[0];
  matrix[7] = 0;
  matrix[8] = u[1];
  matrix[9] = -u[0];
  matrix[10] = 0;
  matrix[11] = 0;
}

void multiplyMatrices(float m1[], float m2[], float result[])
{
  float tmp[16];

  tmp[0]= m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
  tmp[1]= m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
  tmp[2]= m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
  tmp[3]= m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
  tmp[4]= m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
  tmp[5]= m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
  tmp[6]= m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
  tmp[7]= m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
  tmp[8]= m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
  tmp[9]= m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
  tmp[10]= m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
  tmp[11]= m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
  tmp[12]= m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
  tmp[13]= m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
  tmp[14]= m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
  tmp[15]= m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

  copyMatrix(tmp, result);
}

void transformVectorByMatrix(float m[], float vector[])
{
  float result[4];
  result[0] = m[0] * vector[0] + m[4] * vector[1] + m[8] * vector[2] + m[12] * vector[3];
  result[1] = m[1] * vector[0] + m[5] * vector[1] + m[9] * vector[2] + m[13] * vector[3];
  result[2] = m[2] * vector[0] + m[6] * vector[1] + m[10] * vector[2] + m[14] * vector[3];
  result[3] = m[3] * vector[0] + m[7] * vector[1] + m[11] * vector[2] + m[15] * vector[3];
  vector[0] = result[0];
  vector[1] = result[1];
  vector[2] = result[2];
  vector[3] = result[3];
}

#undef DETERMINANT2D