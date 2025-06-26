#ifndef RENDER_MODEL_H
#define RENDER_MODEL_H

#include "stlloader.h"
#include <pspgu.h>
STLModel loadModel(char* filename);
void freeModel(STLModel* model);
void renderModel(STLModel* model, ScePspFVector3 position, ScePspFVector3 rotation);

#endif
