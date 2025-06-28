#include "../include/stlloader.h"
#include "../include/render.h"
#include <pspgu.h>
#include <stdlib.h>

void renderModel(Model* model){
  renderSTL(model->model, model->position, model->rotation);
}

void freeModel(Model* model){
  freeSTL(model->model);
}

Model* loadModel(char* filename, ScePspFVector3 initPos, ScePspFVector3 initRot){
  Model* temp = malloc(sizeof(Model));
  temp->position = initPos;
  temp->rotation = initRot;
  temp->model = loadSTL(filename);
  temp->isValid = 1;
  return temp;    
}
