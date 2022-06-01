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
    f32 leftSideBearing;
    f32 advance;
};

struct FontMetrics {
    f32 ascent;
    f32 lineGap;
    f32 descent;
};

global_variable u32 textShaderProgId;
global_variable u32 textVAO;
global_variable u32 textVBO;

global_variable u32 cursorShaderProgId;
global_variable u32 cursorVAO;
global_variable u32 cursorVBO;

#endif // EDITOR_H
