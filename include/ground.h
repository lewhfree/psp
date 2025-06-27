#ifndef GROUND_H
#define GROUND_H
#include "stlloader.h"
#include <pspgu.h>
// #define GRIDW 10
// #define GRIDH 10
void renderGround(int GRIDW, int GRIDH, STLModel* groundArray[GRIDW][GRIDH],
                  char* filenames[GRIDW][GRIDH],
                  uint8_t isLoaded[GRIDW][GRIDH],
                  ScePspFVector3 eye
                  );
#endif
