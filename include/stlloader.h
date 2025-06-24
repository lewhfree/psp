#ifndef STL_LOADER_H
#define STL_LOADER_H
#include <stdint.h>

typedef struct {
  float x, y, z;
} Vec3;

typedef struct {
  Vec3 normal;
  Vec3 v1, v2, v3;
} Triangle;

typedef struct {
  uint32_t triangleCount;
  Triangle* triangles;
} STLModel;

int load_binary_stl(const char* filename, STLModel* outModel);
void free_stl(STLModel* model);
#endif
