#ifndef STL_LOADER_H
#define STL_LOADER_H
#include <stdint.h>
#include <pspgu.h>

typedef struct {
  ScePspFVector3 normal;
  ScePspFVector3 v1, v2, v3;
} Triangle;

struct Vertex {
    float u, v;
    unsigned int color;
    float nx, ny, nz;
    float x, y, z;
};

typedef struct {
  uint32_t triangleCount;
  Triangle* triangles;
  struct Vertex* vertices;
  unsigned int isLoading;
} STLModel;

STLModel* loadSTL(char* filename);
void freeSTL(STLModel* model);
void renderSTL(STLModel* model, ScePspFVector3 position, ScePspFVector3 rotation);
#endif
