#ifndef INCLUDES_H
#define INCLUDES_H

#include <cstdint>
#include <cmath>
#include <cstddef>
#include <cstdlib>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#if EDITOR_OPENGL_DEBUG
#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif
#define global_variable static
#define local_persist   static

#define OPENGL_MAJOR_VER 3
#define OPENGL_MINOR_VER 3

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

#define BATCH_VERTICES_SIZE 1000
#define BATCH_INDICES_SIZE 1500

#endif //INCLUDES_H
