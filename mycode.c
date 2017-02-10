#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "input.h"
#include "matrix.h"
#include "input.h"
#include "renderer.h"
#include "pipeline.h"
#include "frameratecontroller.h"
#include "object.h"

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

void setCamera(float _position[], float _lookatposition[], float _upvector[])
{
	float length;

	/* Calculate and normalize the lookatdirection. */
	camera.lookatdirection[0] = _lookatposition[0] - _position[0];
	camera.lookatdirection[1] = _lookatposition[1] - _position[1];
	camera.lookatdirection[2] = _lookatposition[2] - _position[2];
	camera.lookatdirection[3] = _lookatposition[3] - _position[3];
	length = sqrtf(camera.lookatdirection[0] * camera.lookatdirection[0] + camera.lookatdirection[1] * camera.lookatdirection[1] + camera.lookatdirection[2] * camera.lookatdirection[2] + camera.lookatdirection[3] * camera.lookatdirection[3]);
	camera.lookatdirection[0] /= length;
	camera.lookatdirection[1] /= length;
	camera.lookatdirection[2] /= length;
	camera.lookatdirection[3] /= length;

	/* Normalize up vector. */
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

void mycodeInitialize(void)
{
	extern int g_WindowWidth;
	extern int g_WindowHeight;

	POINT pn;
	
	float position[] = {0, 0, 0, 1};
	float lookatposition[] = {0, 0, -100, 1};
	float Y[] = {0, 1, 0, 0};
	setCamera(position, lookatposition, Y);

	cube = loadModelFromFile_FBX_ASCII("models\\cube.fbx");
	if (!cube)
		cube = createCube(1);
	angle = 0;
	setViewport(0, 0, (float)g_WindowWidth, (float)g_WindowHeight);
	setMatrixMode(PROJECTION);
	frustrum(znear/2, -znear/2, -znear/2 * g_WindowHeight/g_WindowWidth, znear/2 * g_WindowHeight/g_WindowWidth, znear, zfar);

	/* Hide the cursor. */
	ShowCursor(0);
	GetCursorPos(&pn);
	oldmousex = mousex = pn.x;
	oldmousey = mousey = pn.y;
}

/* Used to limit the mouse upward rotation range. */
static float yanglelimit = 0.f;

void updateCameraAndControls(unsigned int delta)
{
	Matrix4D crossup, camerarotateY, camerarotatetemp, tmpmatrix;
	float dx, dy, d, tempangle;
	float speed = 10.f;
	float length;
	POINT pn;
	float tmp[4], tmp2[4];

	/* Post a WM_QUIT message in case escape is triggered. */
	if (isKeyTriggered(VK_ESCAPE))
		PostQuitMessage(0);

	/* Reset the camera position. */
	if (isKeyTriggered('C')) {
		yanglelimit = 0;
		float position[] = { 0, 0, 0, 1 };
		float lookatposition[] = { 0, 0, -100, 1 };
		float Y[] = { 0, 1, 0, 0 };
		setCamera(position, lookatposition, Y);
	}

	/* Handling the camera controls, first orientation, second translation. */
	GetCursorPos(&pn);
	mousex = pn.x;
	mousey = pn.y;
	dx = (float)mousex - oldmousex;
	dy = (float)mousey - oldmousey;
	SetCursorPos(oldmousex, oldmousey);

	/* Crossing the camera up vector with the opposite of the look at direction. */
	crossProductMatrix(crossup, camera.upvector);
	tmp[0] = -camera.lookatdirection[0];
	tmp[1] = -camera.lookatdirection[1];
	tmp[2] = -camera.lookatdirection[2];
	tmp[3] = -camera.lookatdirection[3];
	/* 'tmp' is now orthogonal to the up and lookat vector. */
	transformVectorByMatrix(crossup, tmp);

	/* Orthogonalizing the camera up and direction vector (to avoid floating point imprecision).
	Discard the y and the w components (preserver x and z, making it parallel to the movement 
	plane). */
	tmp[1] = tmp[3] = 0;
	length = sqrtf(tmp[0] * tmp[0] + tmp[2] * tmp[2]);
	if (length != 0) {
		/* Normalizes tmp and return cross product matrix. */
		crossProductMatrix(crossup, tmp);
		tmp2[0] = camera.upvector[0];
		tmp2[1] = camera.upvector[1];
		tmp2[2] = camera.upvector[2];
		tmp2[3] = camera.upvector[3];
		/* 'tmp2' is now the oppposite of the direction vector. */
		transformVectorByMatrix(crossup, tmp2);
		camera.lookatdirection[0] = -tmp2[0];
		camera.lookatdirection[1] = -tmp2[1];
		camera.lookatdirection[2] = -tmp2[2];
		camera.lookatdirection[3] = -tmp2[3];
	}

	/* Create a rotation matrix on the y relative the movement of the mouse on x. */
	rotationMatrixY(camerarotateY, -dx / 1000);
	/* Limit yangleLimit to a certain range (to control x range of movement). */
	tempangle = yanglelimit;
	tempangle -= dy / 1000;
	d = -dy / 1000;
	if (tempangle > 0.525f)
		d = 0.525f - yanglelimit;
	else if (tempangle < -0.525f)
		d = -0.525f - yanglelimit;
	yanglelimit += d;
	/* Find the rotation matrix along perpendicular axis. */
	axisAngleMatrix(camerarotatetemp, tmp, d);
	/* Switching the rotations here makes no difference, why???, it seems geometrically the 
	result is the same. Just simulate it using your thumb and index. */
	multiplyMatrices(camerarotateY, camerarotatetemp, tmpmatrix);
	transformVectorByMatrix(tmpmatrix, camera.lookatdirection);
	transformVectorByMatrix(tmpmatrix, camera.upvector);

	/* Handling translations. */
	if (isKeyPressed('A')) {
		camera.position[0] -= tmp[0] * speed;
		camera.position[2] -= tmp[2] * speed;
	}

	if (isKeyPressed('D')) {
		camera.position[0] += tmp[0] * speed;
		camera.position[2] += tmp[2] * speed;
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

void mycodeUpdate(unsigned int delta)
{
	extern int g_WindowWidth;
	extern int g_WindowHeight;

	Matrix4D viewport, projection, scale, rotation, translation, worldtransformation, cameratranslation, camerarotation, viewtransformation, unused;
	Matrix4D crossup;
	float tmp[4];
	int l, k;
	int maxX, maxZ;

	/* Getting the projection transformation. */
	getViewportMatrix(viewport);
	setMatrixMode(PROJECTION);
	getMatrix(projection);
	setMatrixMode(MODELVIEW);
	loadIdentity();

	/* Update the camera and the controls. */
	updateCameraAndControls(delta);

	/* Camera transformation. */
	translationMatrix(cameratranslation, -(camera.position[0] - camera.lookatdirection[0] * znear), -(camera.position[1] - camera.lookatdirection[1] * znear), -(camera.position[2] - camera.lookatdirection[2] * znear));
	crossProductMatrix(crossup, camera.upvector);
	tmp[0] = -camera.lookatdirection[0];
	tmp[1] = -camera.lookatdirection[1];
	tmp[2] = -camera.lookatdirection[2];
	tmp[3] = -camera.lookatdirection[3];
	transformVectorByMatrix(crossup, tmp);
	/* 'tmp' now perpendicular to the up and direction vector. */
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

	angle += 1/180.f * 3.1415f;

	maxX = 10;
	maxZ = 20;
	// Draw l * k cubes. */
	for (l = 0; l < maxX; ++l) {
		for (k = 0; k < maxZ; ++k) {
			pushMatrix();

			/* Scale matrix. */
			setIdentity(scale);
			scaleMatrix(scale, 50, 50, 50);
			/* Rotation matrix. */
			setIdentity(rotation);
			rotationMatrixY(rotation, angle);
			/* Translation matrix. */
			setIdentity(translation);
			translationMatrix(translation, 300.f * l - 300.f * maxX/2, -150.f + sinf(angle + (3.f * (maxX/4.f)) * (l/(float)maxX) + (1.f * (maxZ/4.f)) * (k/(float)maxZ)) * 200 - 400, -700.f - 200.f * k);
			/* World transformation (without camera). */
			multiplyMatrices(translation, rotation, worldtransformation);
			multiplyMatrices(worldtransformation, scale, worldtransformation);

			/* Multiply world transformation with camera transformation, yielding effectively a 
			model view tranformation. */
			preMultiply(worldtransformation);

			renderModel(cube);

			/* Pop the matrix. */
			popMatrix(unused);
		}
	}
}

void mycodeEnd(void)
{
	if (cube)
		freeModel(cube);
	cube = NULL;
}