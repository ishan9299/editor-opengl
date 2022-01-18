@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

cl /Z7 ..\code\win32_editor.cpp kernel32.lib opengl32.lib gdi32.lib user32.lib

popd