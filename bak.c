#include <pspkernel.h>
#include <pspdisplay.h>
#include "include/stlloader.h"
#include <pspiofilemgr.h>
#include <string.h>
#include <psprtc.h>
#include <inttypes.h>
#include <pspgu.h>
#include "include/boilerplate.h"
#include <pspgum.h>
#include <stdlib.h>

static unsigned int __attribute__((aligned(16))) list[262144];

struct Vertex
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

struct Vertex __attribute__((aligned(16))) vertices[12*3] =
{
	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 0, 0xff7f0000,-1, 1, 1}, // 4
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5

	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5
	{0, 1, 0xff7f0000, 1,-1, 1}, // 1

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 0, 0xff7f0000, 1,-1,-1}, // 2
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6
	{0, 1, 0xff7f0000,-1, 1,-1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 0, 0xff007f00, 1,-1, 1}, // 3
	{1, 1, 0xff007f00, 1, 1, 1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 1, 0xff007f00, 1, 1, 1}, // 7
	{0, 1, 0xff007f00, 1, 1,-1}, // 4

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 0, 0xff007f00,-1, 1,-1}, // 3
	{1, 1, 0xff007f00,-1, 1, 1}, // 7

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 1, 0xff007f00,-1, 1, 1}, // 7
	{0, 1, 0xff007f00,-1,-1, 1}, // 4

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 0, 0xff00007f, 1, 1,-1}, // 1
	{1, 1, 0xff00007f, 1, 1, 1}, // 2

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 1, 0xff00007f, 1, 1, 1}, // 2
	{0, 1, 0xff00007f,-1, 1, 1}, // 3

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 0, 0xff00007f,-1,-1, 1}, // 7
	{1, 1, 0xff00007f, 1,-1, 1}, // 6

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 1, 0xff00007f, 1,-1, 1}, // 6
	{0, 1, 0xff00007f, 1,-1,-1}, // 5
};

struct Vertex* convertTrianglesToVertices(const Triangle* triangles, size_t numTriangles) {
    struct Vertex* vertices = malloc(sizeof(struct Vertex) * numTriangles * 3);
    if (!vertices) return NULL;

    for (size_t i = 0; i < numTriangles; ++i) {
        const Triangle* tri = &triangles[i];

        // Create 3 vertices per triangle
        vertices[i * 3 + 0] = (struct Vertex){0.0f, 0.0f, 0x00000000, tri->v1.x, tri->v1.y, tri->v1.z};
        vertices[i * 3 + 1] = (struct Vertex){0.0f, 0.0f, 0x00000000, tri->v2.x, tri->v2.y, tri->v2.z};
        vertices[i * 3 + 2] = (struct Vertex){0.0f, 0.0f, 0x00000000, tri->v3.x, tri->v3.y, tri->v3.z};
    }

    return vertices;
}

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);


int main(int argc, char *argv[])  {
    setup_callbacks();

    char path[128];
    strncpy(path, argv[0], sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';
    char *lastSlash = strrchr(path, '/');
    if (lastSlash != NULL) {
        *lastSlash = '\0';
    }
    pspDebugScreenInit();
    
    STLModel model;
    load_binary_stl("ms0:/PSP/GAME/hello/cube.stl", &model);
    
    uint64_t ticks;
    pspDebugScreenClear();
    void* fbp0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
    void* fbp1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
    void* zbp = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);
    
    sceGuInit();

		sceGuStart(GU_DIRECT,list);
		sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
		sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
		sceGuDepthBuffer(zbp,BUF_WIDTH);
		sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
		sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
		sceGuDepthRange(65535,0);
		sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
		sceGuEnable(GU_SCISSOR_TEST);
		sceGuDepthFunc(GU_GEQUAL);
		sceGuEnable(GU_DEPTH_TEST);
		sceGuFrontFace(GU_CW);
		sceGuShadeModel(GU_SMOOTH);
		sceGuEnable(GU_CULL_FACE);
		sceGuEnable(GU_TEXTURE_2D);
		sceGuEnable(GU_CLIP_PLANES);
		sceGuFinish();
		sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

		sceDisplayWaitVblankStart();
		sceGuDisplay(GU_TRUE);
    int val = 0;

    while(1) {
    	sceGuStart(GU_DIRECT,list);
			// clear screen

			sceGuClearColor(0xFFED9564);
			sceGuClearDepth(0);
			sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

			// setup matrices for cube

			sceGumMatrixMode(GU_PROJECTION);
			sceGumLoadIdentity();
			sceGumPerspective(75.0f,16.0f/9.0f,0.5f,1000.0f);

			sceGumMatrixMode(GU_VIEW);
			sceGumLoadIdentity();

			sceGumMatrixMode(GU_MODEL);
			sceGumLoadIdentity();
			{
				ScePspFVector3 pos = { 0, 0, -2.5f };
				ScePspFVector3 rot = { val * 0.79f * (GU_PI/180.0f), val * 0.98f * (GU_PI/180.0f), val * 1.32f * (GU_PI/180.0f) };
				sceGumTranslate(&pos);
				sceGumRotateXYZ(&rot);
			}

			// setup texture

			 sceGuTexMode(GU_PSM_4444,0,0,0);
			// sceGuTexImage(0,64,64,64,aalogo_start);
			 sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
			 sceGuTexEnvColor(0xffff00);
			 sceGuTexFilter(GU_LINEAR,GU_LINEAR);
			 sceGuTexScale(1.0f,1.0f);
			 sceGuTexOffset(0.0f,0.0f);
			sceGuAmbientColor(0xffffffff);

			// draw cube

			sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,vertices);

			val++;

        
      uint64_t old = ticks;
      sceRtcGetCurrentTick(& ticks);
      old = ticks - old;

    	float fold = (float) old;
      float fps = 1000000.0f / fold;
      old = (uint8_t) fps;
      int intold = (int) old;
      pspDebugScreenSetXY(0,0);
    	pspDebugScreenPrintf("%" PRIu8 "\n", intold);
      sceGuFinish();                                 
      sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
      sceDisplayWaitVblankStart();
      sceGuSwapBuffers();
    }
    return 0;
}
