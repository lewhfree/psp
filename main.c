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
#include "include/render_model.h"

#define BUF_WIDTH  (512)
#define SCR_WIDTH  (480)
#define SCR_HEIGHT (272)
#define ANGLE_STEP (0.1f)
#define GRIDW 2
#define GRIDH 2
#define GROUNDSIZE 200
PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
static unsigned int __attribute__((aligned(16))) list[262144];

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv; //use this to later fix paths. full path needed rn, first argument is path to exe
    setup_callbacks();
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    STLModel* teapot = loadModel("ms0:/PSP/GAME/hello/teapot.stl");
    STLModel *groundArray[GRIDW][GRIDH];
    char *filenames[GRIDW][GRIDH];
    uint8_t isLoaded[GRIDW][GRIDH];
    filenames[0][0] = "ms0:/PSP/GAME/hello/plane0101.stl";
    filenames[0][1] = "ms0:/PSP/GAME/hello/plane0101.stl";
    filenames[1][0] = "ms0:/PSP/GAME/hello/plane0101.stl";
    filenames[1][1] = "ms0:/PSP/GAME/hello/plane0101.stl";
    // here loadmodel is used. I could add something that loads/unloads only if im in range;
    for(int i = 0; i < GRIDW; i++){
        for(int j = 0; j < GRIDH; j++){
            STLModel * temp = loadModel(filenames[i][j]);
            groundArray[i][j] = temp;
            isLoaded[i][j] = 1;
        }
    }
    
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

    float playerZ = -20.0f;
    float playerX = 0.0f;
    float playerY = 0.0f;
    float playerYaw = 0.0f;
    float playerPitch = 0.0f;

    float playerCenterX = 0.0f;
    float playerCenterY = 0.0f;
    float playerCenterZ = 0.0f;
    float pi32 = 3 * GU_PI / 2;    

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

        ScePspFVector3 modelpos = {0, 10.0f, 0};
        ScePspFVector3 modelrot = {pi32, 0.0f, 0.0f};
        renderModel(teapot, modelpos, modelrot);

        for(int i = 0; i < GRIDW; i++){
            for (int j = 0; j < GRIDH; j++){
                ScePspFVector3 platePos = {GROUNDSIZE * i, 0, GROUNDSIZE * j};
                ScePspFVector3 plateRot = {pi32, 0, 0};
                float distance = sqrtf(powf(platePos.x - eye.x, 2) + powf(platePos.y - eye.y, 2) + powf(platePos.z - eye.z, 2));
                if(isLoaded[i][j]){
                    renderModel(groundArray[i][j], platePos, plateRot);
                    if((distance > 500.0f) && (isLoaded[i][j])){
                        freeModel(groundArray[i][j]);
                        groundArray[i][j] = NULL;
                        isLoaded[i][j] = 0;
                    }
                } else {
                    if((distance < 500.0f) && !(isLoaded[i][j])){
                        STLModel* temp = loadModel(filenames[i][j]);
                        groundArray[i][j] = temp;
                        isLoaded[i][j] = 1;
                    }
                }
            }
        }
        //freeing
        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    free_stl(teapot);
    return 0;
}
