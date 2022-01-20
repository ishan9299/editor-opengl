#include <stdint.h>
#include <windows.h>
#include <gl/gl.h>
#include "editor_opengl.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// TODO(not-set): use freetype for now we will write our own custom ttf file parser.

#define global_variable static
#define local_persist   static

global_variable int32_t running = 1;

LRESULT CALLBACK
EditorWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT message_result;
    switch(message) {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            running = 0;
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
            // initialize functions here
            wglChoosePixelFormatARB =
            (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
            
            wglCreateContextAttribsARB =
            (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
            
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
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        
        *opengl_context = wglCreateContextAttribsARB(device_context, 0,
                                                     context_attribs);
    }
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
    
    HANDLE demo_file = CreateFileA("W:\\editor\\code\\stb_truetype.h", GENERIC_READ,
                                   0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    char *file_buffer = 0;
    LARGE_INTEGER file_size;
    if (GetFileSizeEx(demo_file, &file_size)) {
        file_buffer = (char *)VirtualAlloc(0, file_size.QuadPart,
                                           MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (file_buffer) {
            DWORD bytes_read;
            DWORD buffer_size = (DWORD)file_size.QuadPart;
            ReadFile(demo_file, file_buffer, buffer_size, &bytes_read, 0);
        }
    }
    
    if(RegisterClassEx(&window_class)) {
        LoadWGLExtensions();
        HWND editor_window =
            CreateWindowExA(0, window_class.lpszClassName, "editor",
                            WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            0, 0, instance, 0);
        if (editor_window) {
            HDC device_context = GetDC(editor_window);
            HGLRC opengl_context;
            WIN32CreateModernOpenglContext(device_context, &opengl_context);
            
            if (opengl_context) {
                wglMakeCurrent(device_context, opengl_context);
                while(running) {
                    MSG message;
                    while(PeekMessage(&message, editor_window, 0, 0, PM_REMOVE)) {
                        if(message.message == WM_QUIT) {
                            running = 0;
                        }
                        TranslateMessage(&message);
                        DispatchMessage(&message);
                    }
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    SwapBuffers(device_context);
                }
            }
            
            wglMakeCurrent(0, 0);
            wglDeleteContext(opengl_context);
            ReleaseDC(editor_window, device_context);
        }
    }
    
    VirtualFree(file_buffer, 0, MEM_RELEASE);
}