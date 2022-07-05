#include <windows.h>
#include <gl/gl.h>

#define GAP_BUFFER_IMPLEMENTATION
#include "gap_buffer.h"

#include "editor_types.h"
#include "editor_opengl.h"
#include "editor_opengl_extra.h"
#include "editor_opengl_batch.h"
#include "editor_opengl_text.h"
#include "editor_opengl_cursor.h"
#include "win32_editor.h"

int32_t
WinMain(HINSTANCE instance, HINSTANCE prev_instance,
        LPSTR cmdLine, int32_t showCmd)
{

    Win32Window window;
    
    WNDCLASSEXA windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXA);
    windowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    windowClass.lpfnWndProc = EditorWindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorA(instance, IDC_IBEAM);
    windowClass.lpszClassName = "editorwindowclass";
    
    if(!RegisterClassEx(&windowClass))
    {
        return -1;
    }
    WIN32LoadWGLExtensions();
    window.handle = CreateWindowExA(0, windowClass.lpszClassName, "editor",
                                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        CW_USEDEFAULT, 0, 0, instance, 0);

    if (!window.handle)
    {
        return -1;
    }
    
    window.deviceContext = GetDC(window.handle);
    WIN32CreateModernOpenglContext(window.deviceContext, &window.openglContext);
    
    if (!window.openglContext)
    {
        return -1;
    }
    wglMakeCurrent(window.deviceContext, window.openglContext);
    WIN32LoadGLFunctions();
    
#if EDITOR_OPENGL_DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(MessageCallback, 0);
#endif

    char *demoFileBuffer = (char*)WIN32LoadFile("assets\\demo.txt");
    Vertex batchVertices[BATCH_VERTICES_SIZE];
    u32 batchIndices[BATCH_INDICES_SIZE];
    Batch fontBatch;
    FontAtlas fontAtlas;

    unsigned char *fontBuffer =
        (unsigned char *)WIN32LoadFile("assets\\JuliaMono-Regular.ttf");

    unsigned char *fontAtlasBuffer =
        (unsigned char *)VirtualAlloc(0, fontAtlas.width*fontAtlas.width,
                                      MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

    editorFontInit(fontBuffer, fontAtlasBuffer, &fontAtlas, &fontBatch, batchIndices);

    Cursor c;
    i32 cursorHeight = fontAtlas.f.ascent - fontAtlas.f.descent;
    i32 cursorWidth = 5;
    editorCursorInit(&c, cursorWidth, cursorHeight);

    GapBuffer gb;
    gapBufferInit(&gb, GAP_BUFFER_MIN_SIZE);

    GLuint shaderProgIds[] = {
        fontBatch.shaderProgId,
        c.shaderProgId
    };

    RECT rect;
    GetClientRect(window.handle, &rect);
    window.width = rect.right - rect.left;
    window.height = rect.bottom - rect.top;
    editorLoadMVP(shaderProgIds, ARR_SIZE(shaderProgIds),
                  window.width, window.height);
    editorLoadUniformFloat(shaderProgIds, ARR_SIZE(shaderProgIds), 1.0f, "scale");

    ShowWindow(window.handle, SW_SHOW);

    bool running = true;
    
    while(running)
    {
        MSG message;

        char gapBufferCopyString[BATCH_VERTICES_SIZE/4] = {0};
        while(PeekMessage(&message, window.handle, 0, 0, PM_REMOVE))
        {
            switch(message.message)
            {
                case WM_DESTROY:
                case WM_QUIT:
                {
                    running = false;
                }
                break;
                case WM_SIZE:
                {
                    WPARAM wParam = message.wParam;
                    if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
                    {
                        WIN32HandleWindowResize(&window, shaderProgIds,
                                                ARR_SIZE(shaderProgIds));
                    }
                }
                break;
                case WM_CHAR:
                {
                    char inputChar = (char)message.wParam;
                    if (inputChar > 31 && inputChar < 128)
                    {
                        gapBufferInsertChar(&gb, inputChar);
                        editorCursorRight(&c, fontAtlas.g[inputChar].advance);
                        inputChar = 0;
                    }
                }
                break;
                case WM_KEYDOWN:
                {
                    WPARAM wParam = message.wParam;
                    char currentChar = gapBufferCurrentCharacter(&gb);
                    if (wParam == VK_RIGHT)
                    {
                        if(gapBufferShiftCursorRight(&gb))
                        {
                            editorCursorRight(&c,
                                              fontAtlas.g[currentChar].advance);
                        }
                    }
                    else if (wParam == VK_LEFT)
                    {
                        if(gapBufferShiftCursorLeft(&gb))
                        {
                            editorCursorLeft(&c,
                                             fontAtlas.g[currentChar].advance);
                        }
                    }
                    else if (wParam == VK_RETURN)
                    {
                        gapBufferInsertChar(&gb, '\n');
                        editorCursorDown(&c);
                        c.v1.x = 0.0f;
                        c.v2.x = (f32)c.width;
                    }
                    else if (wParam == VK_BACK)
                    {
                        if(gapBufferBackspaceChar(&gb))
                        {
                            editorCursorLeft(&c,
                                             fontAtlas.g[currentChar].advance);
                        }
                    }
                    else if (wParam == VK_DELETE)
                    {
                        gapBufferDeleteChar(&gb);
                    }
                }
                break;
                case WM_SIZING:
                {
                    WIN32HandleWindowResize(&window, shaderProgIds,
                                            ARR_SIZE(shaderProgIds));
                }
                break;
                case WM_MOUSEWHEEL:
                {
                }
                break;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }


        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        editorDrawCursor(&c);
        gapBufferGetString(&gb, gapBufferCopyString);
        editorDrawBuffer(0, 0, gapBufferCopyString, &fontAtlas,
                         &fontBatch, batchVertices);
        flushGpuBuffer(&fontBatch, batchVertices);

        SwapBuffers(window.deviceContext);
    }
    
    VirtualFree(demoFileBuffer, 0, MEM_RELEASE);
    VirtualFree(fontBuffer, 0, MEM_RELEASE);
    VirtualFree(fontAtlasBuffer, 0, MEM_RELEASE);
    
    wglMakeCurrent(0, 0);
    wglDeleteContext(window.openglContext);
    ReleaseDC(window.handle, window.deviceContext);
    
    return 0;
}

void WIN32HandleWindowResize(Win32Window *window, GLuint *shaderProgIds, u32 arrSize)
{
    RECT bounds;
    GetClientRect(window->handle, &bounds);
    window->width = bounds.right - bounds.left;
    window->height = bounds.bottom - bounds.top;

    glViewport(0, 0, window->width, window->height);

    editorLoadMVP(shaderProgIds, arrSize, window->width, window->height);
    editorLoadUniformFloat(shaderProgIds, arrSize, 1.0f, "scale");
}

LRESULT CALLBACK EditorWindowProc(HWND window, UINT message, WPARAM wParam,
                                  LPARAM lParam)
{
    LRESULT messageResult = 0;
    
    messageResult = DefWindowProc(window, message, wParam, lParam);
    return messageResult;
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
            buffer = VirtualAlloc(0, fileSize.QuadPart, MEM_COMMIT|MEM_RESERVE,
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
    GL_LOAD_FUNCTION(glUniform1f, PFNGLUNIFORM1FPROC);
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
