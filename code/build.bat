@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

cl /nologo /wd4100 /W4 /Z7 /DEDITOR_OPENGL_DEBUG=1 ..\code\win32_editor.cpp kernel32.lib opengl32.lib gdi32.lib user32.lib


popd
