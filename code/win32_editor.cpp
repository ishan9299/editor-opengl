#include <stdint.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <gl/gl.h>
#include <map>

#include "editor_opengl.h"
#include "cglm/cglm.h"
#include "editor.cpp"
#include "win32_editor.h"

int32_t
WinMain(HINSTANCE instance, HINSTANCE prev_instance,
        LPSTR cmdLine, int32_t showCmd)
{
    
    WNDCLASSEXA windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = EditorWindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorA(instance, IDC_IBEAM);
    windowClass.lpszClassName = "editorwindowclass";
    
    if(!RegisterClassEx(&windowClass))
    {
        return -1;
    }
    WIN32LoadWGLExtensions();
    HWND editorWindow = CreateWindowExA(0, windowClass.lpszClassName, "editor",
                                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                        0, 0, instance, 0);
    if (!editorWindow)
    {
        return -1;
    }
    
    HDC deviceContext = GetDC(editorWindow);
    HGLRC openglContext;
    WIN32CreateModernOpenglContext(deviceContext, &openglContext);
    
    if (!openglContext)
    {
        return -1;
    }
    wglMakeCurrent(deviceContext, openglContext);
    WIN32LoadGLFunctions();
    
#if EDITOR_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(MessageCallback, 0);
#endif
    
    unsigned char *fontBuffer = (unsigned char *)WIN32LoadFile("assets\\JuliaMono-Regular.ttf");
    unsigned char *fontatlasBuffer = (unsigned char *)VirtualAlloc(0, fontatlasWidth * fontatlasWidth, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    eglGenerateFontAtlas(fontBuffer, fontatlasBuffer);
    
    unsigned char *demoFileBuffer = (unsigned char*)WIN32LoadFile("assets\\demo.txt");
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &fontatlasTextureId);
        glBindTexture(GL_TEXTURE_2D, fontatlasTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontatlasWidth, fontatlasWidth, 0, GL_RED,
                     GL_UNSIGNED_BYTE, fontatlasBuffer);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fontatlasTextureId);
    }
    char *rectVertBuffer = (char *)WIN32LoadFile("..\\code\\rect_vert.glsl");
    char *rectFragBuffer = (char *)WIN32LoadFile("..\\code\\rect_frag.glsl");
    rectShaderProgId = eglCreateShaderProg(rectVertBuffer, rectFragBuffer);
    VirtualFree(rectVertBuffer, 0, MEM_RELEASE);
    VirtualFree(rectFragBuffer, 0, MEM_RELEASE);
    
    RECT rect;
    GetClientRect(editorWindow, &rect);
    uint32_t width = rect.right - rect.left;
    uint32_t height = rect.bottom - rect.top;
    eglLoadMVP(width, height);
    eglCreateVertexBuffer();
    
    ShowWindow(editorWindow, SW_SHOW);
    
    while(running)
    {
        MSG message;
        while(PeekMessage(&message, editorWindow, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
            
            Vec2f topLeft = { 0 };
            eglDrawText(&topLeft, "hello World");
            SwapBuffers(deviceContext);
        }
    }
    
    VirtualFree(demoFileBuffer, 0, MEM_RELEASE);
    VirtualFree(fontBuffer, 0, MEM_RELEASE);
    VirtualFree(fontatlasBuffer, 0, MEM_RELEASE);
    
    wglMakeCurrent(0, 0);
    wglDeleteContext(openglContext);
    ReleaseDC(editorWindow, deviceContext);
    
    return 0;
}

LRESULT CALLBACK EditorWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT messageResult = 0;
    
    switch(message)
    {
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
        {
            running = 0;
        }
        break;
        
        case WM_SIZE:
        {
            if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) {
                RECT bounds;
                GetClientRect(window, &bounds);
                UINT windowWidth = bounds.right - bounds.left;
                UINT windowHeight = bounds.bottom - bounds.top;
                glViewport(0, 0, windowWidth, windowHeight);
                eglLoadMVP(windowWidth, windowHeight);
            }
        }
        break;
        
        case WM_SIZING:
        {
            if (GlobalOpenglInit)
            {
                RECT bounds;
                GetClientRect(window, &bounds);
                UINT window_width = bounds.right - bounds.left;
                UINT window_height = bounds.bottom - bounds.top;
                glViewport(0, 0, window_width, window_height);
                eglLoadMVP(window_width, window_height);
            }
        }
        break;
        case WM_KEYDOWN:
        {
            Input(wParam);
        }
        break;
        case WM_MOUSEWHEEL:
        {
            OutputDebugStringA("Wheel yo!!");
        }
        break;
        default:
        {
            messageResult = DefWindowProc(window, message, wParam, lParam);
        }
    }
    return messageResult;
}

void Input(WPARAM wParam)
{
    switch(wParam)
    {
        case VK_LBUTTON:
        {
        }
        break;
        
        case VK_RBUTTON:
        {
        }
        break;
        
        case VK_BACK:
        {
        }
        break;
        
        default:
        {
        }
        break;
    }
}

void WIN32LoadWGLExtensions()
{
    WNDCLASSEXA dummyWindowClass = {};
    dummyWindowClass.cbSize = sizeof(WNDCLASSEXA);
    dummyWindowClass.style = CS_OWNDC;
    dummyWindowClass.lpfnWndProc = DefWindowProc;
    dummyWindowClass.lpszClassName = "fakeWindow_class";
    
    RegisterClassEx(&dummyWindowClass);
    
    HWND fakeWindow =
        CreateWindowExA(0, dummyWindowClass.lpszClassName, "fakeWindow",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);
    
    HDC deviceContext = GetDC(fakeWindow);
    PIXELFORMATDESCRIPTOR pixel_format = {};
    int32_t pixelFormatIndex =
        ChoosePixelFormat(deviceContext, &pixel_format);
    if (pixelFormatIndex != 0)
    {
        if(SetPixelFormat(deviceContext, pixelFormatIndex,
                          &pixel_format))
        {
            HGLRC openglContext = wglCreateContext(deviceContext);
            wglMakeCurrent(deviceContext, openglContext);
            
            GL_LOAD_FUNCTION(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC);
            GL_LOAD_FUNCTION(wglCreateContextAttribsARB,
                             PFNWGLCREATECONTEXTATTRIBSARBPROC);
            
            wglMakeCurrent(0, 0);
            wglDeleteContext(openglContext);
        }
        else
        {
            // TODO(not-set): SetPixelFormat failed!
        }
    }
    else
    {
        // TODO(not-set): couldn't get a desired pixel format
    }
    ReleaseDC(fakeWindow, deviceContext);
    DestroyWindow(fakeWindow);
}

void WIN32CreateModernOpenglContext(HDC deviceContext, HGLRC *openglContext)
{
    const int32_t pixelAttribs[] =
    {
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
    
    int32_t pixelFormatIndex;
    UINT formatsReceived;
    bool status = wglChoosePixelFormatARB(deviceContext, pixelAttribs, 0, 1,
                                          &pixelFormatIndex,
                                          &formatsReceived);
    if(status && formatsReceived != 0)
    {
        PIXELFORMATDESCRIPTOR pixelFormat;
        DescribePixelFormat(deviceContext, pixelFormatIndex,
                            sizeof(pixelFormat), &pixelFormat);
        SetPixelFormat(deviceContext, pixelFormatIndex, &pixelFormat);
        
        int32_t  contextAttribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, OPENGL_MAJOR_VER,
            WGL_CONTEXT_MINOR_VERSION_ARB, OPENGL_MINOR_VER,
            WGL_CONTEXT_FLAGS_ARB,
#if EDITOR_OPENGL_DEBUG
            WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        
        *openglContext = wglCreateContextAttribsARB(deviceContext, 0,
                                                    contextAttribs);
    }
}

void* WIN32LoadFile(const char *filePath)
{
    LARGE_INTEGER fileSize;
    HANDLE file = CreateFileA(filePath, GENERIC_READ, 0, 0, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, 0);
    void *buffer = 0;
    if (file)
    {
        if (GetFileSizeEx(file, &fileSize))
        {
            buffer = VirtualAlloc(0, fileSize.QuadPart, MEM_COMMIT | MEM_RESERVE,
                                  PAGE_READWRITE);
            if (buffer)
            {
                DWORD bytesRead;
                DWORD bufferSize = (DWORD)fileSize.QuadPart;
                ReadFile(file, buffer, bufferSize, &bytesRead, 0);
            }
        }
        return buffer;
    }
    else
    {
        return 0;
    }
}

static void WIN32LoadGLFunctions()
{
    GL_LOAD_FUNCTION(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC);
    GL_LOAD_FUNCTION(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
    GL_LOAD_FUNCTION(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
    GL_LOAD_FUNCTION(glGenBuffers, PFNGLGENBUFFERSPROC);
    GL_LOAD_FUNCTION(glBufferData, PFNGLBUFFERDATAPROC);
    GL_LOAD_FUNCTION(glBufferSubData, PFNGLBUFFERSUBDATAPROC);
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
    GL_LOAD_FUNCTION(glUniform4fv, PFNGLUNIFORM4FVPROC);
    GlobalOpenglInit = 1;
}

void WINAPI
MessageCallback(GLuint source, GLuint type, GLuint id, GLuint severity,
                GLint length, const GLchar* message,
                void* userParam)
{
    switch(source)
    {
        case GL_DEBUG_SOURCE_API_ARB:
        {
            OutputDebugStringA("Source: API ");
        }
        break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
        {
            OutputDebugStringA("Source: Window System ");
        }
        break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
        {
            OutputDebugStringA("Source: Shader Compiler ");
        }
        break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
        {
            OutputDebugStringA("Source: Third Party ");
        }
        break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:
        {
            OutputDebugStringA("Source: Application ");
        }
        break;
        case GL_DEBUG_SOURCE_OTHER_ARB:
        {
            OutputDebugStringA("Source: Other ");
        }
        break;
        default:
        {
        }
        break;
    }
    
    switch(type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:
        {
            OutputDebugStringA("Type: Error ");
        }
        break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        {
            OutputDebugStringA("Type: Deprecated Behaviour ");
        }
        break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
        {
            OutputDebugStringA("Type: Undefined Behaviour ");
        }
        break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
        {
            OutputDebugStringA("Type: Portability ");
        }
        break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        {
            OutputDebugStringA("Type: Performance ");
        }
        break;
        case GL_DEBUG_TYPE_OTHER_ARB:
        {
            OutputDebugStringA("Type: Other ");
        }
        break;
        default:
        {
        }
        break;
    }
    
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH_ARB:
        {
            OutputDebugStringA("Severity: high ");
            OutputDebugStringA(message);
            Assert(!"Error");
        }
        break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        {
            OutputDebugStringA("Severity: medium ");
            OutputDebugStringA(message);
            Assert(!"Error");
        }
        break;
        case GL_DEBUG_SEVERITY_LOW_ARB:
        {
            OutputDebugStringA("Severity: low ");
            OutputDebugStringA(message);
            Assert(!"Error");
        }
        break;
        default:
        {
        }
        break;
    }
    OutputDebugStringA("\n");
}