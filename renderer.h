/**
 * @author  Khalil El Henoud
 * @brief   Rendering functionality.
 */
#ifndef CPROJ_RENDERER
#define CPROJ_RENDERER

/**
 * Initializes the renderer. Creates and sets the color and depth buffer and initializes the
 * homogeneous clipping planes.
 */
void initializeRenderer(void);

void releaseRenderer(void);

void clearColorAndDepthBuffer(void);

void swapBuffer(void);

/**
 * Draw triangle, algorithm can be found in cs 200/250 course.
 * @param  vertices   3 vertices each of 4 floats.
 * @param  colors     The vertices colors, 3 floats per vertex.
 */
void drawTriangle(const float vertices[12], const float colors[9]);

/**
 * The clipping phase can generate additional polygons (up to 64 additional polygons per polygon
 * when clipped against a frustum's 6 planes).
 * @remarks  check out 'http://fabiensanglard.net/polygon_codec/' for info on the algorithm.
 * @param [in,out]  polygons    The polygons that are clipped against the homogeneous plane. Each
 *                              time a polygon is clipped in homogeneous space against a plane
 *                              there is the chance that 2 additional polygons might be created.
 *                              Enough memory for these extra polygons must be pre-allocated.
 * @param [in,out]  polycolors  A color array associated with those polygons.
 * @param [in,out]  drawpolygon A array of flags corresponding to the polygons, which indicate
 *                              which ones are to be drawn and which ones are to be discarded.
 * @param [in,out]  polycount   The number of triangles in polygons, up to 2.
 * @param           normal      The normal representing the plane we are clipping against in
 *                              homogeneous space.
 */
void clipPolygonHomogeneousAgainstPlane(float *polygons, float *polycolors, int *drawpolygon, int *polycount, const float *normal);

/**
 * Clips the polygon in homogeneous space against all 6 frustrum planes. Since we are dealing
 * with six planes a max of 64 polygons can be generated.
 * @param [in,out]  polygons      The polygons that are to be clipped, up to 64 polygons might be
 *                                generated by this function. Memory for them must be preallocated
 *                                in polygons and polycolors.
 * @param [in,out]  polycolors    The colors of each vertex of the polygons.
 * @param [in,out]  drawpolygon   Indicates which polygon is to be drawn.
 * @param [in,out]  polycount     The number of polygons we currently have, up to 64.
 */
void clipPolygonHomogeneousAgainstAllPlanes(float *polygons, float *polycolors, int *drawpolygon, int *polycount);

#endif