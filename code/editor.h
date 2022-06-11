#ifndef EDITOR_H
#define EDITOR_H

enum PieceTableFile {
    ORIGINAL_FILE,
    ADD_FILE,
};

struct Node {
    Node *prev;
    Node *next;
    
    PieceTableFile file;
    uint64_t start;
    uint64_t length;
};

struct List {
    Node sentinalHead;
    Node sentinalTail;
};

struct FontGlyphs {
    i32 width;
    i32 height;
    i32 xoffset;
    i32 yoffset;
    i32 leftSideBearing;
    i32 advance;
    
    u32 atlasXoffset;
    u32 atlasYoffset;
};

struct FontMetrics {
    i32 ascent;
    i32 lineGap;
    i32 descent;
};

struct Vec2f {
    f32 x;
    f32 y;
};

struct Vec3f {
    f32 x;
    f32 y;
    f32 z;
};

struct Vec4f {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct Vertex {
    Vec2f vertCoords;
    Vec2f texCoords;
    Vec4f color;
};

struct FontAtlas {
    FontGlyphs g[128];
    FontMetrics f;
    u32 width = 512;
};

struct Batch {
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 verticesFilled;
};

global_variable Vertex vertices[BATCH_VERTICES_SIZE];
global_variable u32 indices[BATCH_INDICES_SIZE];
global_variable u32 shaderProgId;

global_variable f32 fontSize = 64.0f;

global_variable u32 textureId;

global_variable Batch batch;

global_variable FontAtlas atlas;
global_variable f32 FontScale = 1.0f;

global_variable char originalFileBuffer[1024];
global_variable char addBuffer[1024];

#endif // EDITOR_H
