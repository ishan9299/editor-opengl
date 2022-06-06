#ifndef EDITOR_H
#define EDITOR_H

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#if EDITOR_OPENGL_DEBUG
#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif
#define global_variable static
#define local_persist   static
#define FILE_SIZE 3600

#define ASCII_LOW 32
#define ASCII_HIGH 126

#define OPENGL_MAJOR_VER 3
#define OPENGL_MINOR_VER 3

typedef int32_t i32;
typedef int64_t i64;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;

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
    
    u32 fontatlasOffsetX;
    u32 fontatlasOffsetY;
};

struct FontMetrics {
    i32 ascent;
    i32 lineGap;
    i32 descent;
};

struct Vertex {
    float vertCoords[2];
    float vertColor[4];
    
    float texCoords[2];
    float texColor[4];
};

struct Vec2f {
    float x;
    float y;
};

global_variable u32 rectShaderProgId;
global_variable u32 rectVAO;
global_variable u32 rectVBO;
global_variable u32 rectEBO;

global_variable GLuint fontatlasTextureId;
global_variable u32 fontatlasWidth = 512;
global_variable f32 fontSize = 64.0f;

global_variable FontGlyphs glyph[128];
global_variable FontMetrics fontMetrics;
global_variable f32 FontScale = 3.5f;

global_variable char originalFileBuffer[1024];
global_variable char addBuffer[1024];

#endif // EDITOR_H
