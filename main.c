#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdebug.h>
#include <pspctrl.h> //keypad
#include <string.h> //strcp, file path
#include <stdlib.h> //malloc/free
#include <inttypes.h> //uint
#include <math.h> //sqrt sin

#include "include/stlloader.h"
#include "include/boilerplate.h"
#include "include/graphics_setup.h"
// #include "include/ground.h"
#include "include/render.h"
#include "psptypes.h"
#define BUF_WIDTH  (512)
#define SCR_WIDTH  (480)
#define SCR_HEIGHT (272)
#define ANGLE_STEP (0.1f)
#define GRIDW 10
#define GRIDH 10
PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) list[262144];

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv; //use this to later fix paths. full path needed rn, first argument is path to exe
    boilerplate();
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

   
    Model* teapot = loadModel("ms0:/PSP/GAME/hello/teapot.stl", (ScePspFVector3){0, 0, 0}, (ScePspFVector3){0, 0, 0});
    // Model* groundArray[GRIDW][GRIDH];
    // char *filenames[GRIDW][GRIDH];
    // uint8_t isLoaded[GRIDW][GRIDH];
    // for(int i = 0; i < GRIDW; i++){
    //     for(int j = 0; j < GRIDH; j++){
    //         filenames[i][j] = "ms0:/PSP/GAME/hello/plane0102.stl";
    //         Model* temp = loadModel(filenames[i][j], (ScePspFVector3){i*200, 0, j*200}, (ScePspFVector3){0, 0, 0});
    //         groundArray[i][j] = temp;
    //         isLoaded[i][j] = 1;
    //     }
    // }
    
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
    // float pi32 = 3 * GU_PI / 2;    

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

        // sceGuClearColor(0xFFED9564);
        sceGuClearColor(0x00000000);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(70.0f, 16.0f / 9.0f, 0.5f, 3000.0f);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        ScePspFVector3 eye = realEye;      // Camera position
        ScePspFVector3 center = realCenter;   // Where the camera is looking
        ScePspFVector3 up = {0.0f, 1.0f, 0.0f};
        sceGumLookAt(&eye, &center, &up);

        // ScePspFVector3 modelpos = {0, 10.0f, 0};
        // ScePspFVector3 modelrot = {pi32, 0.0f, 0.0f};
        renderModel(teapot);
        // renderGround(GRIDW, GRIDH, groundArray, filenames, iLoaded, eye);
        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    freeModel(teapot);
    return 0;
}
