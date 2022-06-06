#include "editor.h"
#include <math.h>

static void eglLoadUniformMatrix4fv(u32 shaderProgramId, f32 *matrix,
                                    const char *uniformName)
{
    glUseProgram(shaderProgramId);
    i32 uniformLocation = glGetUniformLocation(shaderProgramId, uniformName);
    if (shaderProgramId == GL_INVALID_VALUE)
    {
        Assert(!"Location Error");
    }
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, matrix);
    glUseProgram(0);
}

static void eglLoadMVP(u32 windowWidth, u32 windowHeight)
{
    mat4 orthographic;
    f32 orthoLeft = 0.0f;
    f32 orthoRight = (f32)((f32)windowWidth*FontScale);
    f32 orthoBottom = (f32)((f32)windowHeight*FontScale);
    f32 orthoTop = 0.0f;
    f32 orthoNear = 1.0f;
    f32 orthoFar = -1.0f;
    glm_ortho(orthoLeft, orthoRight, orthoBottom,
              orthoTop, orthoNear, orthoFar, orthographic);
    
    eglLoadUniformMatrix4fv(rectShaderProgId, (f32 *)orthographic, "projection");
}

static void eglCreateVertexBuffer()
{
    u32 indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };
    
    glGenVertexArrays(1, &rectVAO);
    glBindVertexArray(rectVAO);
    
    glGenBuffers(1, &rectVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32)*5*4, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &rectEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32)*3*2, indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

static void eglGenerateFontAtlas(unsigned char *fontBuffer, unsigned char *fontatlasBuffer)
{
    stbtt_fontinfo font;
    i32 fontOffset = 0;
    
    fontOffset = stbtt_GetFontOffsetForIndex(fontBuffer, 0);
    stbtt_InitFont(&font, fontBuffer, fontOffset);
    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    
    i32 ascent;
    i32 descent;
    i32 lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    
    ascent = (int)floorf(scale * ascent);
    descent = (int)floorf(scale * descent);
    lineGap = (int)floorf(scale * lineGap);
    
    fontMetrics.ascent = ascent;
    fontMetrics.descent = descent;
    fontMetrics.lineGap = lineGap;
    
    u32 atlasXOffset = 0;
    u32 atlasYOffset = 0;
    
    for (u32 c = ' '; c < 128; c++)
    {
        unsigned char *bitmap;
        i32 bitmapWidth, bitmapHeight;
        i32 advanceWidth, leftSideBearing;
        i32 xoffset, yoffset;
        bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, c, &bitmapWidth,
                                          &bitmapHeight, &xoffset, &yoffset);
        
        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);
        
        advanceWidth = (int)floorf(advanceWidth*scale);
        leftSideBearing = (int)floorf(leftSideBearing*scale);
        
        if (atlasXOffset + bitmapWidth >= fontatlasWidth)
        {
            atlasXOffset = 0;
            atlasYOffset += ascent - descent;
        }
        
        for (i32 y = 0; y < bitmapHeight; y++)
        {
            for (i32 x = 0; x < bitmapWidth; x++)
            {
                fontatlasBuffer[((y + atlasYOffset)*fontatlasWidth) + x + atlasXOffset] = bitmap[(y*bitmapWidth) + x];
            }
        }
        
        glyph[c].width = bitmapWidth;
        glyph[c].height = bitmapHeight;
        glyph[c].xoffset = xoffset;
        glyph[c].yoffset = yoffset;
        glyph[c].leftSideBearing = leftSideBearing;
        glyph[c].advance = advanceWidth;
        glyph[c].fontatlasOffsetX = atlasXOffset;
        glyph[c].fontatlasOffsetY = atlasYOffset;
        
        atlasXOffset += bitmapWidth;
    }
}

GLuint eglCreateShaderProg(char *vertShaderBuffer, char *fragShaderBuffer)
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
    
    GLuint shaderProgId = glCreateProgram();
    glAttachShader(shaderProgId, vertShaderId);
    glAttachShader(shaderProgId, fragShaderId);
    glLinkProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(shaderProgId, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    return shaderProgId;
}

void eglDrawRect(Vec2f *topLeft, Vec2f *bottomRight, Vec2f *texTopLeft, Vec2f *texBottomRight)
{
    f32 x1 = topLeft->x;
    f32 y1 = topLeft->y;
    
    f32 x2 = bottomRight->x;
    f32 y2 = bottomRight->y;
    
    f32 tex_x1 = texTopLeft->x;
    f32 tex_y1 = texTopLeft->y;
    
    f32 tex_x2 = texBottomRight->x;
    f32 tex_y2 = texBottomRight->y;
    
    GLfloat vertices[] =
    {
        x2, y1, 0.0f, tex_x2, tex_y1,
        x2, y2, 0.0f, tex_x2, tex_y2,
        x1, y2, 0.0f, tex_x1, tex_y2,
        x1, y1, 0.0f, tex_x1, tex_y1,
    };
    
    glUseProgram(rectShaderProgId);
    
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glUseProgram(0);
}

void eglDrawText(Vec2f *topLeft, char *text)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    f32 lineGap = 0.0f;
    
    for (i32 i = 0; text[i] != 0; i++)
    {
        char ascii = text[i];
        
        if (ascii == '\n')
        {
            topLeft->x = 0;
            lineGap += (f32)(fontMetrics.ascent - fontMetrics.descent);
            ascii = text[++i];
        }
        
        if (ascii == '\r')
        {
            topLeft->x = 0;
            lineGap += (f32)(fontMetrics.ascent - fontMetrics.descent);
            i++;
            ascii = text[++i];
        }
        
        f32 baseline = (f32)(glyph[ascii].yoffset + fontMetrics.ascent);
        topLeft->y = baseline + lineGap;
        
        Vec2f bottomRight;
        bottomRight.x = (f32)(topLeft->x + glyph[ascii].width);
        bottomRight.y = (f32)(topLeft->y + glyph[ascii].height);
        
        Vec2f texTopLeft;
        texTopLeft.x = ((f32)glyph[ascii].fontatlasOffsetX/(f32)fontatlasWidth);
        texTopLeft.y = ((f32)glyph[ascii].fontatlasOffsetY/(f32)fontatlasWidth);
        
        Vec2f texBottomRight;
        texBottomRight.x = ((f32)(glyph[ascii].fontatlasOffsetX+glyph[ascii].width)/(f32)fontatlasWidth);
        texBottomRight.y = ((f32)(glyph[ascii].fontatlasOffsetY+glyph[ascii].height)/(f32)fontatlasWidth);
        
        eglDrawRect(topLeft, &bottomRight, &texTopLeft, &texBottomRight);
        topLeft->x += glyph[ascii].advance;
    }
}