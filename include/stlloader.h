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

// Add vertex structure here since it's part of the model now
struct Vertex {
    float u, v;
    unsigned int color;
    float nx, ny, nz;
    float x, y, z;
};

typedef struct {
  uint32_t triangleCount;
  Triangle* triangles;
  struct Vertex* vertices;  // Each model has its own vertices
} STLModel;

int load_binary_stl(const char* filename, STLModel* outModel);
void free_stl(STLModel* model);
void convert_model_triangles_to_vertices(STLModel* model); // New function
#endif
