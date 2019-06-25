/**
 * @author  Khalil El Henoud
 * @brief   Functionality for creating/loading models.
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
 * @remarks	        Incomplete: we need to allow specification of the model name. We also need to 
 *                  allow reading in chunks so as to deal with large files (memory limits).
 * @param	filename	Name of the file we are parsing.
 * @return	        NULL if it fails, else the model from the fbx ASCII file.
 */
Model *loadModelFromFile_FBX_ASCII(const char *filename);

#endif