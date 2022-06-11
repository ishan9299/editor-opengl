/* date = January 18th 2022 0:10 pm */
#ifndef EDITOR_OPENGL_H
#define EDITOR_OPENGL_H

#ifndef APIENTRY
#define APIENTRY
#endif

#define GL_INVALID_VALUE                  0x0501
#define GL_BLEND                          0x0BE2
#define GL_UNSIGNED_INT                   0x1405
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_TRUE                           1
#define GL_FALSE                          0
#define GL_REPEAT                         0x2901
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_VERSION                        0x1F02
#define GL_VENDOR                         0x1F00
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_BGR                            0x80E0
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_UNPACK_ALIGNMENT               0x0CF5

#define GL_DEBUG_OUTPUT                               0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB               0x8242
#define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB               0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB              0x9144
#define GL_DEBUG_LOGGED_MESSAGES_ARB                  0x9145
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB       0x8243
#define GL_DEBUG_CALLBACK_FUNCTION_ARB                0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_ARB              0x8245
#define GL_DEBUG_SOURCE_API_ARB                       0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB             0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB           0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_ARB               0x8249
#define GL_DEBUG_SOURCE_APPLICATION_ARB               0x824A
#define GL_DEBUG_SOURCE_OTHER_ARB                     0x824B
#define GL_DEBUG_TYPE_ERROR_ARB                       0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB         0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB          0x824E
#define GL_DEBUG_TYPE_PORTABILITY_ARB                 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_ARB                 0x8250
#define GL_DEBUG_TYPE_OTHER_ARB                       0x8251
#define GL_DEBUG_SEVERITY_HIGH_ARB                    0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_ARB                  0x9147
#define GL_DEBUG_SEVERITY_LOW_ARB                     0x9148

typedef float GLfloat;
typedef uint32_t GLuint;
typedef uint32_t GLenum;
typedef int GLsizei;
typedef uint8_t GLubyte;
typedef char GLchar;
typedef signed long long int GLsizeiptr;
typedef intptr_t GLintptr;

typedef void (APIENTRY *PFNGLGENERATEMIPMAPPROC) (GLenum target);
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRY *PFNGLDEBUGMESSAGECONTROLARBPROC) (unsigned int source, unsigned int type, unsigned int severity, int count, const unsigned int* ids, bool enabled);
typedef void (APIENTRY *PFNGLDEBUGMESSAGEINSERTARBPROC) (unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* buf);
typedef void (APIENTRY *GLDEBUGPROCARB)(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam);
typedef void (APIENTRY *PFNGLDEBUGMESSAGECALLBACKARBPROC) (GLDEBUGPROCARB callback, void* userParam);
typedef GLuint (APIENTRY *PFNGLGETDEBUGMESSAGELOGARBPROC) (unsigned int count, int bufsize, unsigned int* sources,unsigned int* types, unsigned int* ids, unsigned int* severities, int* lengths, char* messageLog);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);


static PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLBUFFERSUBDATAPROC glBufferSubData;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLUNIFORM1IPROC glUniform1i;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
static PFNGLGETPROGRAMIVPROC glGetProgramiv;
static PFNGLACTIVETEXTUREPROC glActiveTexture;
static PFNGLDEBUGMESSAGECONTROLARBPROC glDebugMessageControlARB;
static PFNGLDEBUGMESSAGEINSERTARBPROC glDebugMessageInsertARB;
static PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB;
static PFNGLGETDEBUGMESSAGELOGARBPROC glGetDebugMessageLogARB;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
static PFNGLUNIFORM4FVPROC glUniform4fv;

#endif //EDITOR_OPENGL_H
