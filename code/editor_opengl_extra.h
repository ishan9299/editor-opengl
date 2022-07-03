#ifndef EDITOR_OPENGL_EXTRA_H
#define EDITOR_OPENGL_EXTRA_H

#include "cglm/cglm.h"

void loadUniformMatrix4fv(u32 shaderProgramId, f32 *matrix,
                          const char *uniformName)
{
    glUseProgram(shaderProgramId);
    i32 uniformLocation = glGetUniformLocation(shaderProgramId, uniformName);
    if (uniformLocation == GL_INVALID_VALUE)
    {
        Assert(!"Location Error");
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, matrix);
    glUseProgram(0);
}

void editorLoadUniformFloat(GLuint *shaderProgramIdArr, u32 arrSize, f32 value,
                            const char *uniformName)
{
    for (u32 i = 0; i < arrSize; i++)
    {
        glUseProgram(shaderProgramIdArr[i]);
        i32 uniformLocation = glGetUniformLocation(shaderProgramIdArr[i], uniformName);
        if (uniformLocation == GL_INVALID_VALUE)
        {
            Assert(!"Location Error");
        }
        glUniform1f(uniformLocation, value);
        glUseProgram(0);
    }
}

void editorLoadMVP(GLuint *shaderProgIdArr, u32 arrSize, u32 windowWidth,
                   u32 windowHeight)
{
    mat4 orthographic;
    f32 orthoLeft = 0.0f;
    f32 orthoRight = (f32)((f32)windowWidth);
    f32 orthoBottom = (f32)((f32)windowHeight);
    f32 orthoTop = 0.0f;
    f32 orthoNear = 1.0f;
    f32 orthoFar = -1.0f;
    glm_ortho(orthoLeft, orthoRight, orthoBottom,
              orthoTop, orthoNear, orthoFar, orthographic);
    
    for (u32 i = 0; i < arrSize; i++)
    {
        loadUniformMatrix4fv(shaderProgIdArr[i], (f32 *)orthographic,
                             "projection");
    }
}

GLuint editorCreateShaderProg(const char *vertShaderBuffer, const char *fragShaderBuffer)
{
    GLchar errorLog[1024];
    GLint status = 0;
    
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderId, 1, &vertShaderBuffer, 0);
    glCompileShader(vertShaderId);
    glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(vertShaderId, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    
    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderId, 1, &fragShaderBuffer, 0);
    glCompileShader(fragShaderId);
    glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(fragShaderId, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    
    GLuint shaderProgId_ = glCreateProgram();
    glAttachShader(shaderProgId_, vertShaderId);
    glAttachShader(shaderProgId_, fragShaderId);
    glLinkProgram(shaderProgId_);
    glGetProgramiv(shaderProgId_, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(shaderProgId_, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    return shaderProgId_;
}

#endif // EDITOR_OPENGL_EXTRA_H
