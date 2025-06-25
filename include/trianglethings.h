#ifndef trianglethings_h
#define trianglethings_h

#include <stddef.h> // for size_t
#include "stlloader.h"
typedef struct {
    float x, y, z;
} Vector;

unsigned int normalToColor(float nx, float ny, float nz);
void convertTrianglesToVertices(const Triangle* triangles, size_t numtriangles);
struct Vertex* getDynamicVertexBuffer(void);

#endif // trianglethings_h

