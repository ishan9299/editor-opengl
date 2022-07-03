#ifndef EDITOR_OPENGL_BATCH_H
#define EDITOR_OPENGL_BATCH_H

struct Vertex {
    Vec2f vertCoords;
    Vec2f texCoords;
    Vec4f color;
};

struct Batch {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 texId;
    u32 shaderProgId;
    u32 verticesFilled;
};

void pushCharToGpu(Vec2f *v1, Vec2f *v2, Vec2f *texV1, Vec2f *texV2, Batch *batch,
                   Vertex *vertices);
void allocateBatchMemGpu(Batch *batch, u32 *indices);
void flushGpuBuffer(Batch *batch, Vertex *vertices);


void pushCharToGpu(Vec2f *v1, Vec2f *v2, Vec2f *texV1, Vec2f *texV2, Batch *batch,
                   Vertex *vertices)
{
    u32 offset = batch->verticesFilled;
    if (offset >= BATCH_VERTICES_SIZE)
    {
        flushGpuBuffer(batch, vertices);
        offset = batch->verticesFilled;
    }

    vertices[offset + 0].vertCoords.x = v2->x;
    vertices[offset + 0].vertCoords.y = v1->y;
    vertices[offset + 0].texCoords.x = texV2->x;
    vertices[offset + 0].texCoords.y = texV1->y;
    vertices[offset + 0].color.x = 1.0f;
    vertices[offset + 0].color.y = 1.0f;
    vertices[offset + 0].color.z = 1.0f;
    vertices[offset + 0].color.w = 1.0f;

    vertices[offset + 1].vertCoords.x = v2->x;
    vertices[offset + 1].vertCoords.y = v2->y;
    vertices[offset + 1].texCoords.x = texV2->x;
    vertices[offset + 1].texCoords.y = texV2->y;
    vertices[offset + 1].color.x = 1.0f;
    vertices[offset + 1].color.y = 1.0f;
    vertices[offset + 1].color.z = 1.0f;
    vertices[offset + 1].color.w = 1.0f;

    vertices[offset + 2].vertCoords.x = v1->x;
    vertices[offset + 2].vertCoords.y = v2->y;
    vertices[offset + 2].texCoords.x = texV1->x;
    vertices[offset + 2].texCoords.y = texV2->y;
    vertices[offset + 2].color.x = 1.0f;
    vertices[offset + 2].color.y = 1.0f;
    vertices[offset + 2].color.z = 1.0f;
    vertices[offset + 2].color.w = 1.0f;

    vertices[offset + 3].vertCoords.x = v1->x;
    vertices[offset + 3].vertCoords.y = v1->y;
    vertices[offset + 3].texCoords.x = texV1->x;
    vertices[offset + 3].texCoords.y = texV1->y;
    vertices[offset + 3].color.x = 1.0f;
    vertices[offset + 3].color.y = 1.0f;
    vertices[offset + 3].color.z = 1.0f;
    vertices[offset + 3].color.w = 1.0f;

    batch->verticesFilled += 4;
}

void allocateBatchMemGpu(Batch *batch, u32 *indices)
{
    u32 offset = 0;
    for (u32 i = 0; i < BATCH_INDICES_SIZE; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 3;
        indices[i + 3] = offset + 1;
        indices[i + 4] = offset + 2;
        indices[i + 5] = offset + 3;
        offset += 4;
    }

    u32 vao;
    u32 vbo;
    u32 ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*BATCH_VERTICES_SIZE, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32)*BATCH_INDICES_SIZE,
                 indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, vertCoords));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    batch->ebo = ebo;
    batch->vbo = vbo;
    batch->vao = vao;
    batch->verticesFilled = 0;
}

void flushGpuBuffer(Batch *batch, Vertex *vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*BATCH_VERTICES_SIZE, vertices);
    glUseProgram(batch->shaderProgId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch->texId);

    glBindVertexArray(batch->vao);
    glDrawElements(GL_TRIANGLES, BATCH_VERTICES_SIZE*6, GL_UNSIGNED_INT, 0);

    batch->verticesFilled = 0;
}

#endif // EDITOR_OPENGL_BATCH_H
