/**
 * @author  Khalil El Henoud
 * @brief   Functionality for creating/loading models.
 */
#ifndef C_OBJECT
#define C_OBJECT

typedef struct Model Model;

void freeModel(Model *model);

const float *getVertexArray(const Model *model, int *vertexnumber);
const float *getColorArray(const Model *model, int *vertexnumber);
int getVertexCount(const Model *model);
const int *getIndexArray(const Model *model, int *indexnumber);
int getIndexCount(const Model *model);

Model *createCube(float halfWidth);

/**
 * Parses an FBX file for the mesh found in it. If a mesh is found a 'Model' pointer is returned,
 * otherwise in case it isn't found or an error occured NULL is returned.
 * @remarks           INCOMPLETE/UNTESTED. 
 *                    Allow specification of the model name.
 *                    Allow reading in chunks so as to deal with large files within memory limits.
 * @param  filename   Name of the file we are parsing.
 * @return            NULL if it fails, else the model from the fbx ASCII file.
 */
Model *loadModelFromFile_FBX_ASCII(const char *filename);

#endif