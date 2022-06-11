#include "editor.h"

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
    f32 orthoRight = (f32)((f32)windowWidth * 3.0f);
    f32 orthoBottom = (f32)((f32)windowHeight * 3.0f);
    f32 orthoTop = 0.0f;
    f32 orthoNear = 1.0f;
    f32 orthoFar = -1.0f;
    glm_ortho(orthoLeft, orthoRight, orthoBottom,
              orthoTop, orthoNear, orthoFar, orthographic);
    
    eglLoadUniformMatrix4fv(shaderProgId, (f32 *)orthographic, "projection");
}

void allocateBatchMemGpu()
{
    u32 offset = 0;
    for (u32 i = 0; i < BATCH_INDICES_SIZE; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 3;
        indices[i + 3] = offset + 1;
        indices[i + 4] = offset + 2;
        indices[i + 5] = offset + 3;
        offset += 4;
    }

    u32 vao;
    u32 vbo;
    u32 ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertCoords));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    batch.ebo = ebo;
    batch.vbo = vbo;
    batch.vao = vao;
    batch.verticesFilled = 0;
}

static void eglLoadTexture(u32 *texId, u32 width, u32 height, unsigned char *data)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, texId);
    glBindTexture(GL_TEXTURE_2D, *texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, width, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    
    atlas.f.ascent = ascent;
    atlas.f.descent = descent;
    atlas.f.lineGap = lineGap;
    
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
        
        if (atlasXOffset + bitmapWidth >= atlas.width)
        {
            atlasXOffset = 0;
            atlasYOffset += ascent - descent;
        }
        
        for (i32 y = 0; y < bitmapHeight; y++)
        {
            for (i32 x = 0; x < bitmapWidth; x++)
            {
                i32 fontatlasIndex = ((y + atlasYOffset)*atlas.width) + x + atlasXOffset;
                i32 bitmapIndex = (y*bitmapWidth) + x;
                
                fontatlasBuffer[fontatlasIndex] = bitmap[bitmapIndex];
                fontatlasBuffer[fontatlasIndex + 1] = 0;
                fontatlasBuffer[fontatlasIndex + 2] = 0;
                fontatlasBuffer[fontatlasIndex + 3] = 0;
            }
        }
        
        atlas.g[c].width = bitmapWidth;
        atlas.g[c].height = bitmapHeight;
        atlas.g[c].xoffset = xoffset;
        atlas.g[c].yoffset = yoffset;
        atlas.g[c].leftSideBearing = leftSideBearing;
        atlas.g[c].advance = advanceWidth;
        atlas.g[c].atlasXoffset = atlasXOffset;
        atlas.g[c].atlasYoffset = atlasYOffset;
        
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

void flushGpuBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glUseProgram(shaderProgId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindVertexArray(batch.vao);
    glDrawElements(GL_TRIANGLES, BATCH_VERTICES_SIZE*6, GL_UNSIGNED_INT, 0);
}

void pushCharToGpu(Vec2f *v1, Vec2f *v2, Vec2f *tex_v1, Vec2f *tex_v2)
{
    u32 offset = batch.verticesFilled;
    if (offset >= BATCH_VERTICES_SIZE)
    {
        flushGpuBuffer();
        batch.verticesFilled = 0;
        offset = batch.verticesFilled;
    }

    vertices[offset + 0].vertCoords.x = v2->x;
    vertices[offset + 0].vertCoords.y = v1->y;
    vertices[offset + 0].texCoords.x = tex_v2->x;
    vertices[offset + 0].texCoords.y = tex_v1->y;
    vertices[offset + 0].color.x = 1.0f;
    vertices[offset + 0].color.y = 1.0f;
    vertices[offset + 0].color.z = 1.0f;
    vertices[offset + 0].color.w = 1.0f;

    vertices[offset + 1].vertCoords.x = v2->x;
    vertices[offset + 1].vertCoords.y = v2->y;
    vertices[offset + 1].texCoords.x = tex_v2->x;
    vertices[offset + 1].texCoords.y = tex_v2->y;
    vertices[offset + 1].color.x = 1.0f;
    vertices[offset + 1].color.y = 1.0f;
    vertices[offset + 1].color.z = 1.0f;
    vertices[offset + 1].color.w = 1.0f;

    vertices[offset + 2].vertCoords.x = v1->x;
    vertices[offset + 2].vertCoords.y = v2->y;
    vertices[offset + 2].texCoords.x = tex_v1->x;
    vertices[offset + 2].texCoords.y = tex_v2->y;
    vertices[offset + 2].color.x = 1.0f;
    vertices[offset + 2].color.y = 1.0f;
    vertices[offset + 2].color.z = 1.0f;
    vertices[offset + 2].color.w = 1.0f;

    vertices[offset + 3].vertCoords.x = v1->x;
    vertices[offset + 3].vertCoords.y = v1->y;
    vertices[offset + 3].texCoords.x = tex_v1->x;
    vertices[offset + 3].texCoords.y = tex_v1->y;
    vertices[offset + 3].color.x = 1.0f;
    vertices[offset + 3].color.y = 1.0f;
    vertices[offset + 3].color.z = 1.0f;
    vertices[offset + 3].color.w = 1.0f;

    batch.verticesFilled += 4;
}

void eglDrawText(f32 x, f32 y, char *text)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    f32 lineGap = 0.0f;
    
    
    for (i32 i = 0; text[i] != 0; i++)
    {
        char ascii = text[i];
        
        if (ascii == '\n')
        {
            x = 0;
            lineGap += (f32)(atlas.f.ascent - atlas.f.descent);
            ascii = text[++i];
        }
        
        if (ascii == '\r')
        {
            x = 0;
            lineGap += (f32)(atlas.f.ascent - atlas.f.descent);
            i++;
            ascii = text[++i];
        }
        
        f32 baseline = (f32)(atlas.g[ascii].yoffset + atlas.f.ascent);
        y = baseline + lineGap;
        
        Vec2f v1;
        v1.x = x;
        v1.y = y;
        
        Vec2f v2;
        v2.x = (f32)(v1.x + atlas.g[ascii].width);
        v2.y = (f32)(v1.y + atlas.g[ascii].height);
        
        Vec2f tex_v1;
        tex_v1.x = ((f32)atlas.g[ascii].atlasXoffset/(f32)atlas.width);
        tex_v1.y = ((f32)atlas.g[ascii].atlasYoffset/(f32)atlas.width);
        
        Vec2f tex_v2;
        tex_v2.x = ((f32)(tex_v1.x + ((f32)atlas.g[ascii].width/(f32)atlas.width)));
        tex_v2.y = ((f32)(tex_v1.y + ((f32)atlas.g[ascii].height/(f32)atlas.width)));

        pushCharToGpu(&v1, &v2, &tex_v1, &tex_v2);
        
        x += atlas.g[ascii].advance;
    }
}
