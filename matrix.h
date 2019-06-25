/**
 * @author  Khalil El Henoud
 */
#ifndef MATRIX_H_
#define MATRIX_H_

/**
 * Holds the 16 floats that define any 4D matrix.
 * > A Matrix4D is made of 16 floats:	
 * >                  0   4	  8	  12
 * >	 								1	  5	  9	  13  
 * >  								f	  6	  10	14  
 * > 									3	  7	  11	15
 * 
 * @remarks Yet to be optimized. A vector's fourth component should be disregarded to save
 *          multiplication operations. 'axisAngleMatrix' and 'crossProductMatrix' are guilty of that.
 *          While 'transformVectorByMatrix' uses that to be general to both points and vectors. SIMD
 *          instructions should be used to speed up operations.
 * 
 * @remarks This style of matrices representation is the same as OpenGl. Meaning if you have 3
 *          matrices m1 * m2 * m3, the geometric interpretation of the result is that the 
 *          transformation of m3 is applied first followed by m2 and finally m1.
 * 
 * @remarks All angles in radian.
 */
typedef float Matrix4D[16];

void copyMatrix(float src[], float dst[]);

void setIdentity(float matrix[]);

float getDeterminant(float matrix[]);

void setInverse(float matrix[]);

void setTranspose(float matrix[]);

void rotationMatrixX(float matrix[], float angle);	

void rotationMatrixY(float matrix[], float angle);

void rotationMatrixZ(float matrix[], float angle);

/**
 * Axis angle matrix.
 * @remarks	      Check out CS 250 transformations chapter and xin li notes (Orientation.doc) for 
 * 			          the information on the logic behind the transformations. It is actually three
 * 			          transformations that combine to form the final matrix.
 * @param	matrix	The resulting matrix, use this to apply the rotation to a vector/point.
 * @param	axis  	The axis around which the rotation will take place.
 * @param	angle 	The angle in radian.
 */
void axisAngleMatrix(float matrix[], float axis[], float angle);

void translationMatrix(float matrix[], float tx, float ty, float tz);

void scaleMatrix(float matrix[], float sx, float sy, float sz);

/**
 * Cross product matrix.
 * @remarks	        Normalizes 'u' and calculate the matrix that will give the resulting vector if 
 * 			            'u' were to be crossed with it.
 * @param	matrix	  The resulting matrix used to find u x n where n is another normalized vector.
 * @param	u	  	    The vector used as the parameter in the cross product.
 */
void crossProductMatrix(float matrix[], float u[]);

void multiplyMatrices(float matrix1[], float matrix2[], float result[]);

void transformVectorByMatrix(float matrix[], float vector[]);

#endif