#include "stlloader.h"
#include <pspiofilemgr.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspdebug.h>
int load_binary_stl(const char *filename, STLModel *outModel) {
  SceUID fileUID = sceIoOpen(filename, PSP_O_RDONLY, 0777);
  if(fileUID < 0){
    return 0;
  }
  //seek past header
  sceIoLseek(fileUID, 80, SEEK_SET);
  uint32_t count = 0;
  uint8_t buf[4];

  sceIoRead(fileUID, &buf, sizeof(uint8_t) * 4);
  count = (buf[0]) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
  Triangle* tris = malloc(sizeof(Triangle) * count);
  if(!tris){
    sceIoClose(fileUID);
    return 0;
  }

  for (uint32_t i = 0; i < count; ++i){
    sceIoRead(fileUID, &tris[i], sizeof(float) * 4 * 3);
    sceIoLseek(fileUID, 2, SEEK_CUR);
  }
  outModel->triangleCount = count;
  outModel->triangles = tris;
  sceIoClose(fileUID);

  return 1;
}
void free_stl(STLModel* model) {
  free(model->triangles);
  model->triangles = NULL;
  model->triangleCount = 0;
}
