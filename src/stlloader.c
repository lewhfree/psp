#include "stlloader.h"
#include <pspiofilemgr.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspdebug.h>
#include <math.h>
#include <malloc.h>
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

int load_binary_stl(const char *filename, STLModel *outModel) {
    SceUID fileUID = sceIoOpen(filename, PSP_O_RDONLY, 0777);
    if(fileUID < 0){
        return 0;
    }
    
    outModel->triangles = NULL;
    outModel->vertices = NULL;
    outModel->triangleCount = 0;
    
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

    struct Vertex* vertices = memalign(16, sizeof(struct Vertex) * count * 3);
    if(!vertices){
        free(tris);
        sceIoClose(fileUID);
        return 0;
    }

    for (uint32_t i = 0; i < count; ++i){
        sceIoRead(fileUID, &tris[i], sizeof(float) * 4 * 3);
        sceIoLseek(fileUID, 2, SEEK_CUR);
    }
    
    outModel->triangleCount = count;
    outModel->triangles = tris;
    outModel->vertices = vertices;
    sceIoClose(fileUID);

    convert_model_triangles_to_vertices(outModel);
    
    return 1;
}

void convert_model_triangles_to_vertices(STLModel* model) {
    for (uint32_t i = 0; i < model->triangleCount; ++i) {
        const Triangle* tri = &model->triangles[i];
        float nx = tri->normal.x;
        float ny = tri->normal.y;
        float nz = tri->normal.z;
        unsigned int color = normalToColor(nx, ny, nz);

        model->vertices[i * 3 + 0] = (struct Vertex){0, 0, color, nx, ny, nz, tri->v1.x, tri->v1.y, tri->v1.z};
        model->vertices[i * 3 + 1] = (struct Vertex){0, 0, color, nx, ny, nz, tri->v2.x, tri->v2.y, tri->v2.z};
        model->vertices[i * 3 + 2] = (struct Vertex){0, 0, color, nx, ny, nz, tri->v3.x, tri->v3.y, tri->v3.z};
    }
}

void free_stl(STLModel* model) {
    if(model->triangles) {
        free(model->triangles);
        model->triangles = NULL;
    }
    if(model->vertices) {
        free(model->vertices);
        model->vertices = NULL;
    }
    model->triangleCount = 0;
}
