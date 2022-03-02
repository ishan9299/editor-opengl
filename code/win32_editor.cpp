#include <stdint.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <gl/gl.h>

#if EDITOR_USE_CGLM
#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#define global_variable static
#define local_persist   static
#include "editor_opengl.h"
#include "cglm/cglm.h"
#else
#include "editor_math.cpp"
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define GL_LOAD_FUNCTION(name, type) name = (type)wglGetProcAddress(#name);

global_variable int32_t running = 1;
global_variable int32_t GlobalOpenglInit = 0;

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

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC,
                                                           HGLRC hShareContext,
                                                           const int32_t *attribList);


// wgl
global_variable PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 0;
global_variable PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 0;

LRESULT CALLBACK
EditorWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT message_result = 0;
    switch(message)
    {
    case WM_CLOSE:
    case WM_DESTROY:
    {
        running = 0;
    }
    break;
    case WM_SIZING:
    {
        if (GlobalOpenglInit) {
            RECT bounds;
            GetWindowRect(window, &bounds);
            UINT width = bounds.right - bounds.left;
            UINT height = bounds.bottom - bounds.top;
            glViewport(0, 0, width, height);
        }
    }
    break;
    default:
    {
        message_result = DefWindowProc(window, message, w_param, l_param);
    }
    break;
    }
    return message_result;
}

void
LoadWGLExtensions()
{
    WNDCLASSEXA dummy_window_class = {};
    dummy_window_class.cbSize = sizeof(WNDCLASSEXA);
    dummy_window_class.style = CS_OWNDC;
    dummy_window_class.lpfnWndProc = DefWindowProc;
    dummy_window_class.lpszClassName = "fake_window_class";
    
    RegisterClassEx(&dummy_window_class);
    
    HWND fake_window =
        CreateWindowExA(0, dummy_window_class.lpszClassName, "fake_window",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);
    HDC device_context = GetDC(fake_window);
    PIXELFORMATDESCRIPTOR pixel_format = {};
    int32_t pixel_format_index =
        ChoosePixelFormat(device_context, &pixel_format);
    if (pixel_format_index != 0) {
        if(SetPixelFormat(device_context, pixel_format_index,
                          &pixel_format)) {
            HGLRC opengl_context = wglCreateContext(device_context);
            wglMakeCurrent(device_context, opengl_context);
            
            GL_LOAD_FUNCTION(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC);
            GL_LOAD_FUNCTION(wglCreateContextAttribsARB,
                             PFNWGLCREATECONTEXTATTRIBSARBPROC);
            
            wglMakeCurrent(0, 0);
            wglDeleteContext(opengl_context);
        }
        else {
            // TODO(not-set): SetPixelFormat failed!
        }
    } else {
        // TODO(not-set): couldn't get a desired pixel format
    }
    ReleaseDC(fake_window, device_context);
    DestroyWindow(fake_window);
}

void
WIN32CreateModernOpenglContext(HDC device_context, HGLRC *opengl_context)
{
    const int32_t pixelAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };
    
    int32_t pixel_format_index;
    UINT formats_received;
    bool status = wglChoosePixelFormatARB(device_context, pixelAttribs, 0, 1,
                                          &pixel_format_index,
                                          &formats_received);
    if(status && formats_received != 0) {
        
        PIXELFORMATDESCRIPTOR pixel_format;
        DescribePixelFormat(device_context, pixel_format_index,
                            sizeof(pixel_format), &pixel_format);
        SetPixelFormat(device_context, pixel_format_index, &pixel_format);
        
        const int32_t major_min = 3, minor_min = 3;
        int32_t  context_attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
            WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
            WGL_CONTEXT_FLAGS_ARB,
#if EDITOR_OPENGL_DEBUG
            WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        
        *opengl_context = wglCreateContextAttribsARB(device_context, 0,
                                                     context_attribs);
    }
}

void *
WIN32LoadFile(const char *file_path, LARGE_INTEGER *file_size)
{
    HANDLE file = CreateFileA(file_path, GENERIC_READ, 0, 0, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, 0);
    void *buffer = 0;
    if (file) {
        if (GetFileSizeEx(file, file_size)) {
            buffer = VirtualAlloc(0, file_size->QuadPart, MEM_COMMIT | MEM_RESERVE,
                                  PAGE_READWRITE);
            if (buffer) {
                DWORD bytes_read;
                DWORD buffer_size = (DWORD)file_size->QuadPart;
                ReadFile(file, buffer, buffer_size, &bytes_read, 0);
            }
        }
        return buffer;
    }
    else {
        return 0;
    }
}

void WINAPI
MessageCallback(GLuint source, GLuint type, GLuint id, GLuint severity,
                GLint length, const GLchar* message,
                void* userParam)
{
    OutputDebugStringA(message);
    Assert(!"Opengl Error");
#if 0
    switch(type) {
    case GL_DEBUG_TYPE_ERROR_ARB:
        {
            Assert(!"Opengl Error");
        }
        break;
    }
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
        {
            Assert(!"Opengl Error");
        }
        break;
    }
#endif
}

void
LoadGLFunctions()
{
    GL_LOAD_FUNCTION(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC);
    GL_LOAD_FUNCTION(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
    GL_LOAD_FUNCTION(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
    GL_LOAD_FUNCTION(glGenBuffers, PFNGLGENBUFFERSPROC);
    GL_LOAD_FUNCTION(glBufferData, PFNGLBUFFERDATAPROC);
    GL_LOAD_FUNCTION(glBindBuffer, PFNGLBINDBUFFERPROC);
    GL_LOAD_FUNCTION(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
    GL_LOAD_FUNCTION(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
    GL_LOAD_FUNCTION(glCreateShader, PFNGLCREATESHADERPROC);
    GL_LOAD_FUNCTION(glShaderSource, PFNGLSHADERSOURCEPROC);
    GL_LOAD_FUNCTION(glCompileShader, PFNGLCOMPILESHADERPROC);
    GL_LOAD_FUNCTION(glCreateProgram, PFNGLCREATEPROGRAMPROC);
    GL_LOAD_FUNCTION(glAttachShader, PFNGLATTACHSHADERPROC);
    GL_LOAD_FUNCTION(glUniform1i, PFNGLUNIFORM1IPROC);
    GL_LOAD_FUNCTION(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
    GL_LOAD_FUNCTION(glUseProgram, PFNGLUSEPROGRAMPROC);
    GL_LOAD_FUNCTION(glLinkProgram, PFNGLLINKPROGRAMPROC);
    GL_LOAD_FUNCTION(glGetShaderiv, PFNGLGETSHADERIVPROC);
    GL_LOAD_FUNCTION(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
    GL_LOAD_FUNCTION(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
    GL_LOAD_FUNCTION(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
    GL_LOAD_FUNCTION(glActiveTexture, PFNGLACTIVETEXTUREPROC);
    GL_LOAD_FUNCTION(glDebugMessageControlARB, PFNGLDEBUGMESSAGECONTROLARBPROC);
    GL_LOAD_FUNCTION(glDebugMessageInsertARB, PFNGLDEBUGMESSAGEINSERTARBPROC);
#if EDITOR_OPENGL_DEBUG
    GL_LOAD_FUNCTION(glDebugMessageCallbackARB, PFNGLDEBUGMESSAGECALLBACKARBPROC);
    GL_LOAD_FUNCTION(glGetDebugMessageLogARB, PFNGLGETDEBUGMESSAGELOGARBPROC);
#endif
    GL_LOAD_FUNCTION(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
    GlobalOpenglInit = 1;
}

int32_t
WinMain(HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR cmd_line, int32_t show_cmd) {
    
    WNDCLASSEXA window_class = {};
    window_class.cbSize = sizeof(WNDCLASSEXA);
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = EditorWindowProc;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursorA(instance, IDC_IBEAM);
    window_class.lpszClassName = "editorwindowclass";

    stbtt_fontinfo font;
    int32_t font_offset = 0;
    int32_t bitmap_width, bitmap_height;
    LARGE_INTEGER font_file_size;
    unsigned char *bitmap;
    unsigned char *font_file =
        (unsigned char *)WIN32LoadFile("..\\data\\IBMPlexMono-Medium.ttf",
                                       &font_file_size);
    font_offset = stbtt_GetFontOffsetForIndex(font_file, 0);
    stbtt_InitFont(&font, font_file, font_offset);
    float scale = stbtt_ScaleForPixelHeight(&font, 35.0f);
    bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, 't',
                                      &bitmap_width, &bitmap_height,
                                      0, 0);
    
    char *demo_file_buffer = 0;
    LARGE_INTEGER demo_file_size;
    demo_file_buffer =
        (char *)WIN32LoadFile("..\\code\\stb_truetype.h", &demo_file_size);
    
    if(RegisterClassEx(&window_class)) {
        LoadWGLExtensions();
        HWND editor_window =
            CreateWindowExA(0, window_class.lpszClassName, "editor",
                            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            0, 0, instance, 0);
        if (editor_window) {
            HDC device_context = GetDC(editor_window);
            HGLRC opengl_context;
            WIN32CreateModernOpenglContext(device_context, &opengl_context);
            
            if (opengl_context) {
                wglMakeCurrent(device_context, opengl_context);
                
                LoadGLFunctions();
                
#if EDITOR_OPENGL_DEBUG
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
                glDebugMessageCallbackARB(MessageCallback, 0);
#endif
                //glEnable(GL_CULL_FACE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                GLchar error_log[1024];
                GLint status = 0;
                
                // setup shaders
                GLchar *vert_shader_buffer = 0;
                LARGE_INTEGER vert_shader_size;
                vert_shader_buffer = (GLchar *)WIN32LoadFile("..\\code\\vert.glsl",
                                                             &vert_shader_size);
                GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vert_shader_id, 1, &vert_shader_buffer, 0);
                glCompileShader(vert_shader_id);
                glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &status);
                if (!status) {
                    glGetShaderInfoLog(vert_shader_id, 1024, 0, error_log);
                    OutputDebugStringA(error_log);
                    Assert(status == 1);
                }
                VirtualFree(vert_shader_buffer, 0, MEM_RELEASE);
                
                GLchar *frag_shader_buffer = 0;
                LARGE_INTEGER frag_shader_size;
                frag_shader_buffer = (GLchar *)WIN32LoadFile("..\\code\\frag.glsl",
                                                             &frag_shader_size);
                GLuint frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(frag_shader_id, 1, &frag_shader_buffer, 0);
                glCompileShader(frag_shader_id);
                glGetShaderiv(frag_shader_id, GL_COMPILE_STATUS, &status);
                if (!status) {
                    glGetShaderInfoLog(frag_shader_id, 1024, 0, error_log);
                    OutputDebugStringA(error_log);
                    Assert(status == 1);
                }
                VirtualFree(frag_shader_buffer, 0, MEM_RELEASE);

                GLuint shader_prog_id = glCreateProgram();
                glAttachShader(shader_prog_id, vert_shader_id);
                glAttachShader(shader_prog_id, frag_shader_id);
                glLinkProgram(shader_prog_id);
                glGetProgramiv(shader_prog_id, GL_LINK_STATUS, &status);
                if (!status) {
                    glGetProgramInfoLog(shader_prog_id, 1024, 0, error_log);
                    OutputDebugStringA(error_log);
                    Assert(status == 1);
                }

#if !EDITOR_USE_CGLM 
                float Scale[4][4];
                ScaleMatrix4x4(Scale, 50.0f);

                quat Quaternion;
                float Rotation[4][4];
                float RotationAngle = 180.00f;
                vec3 Axis = {};
                Axis.x = 0.0f;
                Axis.y = 0.0f;
                Axis.z = 1.0f;
                MakeQuaternion(&Quaternion, &Axis, RotationAngle);
                MakeQuaternionToMatrix(&Quaternion, Rotation);

                float Translate[4][4];
                vec3 translate_v = {};
                translate_v.x = 20.0f;
                translate_v.y = 20.0f;
                translate_v.z = 0.0f;
                TranslateMatrix4x4(Translate, &translate_v);

                float Model[4][4];
                MultiplyMatrix4x4(Translate, Rotation, Model);
                MultiplyMatrix4x4(Model, Scale, Model);

                RECT rect;
                GetWindowRect(editor_window, &rect);
                float Orthographic[4][4];
                GLuint width = rect.right - rect.left;
                GLuint height = rect.bottom - rect.top;
                float ortho_right = (float)width;
                float ortho_left = 0.0f;
                float ortho_top = 0.0f;
                float ortho_bottom = (float)height;
                float ortho_far = -1.0f;
                float ortho_near = 1.0f;
                MakeOrthographicMatrix(ortho_right, ortho_left, ortho_top,
                                       ortho_bottom, ortho_far, ortho_near,
                                       Orthographic);

                glUseProgram(shader_prog_id);
                GLuint model_location = glGetUniformLocation(shader_prog_id, "model");
                if (model_location == GL_INVALID_VALUE) {
                    Assert(!"Location Error");
                }
                glUniformMatrix4fv(model_location, 1, GL_FALSE, (GLfloat *)Model);

                GLuint projection_location = glGetUniformLocation(shader_prog_id, "projection");
                if (model_location == GL_INVALID_VALUE) {
                    Assert(!"Location Error");
                }
                glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *)Orthographic);
#else
                mat4 Translate;
                vec3 translate_v = {20.0f, 20.0f, 0.0f};
                glm_mat4_identity(Translate);
                glm_translate(Translate, translate_v);
		
                mat4 Scale;
                glm_mat4_identity(Scale);
                float scaler = 20.0f;
                vec3 scale_v = {scaler, scaler, scaler};
                glm_scale(Scale, scale_v);

                versor v;
                mat4 Rotation;
                glm_quat(v, 3.14159265358f, 0.0f, 0.0f, 1.0f);
                glm_quat_mat4(v, Rotation);

                mat4 Model;
                mat4 *mul_m[] = {&Translate, &Rotation, &Scale};
                glm_mat4_mulN(mul_m, 3, Model);
	     
                RECT rect;
                mat4 Orthographic;
                GetWindowRect(editor_window, &rect);
                GLuint width = rect.right - rect.left;
                GLuint height = rect.bottom - rect.top;
                float ortho_right = (float)width;
                float ortho_left = 0.0f;
                float ortho_top = 0.0f;
                float ortho_bottom = (float)height;
                float ortho_far = -1.0f;
                float ortho_near = 1.0f;
                glm_ortho(ortho_left, ortho_right, ortho_bottom, ortho_top,
                          ortho_near, ortho_far, Orthographic);

                glUseProgram(shader_prog_id);
                GLuint model_location = glGetUniformLocation(shader_prog_id, "model");
                if (model_location == GL_INVALID_VALUE) {
                    Assert(!"Location Error");
                }
                glUniformMatrix4fv(model_location, 1, GL_FALSE, (GLfloat *)Model);

                GLuint projection_location = glGetUniformLocation(shader_prog_id, "projection");
		
                if (model_location == GL_INVALID_VALUE) {
                    Assert(!"Location Error");
                }
                glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *)Orthographic);
#endif
                
                // setup buffers
                GLfloat vertices[] = {
                    0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
                    0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
                    0.0f,  0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f,  0.0f, 0.0f, 0.0f, 1.0f 
                };
                GLuint indices[] = {
                    0, 1, 3,
                    1, 2, 3,
                };
                GLuint vertex_buffer_object, vertex_array_object;
                GLuint element_buffer_object;
                glGenVertexArrays(1, &vertex_array_object);
                glGenBuffers(1, &vertex_buffer_object);
                glGenBuffers(1, &element_buffer_object);
                glBindVertexArray(vertex_array_object);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                             GL_STATIC_DRAW);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                             GL_STATIC_DRAW);
                
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                      (void *)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                                      (void *)(3 * sizeof(GLfloat)));
                glEnableVertexAttribArray(1);
                glBindVertexArray(0);
                
                GLuint texture_id;
                glGenTextures(1, &texture_id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap_width, bitmap_height, 0,
                             GL_RED, GL_UNSIGNED_BYTE, bitmap);
                
                ShowWindow(editor_window, SW_SHOW);
                
                while(running) {
                    MSG message;
                    while(PeekMessage(&message, editor_window, 0, 0, PM_REMOVE)) {
                        if(message.message == WM_QUIT) {
                            running = 0;
                        }
                        TranslateMessage(&message);
                        DispatchMessage(&message);
                    }
                    
                    // TODO(not-set): always glClear first
                    glClear(GL_COLOR_BUFFER_BIT);
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    glUseProgram(shader_prog_id);
                    glBindVertexArray(vertex_array_object);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    
                    SwapBuffers(device_context);
                }
            }
            
            wglMakeCurrent(0, 0);
            wglDeleteContext(opengl_context);
            ReleaseDC(editor_window, device_context);
        }
    }
    
    VirtualFree(demo_file_buffer, 0, MEM_RELEASE);
}
