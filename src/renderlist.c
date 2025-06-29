#include "../include/renderlist.h"
#include "../include/render.h"
#include <stdlib.h>
#include <math.h>

#define CULLDIST 250.0f
#define FREEDIST 350.0f
#define ALLOCDIST 300.0f

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
      if(distToCamera <= CULLDIST){
        renderModel(modelList->models[i]);
      } else if (distToCamera > FREEDIST){
        //free the memory
        freeSTL(modelList->models[i]->model);
        modelList->models[i]->isValid = 0;
      }
    } else {
      if(distToCamera < ALLOCDIST) {
        //allocate new mem
        modelList->models[i]->model = loadSTL(modelList->models[i]->filename);
        modelList->models[i]->isValid = 1;
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
