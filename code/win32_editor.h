#ifndef WIN32_EDITOR_H
#define WIN32_EDITOR_H


#define GL_LOAD_FUNCTION(name, type) name = (type)wglGetProcAddress(#name);

#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_FLAGS_ARB             0x2094

typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc,
                                                       const int32_t *piAttribIList,
                                                       const FLOAT *pfAttribFList,
                                                       UINT nMaxFormats,
                                                       int32_t *piFormats,
                                                       UINT *nNumFormats);

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext,
                                                           const int32_t *attribList);

global_variable PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 0;
global_variable PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 0;

struct Win32Window {
    HWND handle;
    HDC deviceContext;
    HGLRC openglContext;
    u32 width;
    u32 height;
};

LRESULT CALLBACK EditorWindowProc(HWND window, UINT message, WPARAM wParam,
                                  LPARAM l_param);
void WIN32LoadWGLExtensions();
void WIN32CreateModernOpenglContext(HDC device_context, HGLRC *openglContext);
void* WIN32LoadFile(const char *file_path);
static void WIN32LoadGLFunctions();
void WINAPI MessageCallback(GLuint source, GLuint type, GLuint id,
                            GLuint severity, GLint length,
                            const GLchar* message, void* userParam);
void WIN32HandleWindowResize(Win32Window *window, GLuint *shaderProgamId,
                             u32 arrSize);
void WIN32Input(WPARAM wParam, GapBuffer *gb, Cursor *c, Win32Window *window);
uint8_t WIN32KeyPressStatus(u32 keyCode);


#endif //WIN32_EDITOR_H
