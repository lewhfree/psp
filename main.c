#include <pspkernel.h>
#include <pspdisplay.h>
#include "include/stlloader.h"
#include <pspiofilemgr.h>
#include <string.h>
#include <psprtc.h>
#include <inttypes.h>
#include <pspgu.h>
#include "include/boilerplate.h"
static unsigned int __attribute__((aligned(16))) list[262144];

// PSP_MODULE_INFO is required
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
    
    // STLModel model;
    // load_binary_stl("ms0:/PSP/GAME/hello/cube.stl", &model);
    uint64_t ticks;
    pspDebugScreenClear();

    void * VramBuffer = guGetStaticVramBuffer(512, 272, GU_PSM_5650);
    sceGuInit();
    // sceGuStart(GU_DIRECT, list)
    while(1) {
        
        // sceGuBeginObject(GU_VERTEX_32BITF, 8, 0, 0);
        //model used is named model
        

        
        uint64_t old = ticks;
        sceRtcGetCurrentTick(& ticks);
        old = ticks - old;

        float fold = (float) old;
        float fps = 1000000.0f / fold;
        old = (uint8_t) fps;
        int intold = (int) old;
        pspDebugScreenSetXY(0,0);
        pspDebugScreenPrintf("%" PRIu8 "\n", intold);
        sceDisplayWaitVblankStart();
    }
    return 0;
}
