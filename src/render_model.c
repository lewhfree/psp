#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>

#include "../include/stlloader.h"
#include "../include/render_model.h"

STLModel loadModel(char* filename){
    STLModel model;
    if(!load_binary_stl(filename, &model)) {
        model.triangles = NULL;
        model.vertices = NULL;
        model.triangleCount = 0;
    }
    
    if(model.triangleCount == 0) {
        sceKernelDelayThread(3000000);
        sceKernelExitGame();
    }
    return model;
}

void freeModel(STLModel* model){
    free_stl(model);  
}

void renderModel(STLModel* model, ScePspFVector3 position, ScePspFVector3 rotation){
    if(!model || !model->vertices || model->triangleCount == 0) {
        return; // Nothing to render
    }
    
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    sceGumTranslate(&position);
    sceGumRotateXYZ(&rotation);
    
    sceGumDrawArray(GU_TRIANGLES,
        GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
        model->triangleCount * 3,
        0,
        model->vertices  // Use model's own vertices
    );
}
