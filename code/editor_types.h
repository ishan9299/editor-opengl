#ifndef EDITOR_TYPES_H
#define EDITOR_TYPES_H

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

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

#if EDITOR_OPENGL_DEBUG
#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define BYTES(x) x
#define KILO_BYTES(x) BYTES(x)*1024
#define MEGA_BYTES(x) KILO_BYTES(x)*1024

#define ARR_SIZE(array) sizeof(array)/sizeof(array[0])

#define global_variable static
#define local_persist   static

#define OPENGL_MAJOR_VER 3
#define OPENGL_MINOR_VER 3

#define BATCH_VERTICES_SIZE 1000
#define BATCH_INDICES_SIZE 1500

#endif // EDITOR_TYPES_H
