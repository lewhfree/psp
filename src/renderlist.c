#include "../include/renderlist.h"
#include "../include/render.h"
#include <stdlib.h>
#include <math.h>
float distanceBetweenVectors(ScePspFVector3 a, ScePspFVector3 b) {
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  float dz = a.z - b.z;
  return sqrt(dx * dx + dy * dy + dz * dz);
}

void renderAll(Modellist* modelList, ScePspFVector3 eye){
  int numModels = modelList->firstItem + 1;
  for(int i = 0; i < numModels; i++){
    float distToCamera = distanceBetweenVectors(eye, modelList->models[i]->position);
    if(modelList->models[i]->isValid){
      if(distToCamera <= 1000.0f){
        renderModel(modelList->models[i]);
      } else if (distToCamera > 2000.0f){
        //free the memory
      }
    } else {
      if(distToCamera< 1200.0f) {
        //allocate new mem
      }
    }
  }
}

Modellist* initList(){
  Modellist* temp = malloc(sizeof(Modellist));
  temp->firstItem = -1;  
  return temp;  
}

void addModel(Model* newModel, Modellist* modelList){
  modelList->firstItem++;
  modelList->models[modelList->firstItem] = newModel;
}

void deleteModel(unsigned int index, Modellist* modelList){
  modelList->models[index]->isValid = 0;
  freeModel(modelList->models[index]);
}
