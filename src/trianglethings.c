#include <math.h>
#include "trianglethings.h"
#include "stlloader.h"
#define MAX_TRIANGLES 65535

struct Vertex {
    float u, v;
    unsigned int color;
    float nx, ny, nz;
    float x, y, z;
};

struct Vertex __attribute__((aligned(16))) dynamicVertices[MAX_TRIANGLES * 3];

unsigned int normalToColor(float nx, float ny, float nz) {
    float len = sqrtf(nx * nx + ny * ny + nz * nz);
    if (len > 0.0f) {
        nx /= len;
        ny /= len;
        nz /= len;
    }

    unsigned char r = (unsigned char)((nx + 1.0f) * 0.5f * 255.0f);
    unsigned char g = (unsigned char)((ny + 1.0f) * 0.5f * 255.0f);
    unsigned char b = (unsigned char)((nz + 1.0f) * 0.5f * 255.0f);
    unsigned char a = 255;

    return (a << 24) | (b << 16) | (g << 8) | r;
}

void convertTrianglesToVertices(const Triangle* triangles, size_t numTriangles) {
    if (numTriangles > MAX_TRIANGLES) return;
    
    for (size_t i = 0; i < numTriangles; ++i) {
        const Triangle* tri = &triangles[i];
        float nx = tri->normal.x;
        float ny = tri->normal.y;
        float nz = tri->normal.z;
        unsigned int color = normalToColor(nx, ny, nz);

        dynamicVertices[i * 3 + 0] = (struct Vertex){0, 0, color, nx, ny, nz, tri->v1.x, tri->v1.y, tri->v1.z};
        dynamicVertices[i * 3 + 1] = (struct Vertex){0, 0, color, nx, ny, nz, tri->v2.x, tri->v2.y, tri->v2.z};
        dynamicVertices[i * 3 + 2] = (struct Vertex){0, 0, color, nx, ny, nz, tri->v3.x, tri->v3.y, tri->v3.z};
    }
}

struct Vertex* getDynamicVertexBuffer(void) {
    return dynamicVertices;
}
