#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspiofilemgr.h>
#include <psprtc.h>
#include <pspdebug.h>

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "include/stlloader.h"
#include "include/boilerplate.h"
#include "include/trianglethings.h"
#define BUF_WIDTH  (512)
#define SCR_WIDTH  (480)
#define SCR_HEIGHT (272)

PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[262144];

int main(int argc, char *argv[]) {
    setup_callbacks();

    STLModel model;
    if (load_binary_stl("ms0:/PSP/GAME/hello/cube.stl", &model) == 0) {
        sceKernelDelayThread(3 * 1000000);
        return -1;
    }

    convertTrianglesToVertices(model.triangles, model.triangleCount);

    void* fbp0 = guGetStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
    void* fbp1 = guGetStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
    void* zbp  = guGetStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_4444);

    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_5650, fbp0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, fbp1, BUF_WIDTH);
    sceGuDepthBuffer(zbp, BUF_WIDTH);
    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
    sceGuDepthRange(65535, 0);
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
//    sceGuEnable(GU_CULL_FACE);
    sceGuDisable(GU_TEXTURE_2D);  // No textures needed
    //sceGuEnable(GU_CLIP_PLANES);
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    int val = 0;

    while (1) {
        sceGuStart(GU_DIRECT, list);

        sceGuClearColor(0xFFED9564);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(80.0f, 16.0f / 9.0f, 0.5f, 1000.0f);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        ScePspFVector3 eye = { 0.0f, 0.0f, -12.0f };      // Camera position
        ScePspFVector3 center = { 0.0f, 0.0f, 0.0f };   // Where the camera is looking
        ScePspFVector3 up = { 0.0f, 1.0f, 0.0f };
        sceGumLookAt(&eye, &center, &up);

        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
        {
            ScePspFVector3 pos = { 0, 0, 0 };
            ScePspFVector3 rot = {
                val * 0.05f,// * 0.79f * (GU_PI / 180.0f),
                val * 0.05f,//lib32 * 0.98f * (GU_PI / 180.0f),
                val * 0.05f// * 1.32f * (GU_PI / 180.0f)
            };
            sceGumTranslate(&pos);
            sceGumRotateXYZ(&rot);
        }

        sceGumDrawArray(
            GU_TRIANGLES,
            GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
            model.triangleCount * 3,
            0,
            getDynamicVertexBuffer());

        val = (val + 1);
        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    free_stl(&model);
    return 0;
}
