#include <stdio.h>
#include <math.h>
#include "platform.h"
#include "input.h"
#include "matrix.h"
#include "renderer.h"
#include "pipeline.h"
#include "frameratecontroller.h"
#include "object.h"

#ifndef _MSC_VER
#define sprintf_s(A, B, C, D) sprintf(A, C, D)
#endif

static Model *cube;
static float angle;

struct Camera {
  float position[4];
  float lookatdirection[4];
  float upvector[4];
};

static struct Camera camera;
static float znear = 15.f;
static float zfar = 5000.f;

#define ANIMATED 0

void setCamera(float _position[], float _lookatposition[], float _upvector[])
{
  float length;

  // Calculate and normalize the lookatdirection.
  camera.lookatdirection[0] = _lookatposition[0] - _position[0];
  camera.lookatdirection[1] = _lookatposition[1] - _position[1];
  camera.lookatdirection[2] = _lookatposition[2] - _position[2];
  camera.lookatdirection[3] = _lookatposition[3] - _position[3];
  length = sqrtf(camera.lookatdirection[0] * camera.lookatdirection[0] + camera.lookatdirection[1] * camera.lookatdirection[1] + camera.lookatdirection[2] * camera.lookatdirection[2] + camera.lookatdirection[3] * camera.lookatdirection[3]);
  camera.lookatdirection[0] /= length;
  camera.lookatdirection[1] /= length;
  camera.lookatdirection[2] /= length;
  camera.lookatdirection[3] /= length;

  // Normalize up vector.
  camera.upvector[0] = _upvector[0];
  camera.upvector[1] = _upvector[1];
  camera.upvector[2] = _upvector[2];
  camera.upvector[3] = _upvector[3];
  length = sqrtf(camera.upvector[0] * camera.upvector[0] + camera.upvector[1] * camera.upvector[1] + camera.upvector[2] * camera.upvector[2] + camera.upvector[3] * camera.upvector[3]);
  camera.upvector[0] /= length;
  camera.upvector[1] /= length;
  camera.upvector[2] /= length;
  camera.upvector[3] /= length;

  camera.position[0] = _position[0];
  camera.position[1] = _position[1];
  camera.position[2] = _position[2];
  camera.position[3] = _position[3];
}

static int mousex = 0;
static int mousey = 0;
static int oldmousex = 0;
static int oldmousey = 0;
static int windowWidth;
static int windowHeight;

void Initialize(void)
{
  float position[] = {0, 0, 0, 1};
  float lookatposition[] = {0, 0, -100, 1};
  float Y[] = {0, 1, 0, 0};

  // TODO: Ultimately window size and settings is to be read from config.
  setupWindow("CRasterizerWindow", 1500, 800, 1);
  
  windowWidth   = getWindowWidth();
  windowHeight  = getWindowHeight();

  setCamera(position, lookatposition, Y);

  frameRateControllerInitialize(60, 0);
  setupRenderer(1500, 800);

  cube = loadModelFromFile_FBX_ASCII("models\\cube.fbx");
  if (!cube)
    cube = createCube(1);
  angle = 0;
  setViewport(0, 0, (float)windowWidth, (float)windowHeight);
  setMatrixMode(PROJECTION);
  frustrum(znear/2, -znear/2, -znear/2 * windowHeight / windowWidth, znear/2 * windowHeight / windowWidth, znear, zfar);

  // Hide the cursor.
  showCursor(0);
  getCursorScreenPosition(&mousex, &mousey);
  oldmousex = mousex;
  oldmousey = mousey;
}

// Used to limit the mouse upward rotation range.
static float yanglelimit = 0.f;
static int cameramode = 1;
static int initialx = -1;
static int initialy = -1;

void updateCameraAndControls(unsigned int delta)
{
  Matrix4D crossup, camerarotateY, camerarotatetemp, tmpmatrix;
  float dx, dy, d, tempangle;
  float speed = 10.f * (delta/16.f);
  float length;
  float tmp[4], tmp2[4];
  int pnx, pny;

  if (isKeyTriggered('T'))
  {
    cameramode = !cameramode;
    if (cameramode)
    {
      oldmousex = initialx;
      oldmousey = initialy;
      moveCursorToClientArea();
    }
  }
  
  // Signal the applicaton that we want to quit.
  if (isKeyTriggered(KEY_ESCAPE))
    signalQuitApplication();

  // Reset the camera position.
  if (isKeyTriggered('C')) {
    yanglelimit = 0;
    float position[] = { 0, 0, 0, 1 };
    float lookatposition[] = { 0, 0, -100, 1 };
    float Y[] = { 0, 1, 0, 0 };
    setCamera(position, lookatposition, Y);
  }

  // Handling the camera controls, first orientation, second translation.
  getCursorScreenPosition(&pnx, &pny);
  mousex = pnx;
  mousey = pny;
  initialx = initialx == -1 ? mousex : initialx;
  initialy = initialy == -1 ? mousey : initialy;
  dx = (float)mousex - oldmousex;
  dy = (float)mousey - oldmousey;
  if (cameramode)
    setCursorScreenPosition(oldmousex, oldmousey);
  else
  {
    oldmousex = mousex;
    oldmousey = mousey;
  }
  
  // Crossing the camera up vector with the opposite of the look at direction.
  crossProductMatrix(crossup, camera.upvector);
  tmp[0] = -camera.lookatdirection[0];
  tmp[1] = -camera.lookatdirection[1];
  tmp[2] = -camera.lookatdirection[2];
  tmp[3] = -camera.lookatdirection[3];
  // 'tmp' is now orthogonal to the up and lookat vector. 
  transformVectorByMatrix(crossup, tmp);

  // Orthogonalizing the camera up and direction vector (to avoid floating point imprecision).
  // Discard the y and the w components (preserver x and z, making it parallel to the movement 
  // plane).
  tmp[1] = tmp[3] = 0;
  length = sqrtf(tmp[0] * tmp[0] + tmp[2] * tmp[2]);
  if (length != 0) {
    // Normalizes tmp and return cross product matrix.
    crossProductMatrix(crossup, tmp);
    tmp2[0] = camera.upvector[0];
    tmp2[1] = camera.upvector[1];
    tmp2[2] = camera.upvector[2];
    tmp2[3] = camera.upvector[3];
    // 'tmp2' is now the oppposite of the direction vector.
    transformVectorByMatrix(crossup, tmp2);
    camera.lookatdirection[0] = -tmp2[0];
    camera.lookatdirection[1] = -tmp2[1];
    camera.lookatdirection[2] = -tmp2[2];
    camera.lookatdirection[3] = -tmp2[3];
  }

  // Create a rotation matrix on the y relative the movement of the mouse on x.
  rotationMatrixY(camerarotateY, -dx / 1000);
  // Limit yangleLimit to a certain range (to control x range of movement).
  tempangle = yanglelimit;
  tempangle -= dy / 1000;
  d = -dy / 1000;
  if (tempangle > 0.525f)
    d = 0.525f - yanglelimit;
  else if (tempangle < -0.525f)
    d = -0.525f - yanglelimit;
  yanglelimit += d;
  // Find the rotation matrix along perpendicular axis.
  axisAngleMatrix(camerarotatetemp, tmp, d);
  // Switching the rotations here makes no difference, why? It seems geometrically the result is the
  // same. Just simulate it using your thumb and index.
  multiplyMatrices(camerarotateY, camerarotatetemp, tmpmatrix);
  transformVectorByMatrix(tmpmatrix, camera.lookatdirection);
  transformVectorByMatrix(tmpmatrix, camera.upvector);

  // Handling translations.
  if (isKeyPressed('A')) {
    camera.position[0] -= tmp[0] * speed;
    camera.position[2] -= tmp[2] * speed;
  }

  if (isKeyPressed('D')) {
    camera.position[0] += tmp[0] * speed;
    camera.position[2] += tmp[2] * speed;
  }

  if (isKeyPressed('Q')) {
    camera.position[1] -= tmp[0] * speed;
  }

  if (isKeyPressed('E')) {
    camera.position[1] += tmp[0] * speed;
  }

  if (isKeyPressed('W')) {
    float vecxz[4]= {0};
    vecxz[0] = camera.lookatdirection[0];
    vecxz[2] = camera.lookatdirection[2];
    length = sqrtf(vecxz[0] * vecxz[0] + vecxz[2] * vecxz[2]);
    if (length != 0) {
      vecxz[0] /= length;
      vecxz[2] /= length;
      camera.position[0] += vecxz[0] * speed;
      camera.position[2] += vecxz[2] * speed;
    }
  }

  if (isKeyPressed('S')) {
    float vecxz[4] = {0};
    vecxz[0] = camera.lookatdirection[0];
    vecxz[2] = camera.lookatdirection[2];
    length = sqrtf(vecxz[0] * vecxz[0] + vecxz[2] * vecxz[2]);
    if (length != 0) {
      vecxz[0] /= length;
      vecxz[2] /= length;
      camera.position[0] -= vecxz[0] * speed;
      camera.position[2] -= vecxz[2] * speed;
    }
  }
}

static char array[100] = { 0 };

void Update(void)
{
  clearColorAndDepthBuffer();

  keyboardUpdate();
  mouseUpdate();

  Matrix4D viewport, projection, scale, rotation, translation, worldtransformation, cameratranslation, camerarotation, viewtransformation, unused;
  Matrix4D crossup;
  float tmp[4];
  int l, k, m;
  int maxX, maxY, maxZ;

  // Get projection transformation, set model view matrix to identity.
  getViewportMatrix(viewport);
  setMatrixMode(PROJECTION);
  getMatrix(projection);
  setMatrixMode(MODELVIEW);
  loadIdentity();

  // Update the camera and the controls. 
  updateCameraAndControls(frameRateControllerStart());

  // Camera transformation. 
  translationMatrix(cameratranslation, -(camera.position[0] - camera.lookatdirection[0] * znear), -(camera.position[1] - camera.lookatdirection[1] * znear), -(camera.position[2] - camera.lookatdirection[2] * znear));
  crossProductMatrix(crossup, camera.upvector);
  tmp[0] = -camera.lookatdirection[0];
  tmp[1] = -camera.lookatdirection[1];
  tmp[2] = -camera.lookatdirection[2];
  tmp[3] = -camera.lookatdirection[3];
  transformVectorByMatrix(crossup, tmp);
  // 'tmp' now perpendicular to the up and direction vector. 
  setIdentity(camerarotation);
  camerarotation[0] = tmp[0];
  camerarotation[4] = tmp[1];
  camerarotation[8] = tmp[2];
  camerarotation[1] = camera.upvector[0];
  camerarotation[5] = camera.upvector[1];
  camerarotation[9] = camera.upvector[2];
  camerarotation[2] = -camera.lookatdirection[0];
  camerarotation[6] = -camera.lookatdirection[1];
  camerarotation[10] = -camera.lookatdirection[2];

  multiplyMatrices(camerarotation, cameratranslation, viewtransformation);
  postMultiply(viewtransformation);

#if ANIMATED
  angle += 1/180.f * 3.1415f;
#endif

  maxX = 10;
  maxY = 5;
  maxZ = 10;

  // Draw a grid of cubes.
  for (l = 0; l < maxX; ++l) {
    for (m = 0; m < maxY; ++m) {
      for (k = 0; k < maxZ; ++k) {

        pushMatrix();

        // Scale, rotate, translate.
        setIdentity(scale);
        scaleMatrix(scale, 50, 50, 50);
        setIdentity(rotation);
        rotationMatrixY(rotation, angle);
        setIdentity(translation);
#if ANIMATED
        translationMatrix(translation, 300.f * l - 300.f * maxX / 2, m * 200.f - 150.f + sinf(angle + (3.f * (maxX/4.f)) * (l/(float)maxX) + (1.f * (maxZ/4.f)) * (k/(float)maxZ)) * 200 -400, -700.f - 200.f * k);
#else
        translationMatrix(translation, 300.f * l - 300.f * maxX / 2, m * 200.f - 400, -700.f - 200.f * k);
#endif
        // World transformation (without camera). 
        multiplyMatrices(translation, rotation, worldtransformation);
        multiplyMatrices(worldtransformation, scale, worldtransformation);

        // Premultiply with camera transformation, yielding effectively a model-view matrix. 
        preMultiply(worldtransformation);

        {
          int vertexnumber, indexnumber;
          const float *vertices   = getVertexArray(cube, &vertexnumber);
          const float *colors     = getColorArray(cube, &vertexnumber);
          const int *indices      = getIndexArray(cube, &indexnumber);
          render(vertices, colors, vertexnumber, indices, indexnumber);
        }
        
        popMatrix(unused);
      }
    }
  }

  swapBuffer();

  sprintf_s(array, sizeof(array), "C Project: %ifps", getActualFrameRate());
  setWindowTitle(array);

  frameRateControllerEnd();
}

void Cleanup(void)
{
  frameRateControllerRelease();
  releaseRenderer();
  cleanupWindow();

  if (cube)
    freeModel(cube);
  cube = NULL;
}