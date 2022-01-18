#include <stdint.h>
#include <windows.h>
#include <gl/gl.h>
#include "editor_opengl.h"

#define global_variable static
#define local_persist   static

global_variable int32_t running = 1;

LRESULT CALLBACK
EditorWindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT message_result;
    switch(message) {
        case WM_QUIT:
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

int32_t
WinMain(HINSTANCE instance,
        HINSTANCE prev_instance,
        LPSTR cmd_line, int32_t show_cmd) {
    WNDCLASSEXA window_class = {};
    window_class.cbSize = sizeof(WNDCLASSEXA);
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = EditorWindowProc;
    // TODO(not-set): look into it if I want to implement a dialog box
    // window_class.cbWndExtra = ;
    window_class.hInstance = instance;
    // TODO(not-set): set a icon if you want
    //window_class.hIcon = ;
    window_class.hCursor = LoadCursorA(instance, IDC_IBEAM);
    // TODO(not-set): set a brush to paint the background look into it.
    //window_class.hbrBackground = ;
    window_class.lpszClassName = "editorwindowclass";
    
    if(RegisterClassEx(&window_class)) {
        HWND fake_window = CreateWindowExA(0, window_class.lpszClassName, "fake_window",
                                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                           CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, 0);
        if(fake_window) {
            HDC device_context = GetDC(fake_window);
            PIXELFORMATDESCRIPTOR pixel_format_desired = {};
            PIXELFORMATDESCRIPTOR pixel_format_suggested = {};
            pixel_format_desired.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pixel_format_desired.nVersion = 1;
            pixel_format_desired.dwFlags =
                PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
            
            pixel_format_desired.iPixelType = PFD_TYPE_RGBA;
            pixel_format_desired.cColorBits = 32;
            pixel_format_desired.cAlphaBits = 8;
            pixel_format_desired.cDepthBits = 24;
            int32_t pixel_format_index = ChoosePixelFormat(device_context, &pixel_format_desired);
            if (pixel_format_index != 0) {
                if(SetPixelFormat(device_context, pixel_format_index,
                                  &pixel_format_suggested)) {
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
        
        HWND editor_window = CreateWindowExA(0, window_class.lpszClassName, "fake_window",
                                             WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT,
                                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                             0, 0, instance, 0);
        if (editor_window) {
            HDC device_context = GetDC(editor_window);
            
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
            
            int32_t pixelFormatID;
            UINT numFormats;
            bool status = wglChoosePixelFormatARB(device_context, pixelAttribs, 0, 1, 
                                                  &pixelFormatID,
                                                  &numFormats);
            if(status && numFormats != 0) {
                
                PIXELFORMATDESCRIPTOR PFD;
                DescribePixelFormat(device_context, pixelFormatID, sizeof(PFD), &PFD);
                SetPixelFormat(device_context, pixelFormatID, &PFD);
                
                const int32_t major_min = 4, minor_min = 5;
                int32_t  contextAttribs[] = {
                    WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
                    WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
                    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                    0
                };
                
                HGLRC opengl_context = wglCreateContextAttribsARB(device_context, 0,
                                                                  contextAttribs);
                
                if (opengl_context) {
                    wglMakeCurrent(device_context, opengl_context);
                    while(running) {
                        MSG message;
                        while(PeekMessage(&message, editor_window, 0, 0, PM_REMOVE)) {
                            if(message.message == WM_QUIT) {
                                running = 0;
                            } else {
                                TranslateMessage(&message);
                                DispatchMessage(&message);
                            }
                        }
                        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT);
                        SwapBuffers(device_context);
                    }
                }
            }
        }
    }
}