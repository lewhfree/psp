#ifndef TILES_H
#define TILES_H

#include <stdint.h>
#include "stlloader.h"
#include "render_model.h"
typedef struct {
  char name[20];
  uint8_t x;
  uint8_t y;
  STLModel model;
} groundObject;



#endif
