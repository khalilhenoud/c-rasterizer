#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

typedef struct Model {
  float *vertices;                          // Each vertex is 4 floats.
  float *colors;                            // Color is 3 floats.
  int vertexNumber;

  int *indices;                             // Indices array.
  int indexNumber;
} Model;

/**
 * Creates an empty Model structure with the appropriate number of vertices and indices.
 * @return    A heap allocated Model pointer. Vertices/Colors/Incides are also heap allocated.
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
    printf("Model was NULL.");
    return;
  }

  free((void *)model->vertices);
  free((void *)model->colors);
  free((void *)model->indices);
  free((void *)model);
}

const float *getVertexArray(const Model *model, int *vertexnumber)
{
  *vertexnumber = model->vertexNumber;
  return model->vertices;
}

const float *getColorArray(const Model *model, int *vertexnumber)
{
  *vertexnumber = model->vertexNumber;
  return model->colors;
}

int getVertexCount(const Model *model)
{
  return model->vertexNumber;
}

const int *getIndexArray(const Model *model, int *indexnumber)
{
  *indexnumber = model->indexNumber;
  return model->indices;
}

int getIndexCount(const Model *model)
{
  return model->indexNumber;
}

Model *createCube(float n)
{
  float x1 = -n, x2 = n, y1 = -n, y2 = n, z1 = -n, z2 = n;
  Model *model = createStructure(36, 36);
  float *vertices = model->vertices;
  float *colors = model->colors;
  float colorVertices[6][3] = { {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}, {1.f, 1.f, 0.f}, {0.f, 1.f, 1.f} };
  int i, j;

  // Hard set the indices.
  for (i = 0; i < 36; ++i)
    model->indices[i] = i;

  // Create each face in turn.
  for (i = 0; i < 6; ++i) {
    if (i == 0) {
      vertices[i * 24 + 0 * 4 + 0] = x1;    // Vertex 1.
      vertices[i * 24 + 0 * 4 + 1] = y2;
      vertices[i * 24 + 0 * 4 + 2] = z2;

      vertices[i * 24 + 1 * 4 + 0] = x1;    // Vertex 2.
      vertices[i * 24 + 1 * 4 + 1] = y1;
      vertices[i * 24 + 1 * 4 + 2] = z2;

      vertices[i * 24 + 2 * 4 + 0] = x2;    // Vertex 3.
      vertices[i * 24 + 2 * 4 + 1] = y1;
      vertices[i * 24 + 2 * 4 + 2] = z2;

      vertices[i * 24 + 3 * 4 + 0] = x1;    // Vertex 4.
      vertices[i * 24 + 3 * 4 + 1] = y2;
      vertices[i * 24 + 3 * 4 + 2] = z2;

      vertices[i * 24 + 4 * 4 + 0] = x2;    // Vertex 5.
      vertices[i * 24 + 4 * 4 + 1] = y1;
      vertices[i * 24 + 4 * 4 + 2] = z2;

      vertices[i * 24 + 5 * 4 + 0] = x2;    // Vertex 6.
      vertices[i * 24 + 5 * 4 + 1] = y2;
      vertices[i * 24 + 5 * 4 + 2] = z2;
    } else if (i == 1) {
      vertices[i * 24 + 0 * 4 + 0] = x2;    // Vertex 1.
      vertices[i * 24 + 0 * 4 + 1] = y2;
      vertices[i * 24 + 0 * 4 + 2] = z1;

      vertices[i * 24 + 1 * 4 + 0] = x2;    // Vertex 2.
      vertices[i * 24 + 1 * 4 + 1] = y1;
      vertices[i * 24 + 1 * 4 + 2] = z1;

      vertices[i * 24 + 2 * 4 + 0] = x1;    // Vertex 3.
      vertices[i * 24 + 2 * 4 + 1] = y1;
      vertices[i * 24 + 2 * 4 + 2] = z1;

      vertices[i * 24 + 3 * 4 + 0] = x2;    // Vertex 4.
      vertices[i * 24 + 3 * 4 + 1] = y2;
      vertices[i * 24 + 3 * 4 + 2] = z1;

      vertices[i * 24 + 4 * 4 + 0] = x1;    // Vertex 5.
      vertices[i * 24 + 4 * 4 + 1] = y1;
      vertices[i * 24 + 4 * 4 + 2] = z1;

      vertices[i * 24 + 5 * 4 + 0] = x1;    // Vertex 6.
      vertices[i * 24 + 5 * 4 + 1] = y2;
      vertices[i * 24 + 5 * 4 + 2] = z1;
    } else if (i == 2) {
      vertices[i * 24 + 0 * 4 + 0] = x2;    // Vertex 1.
      vertices[i * 24 + 0 * 4 + 1] = y2;
      vertices[i * 24 + 0 * 4 + 2] = z2;

      vertices[i * 24 + 1 * 4 + 0] = x2;    // Vertex 2.
      vertices[i * 24 + 1 * 4 + 1] = y1;
      vertices[i * 24 + 1 * 4 + 2] = z2;

      vertices[i * 24 + 2 * 4 + 0] = x2;    // Vertex 3.
      vertices[i * 24 + 2 * 4 + 1] = y1;
      vertices[i * 24 + 2 * 4 + 2] = z1;

      vertices[i * 24 + 3 * 4 + 0] = x2;    // Vertex 4.
      vertices[i * 24 + 3 * 4 + 1] = y2;
      vertices[i * 24 + 3 * 4 + 2] = z2;

      vertices[i * 24 + 4 * 4 + 0] = x2;    // Vertex 5.
      vertices[i * 24 + 4 * 4 + 1] = y1;
      vertices[i * 24 + 4 * 4 + 2] = z1;

      vertices[i * 24 + 5 * 4 + 0] = x2;    // Vertex 6.
      vertices[i * 24 + 5 * 4 + 1] = y2;
      vertices[i * 24 + 5 * 4 + 2] = z1;
    } else if (i == 3) {
      vertices[i * 24 + 0 * 4 + 0] = x1;    // Vertex 1.
      vertices[i * 24 + 0 * 4 + 1] = y2;
      vertices[i * 24 + 0 * 4 + 2] = z1;

      vertices[i * 24 + 1 * 4 + 0] = x1;    // Vertex 2.
      vertices[i * 24 + 1 * 4 + 1] = y1;
      vertices[i * 24 + 1 * 4 + 2] = z1;

      vertices[i * 24 + 2 * 4 + 0] = x1;    // Vertex 3.
      vertices[i * 24 + 2 * 4 + 1] = y1;
      vertices[i * 24 + 2 * 4 + 2] = z2;

      vertices[i * 24 + 3 * 4 + 0] = x1;    // Vertex 4.
      vertices[i * 24 + 3 * 4 + 1] = y2;
      vertices[i * 24 + 3 * 4 + 2] = z1;

      vertices[i * 24 + 4 * 4 + 0] = x1;    // Vertex 5.
      vertices[i * 24 + 4 * 4 + 1] = y1;
      vertices[i * 24 + 4 * 4 + 2] = z2;

      vertices[i * 24 + 5 * 4 + 0] = x1;    // Vertex 6.
      vertices[i * 24 + 5 * 4 + 1] = y2;
      vertices[i * 24 + 5 * 4 + 2] = z2;
    } else if (i == 4) {
      vertices[i * 24 + 0 * 4 + 0] = x1;    // Vertex 1.
      vertices[i * 24 + 0 * 4 + 1] = y2;
      vertices[i * 24 + 0 * 4 + 2] = z1;

      vertices[i * 24 + 1 * 4 + 0] = x1;    // Vertex 2.
      vertices[i * 24 + 1 * 4 + 1] = y2;
      vertices[i * 24 + 1 * 4 + 2] = z2;

      vertices[i * 24 + 2 * 4 + 0] = x2;    // Vertex 3.
      vertices[i * 24 + 2 * 4 + 1] = y2;
      vertices[i * 24 + 2 * 4 + 2] = z2;

      vertices[i * 24 + 3 * 4 + 0] = x1;    // Vertex 4.
      vertices[i * 24 + 3 * 4 + 1] = y2;
      vertices[i * 24 + 3 * 4 + 2] = z1;

      vertices[i * 24 + 4 * 4 + 0] = x2;    // Vertex 5.
      vertices[i * 24 + 4 * 4 + 1] = y2;
      vertices[i * 24 + 4 * 4 + 2] = z2;

      vertices[i * 24 + 5 * 4 + 0] = x2;    // Vertex 6.
      vertices[i * 24 + 5 * 4 + 1] = y2;
      vertices[i * 24 + 5 * 4 + 2] = z1;
    } else if (i == 5) {
      vertices[i * 24 + 0 * 4 + 0] = x2;    // Vertex 1.
      vertices[i * 24 + 0 * 4 + 1] = y1;
      vertices[i * 24 + 0 * 4 + 2] = z1;

      vertices[i * 24 + 1 * 4 + 0] = x2;    // Vertex 2.
      vertices[i * 24 + 1 * 4 + 1] = y1;
      vertices[i * 24 + 1 * 4 + 2] = z2;

      vertices[i * 24 + 2 * 4 + 0] = x1;    // Vertex 3.
      vertices[i * 24 + 2 * 4 + 1] = y1;
      vertices[i * 24 + 2 * 4 + 2] = z2;

      vertices[i * 24 + 3 * 4 + 0] = x2;    // Vertex 4.
      vertices[i * 24 + 3 * 4 + 1] = y1;
      vertices[i * 24 + 3 * 4 + 2] = z1;

      vertices[i * 24 + 4 * 4 + 0] = x1;    // Vertex 5.
      vertices[i * 24 + 4 * 4 + 1] = y1;
      vertices[i * 24 + 4 * 4 + 2] = z2;

      vertices[i * 24 + 5 * 4 + 0] = x1;    // Vertex 6.
      vertices[i * 24 + 5 * 4 + 1] = y1;
      vertices[i * 24 + 5 * 4 + 2] = z1;
    }

    for (j = 0; j < 6; ++j) {
      colors[i * 18 + j * 3 + 0]    = colorVertices[i][0];
      colors[i * 18 + j * 3 + 1]    = colorVertices[i][1];
      colors[i * 18 + j * 3 + 2]    = colorVertices[i][2];
      vertices[i * 24 + j * 4 + 3]  = 1.f;
    }
  }

  return model;
}

// TODO: Finish and test the functionality.
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

      // An error occured while reading.
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
 * Parses the FBX ASCII file content for the first model/mesh it finds.
 * @remarks       INCOMPLETE.
 * @param  data   The file string content.
 * @return        NULL if it fails, else a pointer to a Model.
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
  
  // Get the vertices chunck.
  verticesData = reachField_FBX_ASCII(modelData, &vertexDataLength, "Vertices");
  vertices = malloc(sizeof(char) * (vertexDataLength + 1));
  strncpy(vertices, verticesData, vertexDataLength);
  vertices[vertexDataLength] = '\0';

  // Find how many vertices we have.
  for (i = 0, ptr = vertices; ptr[i]; ptr[i] == ',' ? ++i : i, ++ptr);

  // Get the indices.
  indicesData = reachField_FBX_ASCII(modelData, &indicesDataLength, "PolygonVertexIndex");
  indices = malloc(sizeof(char) * (indicesDataLength + 1));
  strncpy(indices, indicesData, indicesDataLength);
  indices[indicesDataLength] = '\0';

  // Find how many indices we have.
  for (j = 0, ptr = indices; ptr[j]; ptr[j] == ',' ? ++j : j, ++ptr);

  // Create and fill the object.
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
 * @remarks                     INCOMPLETE
 * @param  chunk                A pointer to the begining of the chunk to consider.
 * @param [in,out]  length      The length of said chunk.
 * @param  field                The field we are searching for.
 * @return                      NULL if it fails, else a valid pointer
 */
static const char *reachField_FBX_ASCII(const char *chunk, int *length, const char *field)
{
  const char *ptr = strstr(chunk, field);
  const char *columnPtr = strstr(ptr, ":");
  const char *endline = columnPtr++;
  const char *start = endline;
  char *lastComma;
  char line[2056];

  // A line ending with a newline preceeded by a comma signals data continues on the next line. 
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
 * @remarks                       INCOMPLETE
 * @param   data                  A pointer to the file content.
 * @param   [in,out]  length      The length of the chunk, this coupled with the returned address
 *                                define our chunk.
 * @param   [in,out]  chunkPath   The path to the inner most chunk we want, such as "Objects.Model".
 * @return                        NULL if it fails, else pointer to the chunk we seek.
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
 * Similar to 'readChunk_FBX_ASCII' but only concern itselfs with a single chunk (no nesting).
 * @remarks                       INCOMPLETE.
 * @param   data                  A pointer to the string we are searching into.
 * @param   [in,out]  Length      Will be modified to show the length of the chunk if found.
 * @param   token                 The chunk we are looking for.
 * @return                        NULL or valid chunk address.
 */
static const char *readSubChunk_FBX_ASCII(const char *data, int *length, const char *token)
{
  const char *ptr = strstr(data, token);

  if (ptr) {
    const char *openBrace = strstr(ptr, "{");
    const char *newLine = strstr(ptr, "\n");
    const char *cptr;
    int count = 1;

    // Newline is found before an open brace (meaning it is not a node).
    assert(newLine > openBrace);

    cptr = openBrace;
    while (count) {
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
