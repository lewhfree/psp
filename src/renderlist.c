#include "../include/renderlist.h"
#include "../include/render.h"
#include <stdlib.h>
#define MAXMODELS 100
void renderAll(Modellist* modelList){
  renderModel(&(modelList->models[0]));
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
