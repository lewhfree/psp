#ifndef RENDER_H
#define RENDER_H

#include <pspgu.h>
#include "stlloader.h"

typedef struct {
  ScePspFVector3 coordinate;
  ScePspFVector3 size;
} CubeCollider;

typedef struct {
  char* filename;
  ScePspFVector3 position;
  ScePspFVector3 rotation;
  STLModel* model;
  CubeCollider colliders[10];
  int isValid;  
} Model;
void freeModel(Model* model);
Model* loadModel(char* filename, ScePspFVector3 initPos, ScePspFVector3 initRot);
void renderModel(Model* model);
#endif
