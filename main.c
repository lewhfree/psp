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

#define BUF_WIDTH  (512)
#define SCR_WIDTH  (480)
#define SCR_HEIGHT (272)

PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[262144];

struct Vertex {
    float u, v;           // Texture coordinates (required even if not used for proper alignment)
    unsigned int color;   // Color
    float nx, ny, nz;     // Normal vector
    float x, y, z;        // Position (must be last)
};

#define MAX_TRIANGLES 4096
static struct Vertex __attribute__((aligned(16))) dynamicVertices[MAX_TRIANGLES * 3];

unsigned int normalToColor(float nx, float ny, float nz) {
    float len = sqrtf(nx*nx + ny*ny + nz*nz);
    if (len > 0.0f) {
        nx /= len;
        ny /= len;
        nz /= len;
    }
    
    // Convert from [-1,1] range to [0,255] range
    unsigned char r = (unsigned char)((nx + 1.0f) * 0.5f * 255.0f);
    unsigned char g = (unsigned char)((ny + 1.0f) * 0.5f * 255.0f);
    unsigned char b = (unsigned char)((nz + 1.0f) * 0.5f * 255.0f);
    unsigned char a = 255; // Full alpha
    
    return (a << 24) | (b << 16) | (g << 8) | r;
}

void convertTrianglesToVertices(const Triangle* triangles, size_t numTriangles) {
    if (numTriangles > MAX_TRIANGLES) return;

    for (size_t i = 0; i < numTriangles; ++i) {
        const Triangle* tri = &triangles[i];
        
        float nx = tri->normal.x;
        float ny = tri->normal.y;
        float nz = tri->normal.z;
        
        unsigned int color = normalToColor(nx, ny, nz);
        
        dynamicVertices[i * 3 + 0] = (struct Vertex){
            0.0f, 0.0f,    // Texture coordinates (not used but needed)
            color,         // Color
            nx, ny, nz,    // Normal
            tri->v1.x, tri->v1.y, tri->v1.z  // Position
        };
        dynamicVertices[i * 3 + 1] = (struct Vertex){
            0.0f, 0.0f,
            color,
            nx, ny, nz,
            tri->v2.x, tri->v2.y, tri->v2.z
        };
        dynamicVertices[i * 3 + 2] = (struct Vertex){
            0.0f, 0.0f,
            color,
            nx, ny, nz,
            tri->v3.x, tri->v3.y, tri->v3.z
        };
    }
}

int main(int argc, char *argv[]) {
    setup_callbacks();
    pspDebugScreenInit();
    pspDebugScreenPrintf("Loading STL...\n");

    STLModel model;
    if (load_binary_stl("ms0:/PSP/GAME/hello/cube.stl", &model) == 0) {
        pspDebugScreenPrintf("Failed to load STL file!\n");
        sceKernelDelayThread(3 * 1000000);
        return -1;
    }

    pspDebugScreenPrintf("Model OK: %u triangles\n", (unsigned int)model.triangleCount);
    convertTrianglesToVertices(model.triangles, model.triangleCount);
//    sceKernelDelayThread(1 * 1000000);

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

    uint64_t ticks;
    int val = 0;

    while (1) {
        sceGuStart(GU_DIRECT, list);

        sceGuClearColor(0xFF10ff80);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(80.0f, 16.0f / 9.0f, 0.5f, 1000.0f);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        ScePspFVector3 eye = { 0.0f, 0.0f, -50.0f };      // Camera position
        ScePspFVector3 center = { 0.0f, 0.0f, 0.0f };   // Where the camera is looking
        ScePspFVector3 up = { 0.0f, 1.0f, 0.0f };
        sceGumLookAt(&eye, &center, &up);

        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
        {
            ScePspFVector3 pos = { 0, 0, 0 };
            // ScePspFVector3 rot = {
            //     val * 0.79f * (GU_PI / 180.0f),
            //     val * 0.98f * (GU_PI / 180.0f),
            //     val * 1.32f * (GU_PI / 180.0f)
            // };
            float time = val * 0.016f;  // Assuming ~60fps, this gives smooth time progression
ScePspFVector3 rot = {
    fmodf(time * 0.79f, 2.0f * GU_PI),
    fmodf(time * 0.98f, 2.0f * GU_PI),
    fmodf(time * 1.32f, 2.0f * GU_PI)
};
            sceGumTranslate(&pos);
            sceGumRotateXYZ(&rot);
        }

        sceGumDrawArray(
            GU_TRIANGLES,
            GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
            model.triangleCount * 3,
            0,
            dynamicVertices);

        val = (val + 1) % 100;
        uint64_t old = ticks;
        sceRtcGetCurrentTick(&ticks);
        old = ticks - old;

        float fold = (float) old;
        float fps = 1000000.0f / fold;
        old = (uint8_t) fps;
        int intold = (int) old;
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf("FPS: %d\n", intold);

        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    free_stl(&model);
    return 0;
}
