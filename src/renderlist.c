#include "../include/renderlist.h"
#include "../include/render.h"
#include <stdlib.h>
#define MAXMODELS 100
void renderAll(Modellist* modelList){
  int numModels = modelList->firstItem + 1;
  for(int i = 0; i < numModels; i++){
    renderModel(&(modelList->models[i]));
  }
}

Modellist* initList(){
  Modellist* temp = malloc(sizeof(Modellist));
  temp->firstItem = -1;  
  return temp;  
}

void addModel(Model* newModel, Modellist* modelList){
  modelList->firstItem++;
  modelList->models[modelList->firstItem] = *newModel;
}

void deleteModel(unsigned int index, Modellist* modelList){
  modelList->models[index].isValid = 0;
  freeModel(&modelList->models[index]);
}
