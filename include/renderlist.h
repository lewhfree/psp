#ifndef RENDERLIST_H
#define RENDERLIST_H
#include "render.h"
#define MAXMODELS 100

typedef struct {
  Model models[MAXMODELS];
  int firstItem;
} Modellist;

void renderAll(Modellist* modelList);
void addModel(Model* newModel, Modellist* modelList);
Modellist* initList();
#endif
