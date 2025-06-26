#include <pspgu.h>
#include <pspgum.h>

#include "../include/stlloader.h"
#include "../include/render_model.h"
#include "../include/trianglethings.h"
STLModel loadModel(char * filename){
  STLModel model;
  load_binary_stl(filename, & model);
  return model;
}

void freeModel(STLModel* model){
  free_stl(model);  
}

void renderModel(STLModel* model, ScePspFVector3 position, ScePspFVector3 rotation){
  sceGumMatrixMode(GU_MODEL);
  sceGumLoadIdentity();
  sceGumTranslate(&position);
  sceGumRotateXYZ(&rotation);
  
  sceGumDrawArray(GU_TRIANGLES,
    GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
    model->triangleCount * 3,
    0,
    getDynamicVertexBuffer()
  );
}
