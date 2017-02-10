/**
 * @file	object.h
 * @brief	Functionality for creating/loading models.
 */
#ifndef C_OBJECT
#define C_OBJECT

typedef struct Model Model;

void freeModel(Model *model);

Model *createCube(float halfWidth);

void renderModel(Model *model);

/**
 * Parses an FBX file for the mesh found in it. If a mesh is found a 'Model' pointer is returned,
 * otherwise in case it isn't found or an error occured NULL is returned.
 * @remarks	incomplete function. We need to allow specification of the model name. Additional
 * 			improvements include reading the file content using chunks of memory, so as to not
 * 			run out of memory when dealing with big files.
 * @param	filename	Name of the file we are parsing.
 * @return	NULL if it fails, else the model from the fbx ASCII file.
 */
Model *loadModelFromFile_FBX_ASCII(const char *filename);

#endif

