#include "../include/ground.h"
#include "../include/stlloader.h"
#include <stdint.h>
#include <pspgu.h>
#include <math.h>
// #define GRIDW 10
// #define GRIDH 10
#define GROUNDSIZE 200
#define VIEW_DISTANCE 1000.0f
void renderGround(int GRIDW, int GRIDH, STLModel* groundArray[GRIDW][GRIDH], char* filenames[GRIDW][GRIDH], uint8_t isLoaded[GRIDW][GRIDH], ScePspFVector3 eye){
        for(int i = 0; i < GRIDW; i++){
            for (int j = 0; j < GRIDH; j++){
                ScePspFVector3 platePos = {GROUNDSIZE * i, 0, GROUNDSIZE * j};
                ScePspFVector3 plateRot = {3*GU_PI/2, 0, 0};
                float distance = sqrtf(powf(platePos.x - eye.x, 2) + powf(platePos.y - eye.y, 2) + powf(platePos.z - eye.z, 2));
                if(isLoaded[i][j]){
                    renderModel(groundArray[i][j], platePos, plateRot);
                    if((distance > VIEW_DISTANCE)){
                        freeModel(groundArray[i][j]);
                        groundArray[i][j] = NULL;
                        isLoaded[i][j] = 0;
                    }
                } else {
                    if((distance < VIEW_DISTANCE) && !(isLoaded[i][j])){
                        STLModel* temp = loadModel(filenames[i][j]);
                        groundArray[i][j] = temp;
                        isLoaded[i][j] = 1;
                    }
                }
            }
        }
}
