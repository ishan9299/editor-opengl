#ifndef EDITOR_H
#define EDITOR_H

#if EDITOR_OPENGL_DEBUG
#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif
#define global_variable static
#define local_persist   static

typedef int32_t i32;
typedef float f32;

struct character {
    GLuint tex_id;
    unsigned char *bitmap;
    i32 width;
    i32 height;
    i32 xoffset;
    i32 yoffset;
    
    f32 left_side_bearing;
    f32 advance;
    
    f32 ascent;
    f32 line_gap;
    f32 descent;
};

#endif // EDITOR_H
