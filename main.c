#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#include "include/boilerplate.h"
#include "include/graphics_setup.h"
// #include "include/render.h"
#include "include/renderlist.h"
#include "psptypes.h"
#define BUF_WIDTH  (512)
#define SCR_WIDTH  (480)
#define SCR_HEIGHT (272)
#define ANGLE_STEP (0.1f)
#define MAXMODELS 100
PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) list[262144];
int main() {
    boilerplate();
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    Modellist* modelList = initList();
    addModel(loadModel("ms0:/PSP/GAME/hello/teapot.stl", (ScePspFVector3){0, 10, 0}, (ScePspFVector3){GU_PI/2*3, 0, 0}), modelList);   
    void* fbp0 = guGetStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
    void* fbp1 = guGetStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
    void* zbp  = guGetStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_4444);

    graphicsSetup(fbp0, fbp1, zbp, list);

    float playerZ = -20.0f;
    float playerX = 0.0f;
    float playerY = 0.0f;
    float playerYaw = 0.0f;
    float playerPitch = 0.0f;

    float playerCenterX = 0.0f;
    float playerCenterY = 0.0f;
    float playerCenterZ = 0.0f;

    while (1) {
        sceCtrlReadBufferPositive(&pad, 1);

        float rawX = pad.Lx - 128.0f;
        float rawY = pad.Ly - 128.0f;

        float magnitude = sqrtf(rawX * rawX + rawY * rawY);

        float DEADZONE = 20.0f;

        float aX = 0.0f;
        float aY = 0.0f;

        if (magnitude > DEADZONE) {
            float scale = (magnitude - DEADZONE) / (127.0f - DEADZONE);
            aX = (rawX / magnitude) * scale;
            aY = -(rawY / magnitude) * scale;
        }
       
        float forwardX = sinf(playerYaw);
        float forwardZ = cosf(playerYaw);
        float rightX = -cosf(playerYaw);
        float rightZ = sinf(playerYaw);

        playerX += forwardX * aY + rightX * aX;
        playerZ += forwardZ * aY + rightZ * aX;
                
        if (pad.Buttons & PSP_CTRL_TRIANGLE){
            playerPitch += ANGLE_STEP;
        }
        if (pad.Buttons & PSP_CTRL_CROSS){
            playerPitch -= ANGLE_STEP;
        }
        if (pad.Buttons & PSP_CTRL_CIRCLE){
            playerYaw -= ANGLE_STEP;
        }
        if (pad.Buttons & PSP_CTRL_SQUARE){
            playerYaw += ANGLE_STEP;
        }
        playerCenterX = playerX + sinf(playerYaw) * cosf(playerPitch);
        playerCenterY = playerY + sinf(playerPitch);
        playerCenterZ = playerZ + cosf(playerYaw) * cosf(playerPitch);

        ScePspFVector3 realEye = {playerX, playerY + 50, playerZ};
        ScePspFVector3 realCenter = {playerCenterX, playerCenterY + 50, playerCenterZ}; 

        sceGuStart(GU_DIRECT, list);

        sceGuClearColor(0xFFED9564);
        // sceGuClearColor(0x00000000);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(70.0f, 16.0f / 9.0f, 0.5f, 3000.0f);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        ScePspFVector3 eye = realEye;
        ScePspFVector3 center = realCenter;
        ScePspFVector3 up = {0.0f, 1.0f, 0.0f};
        sceGumLookAt(&eye, &center, &up);
        renderAll(modelList);
        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }
    // freeModel(models[0]);
    // models[0]  = NULL;
    return 0;
}
