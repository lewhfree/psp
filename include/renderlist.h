#ifndef RENDERLIST_H
#define RENDERLIST_H
#include "render.h"
#define MAXMODELS 1000000

typedef struct {
  Model* models[MAXMODELS];
  int firstItem;
} Modellist;

void renderAll(Modellist* modelList, ScePspFVector3 eye);
void addModel(Model* newModel, Modellist* modelList);
Modellist* initList();
#endif
