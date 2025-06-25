#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspiofilemgr.h>
#include <psprtc.h>
#include <pspdebug.h>
#include <pspctrl.h>

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
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
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
    //sceGuEnable(GU_CULL_FACE);
    sceGuDisable(GU_TEXTURE_2D);
    //sceGuEnable(GU_CLIP_PLANES);
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    int val = 0;
    float playerZ = -12.0f;
    float playerX = 0.0f;
    float playerY = 0.0f;
    float playerYaw = 0.0f;
    float playerPitch = 0.0f;
    while (1) {
        sceCtrlReadBufferPositive(&pad, 1);
        float aX = (pad.Lx- 127.0f) / 127.0f;
        float aY = -(pad.Ly- 127.0f) / 127.0f;
        
        playerX = playerX + aX;
        playerZ = playerZ + aY;
        
        if (pad.Buttons & PSP_CTRL_TRIANGLE){
            //LOOK UP
        }
        if (pad.Buttons & PSP_CTRL_CROSS){
            //LOOK DOWN
        }
        if (pad.Buttons & PSP_CTRL_CIRCLE){
            //LOOK RIGHT
        }
        if (pad.Buttons & PSP_CTRL_SQUARE){
            //LOOK LEFT
        }

        ScePspFVector3 realEye = {playerX, playerY, playerZ};
        ScePspFVector3 realCenter = {0.0f, 0.0f, 0.0f}; //lookingat
        sceGuStart(GU_DIRECT, list);

        sceGuClearColor(0xFFED9564);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(80.0f, 16.0f / 9.0f, 0.5f, 1000.0f);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        ScePspFVector3 eye = realEye;      // Camera position
        ScePspFVector3 center = realCenter;   // Where the camera is looking
        ScePspFVector3 up = {0.0f, 1.0f, 0.0f};

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
