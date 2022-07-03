#ifndef EDITOR_OPENGL_TEXT_H
#define EDITOR_OPENGL_TEXT_H

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct FontGlyphs {
    i32 width;
    i32 height;
    i32 xoffset;
    i32 yoffset;
    i32 leftSideBearing;
    i32 advance;
    
    u32 atlasXoffset;
    u32 atlasYoffset;
};

struct FontMetrics {
    i32 ascent;
    i32 lineGap;
    i32 descent;
};

struct FontAtlas {
    FontGlyphs g[128];
    FontMetrics f;
    u32 width = 512;
    u32 texId;
};

void generateFontAtlas(unsigned char *fontBuffer,
                       unsigned char *fontatlasBuffer, FontAtlas *atlas)
{
    stbtt_fontinfo font;
    i32 fontOffset = 0;
    f32 fontSize = 32.0f;
    
    fontOffset = stbtt_GetFontOffsetForIndex(fontBuffer, 0);
    stbtt_InitFont(&font, fontBuffer, fontOffset);
    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    
    i32 ascent;
    i32 descent;
    i32 lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    
    ascent = (i32)floorf(scale * ascent);
    descent = (i32)floorf(scale * descent);
    lineGap = (i32)floorf(scale * lineGap);
    
    atlas->f.ascent = ascent;
    atlas->f.descent = descent;
    atlas->f.lineGap = lineGap;
    
    u32 border = 2;
    u32 atlasXOffset = border;
    u32 atlasYOffset = border;
    
    for (u32 c = ' '; c < 128; c++)
    {
        unsigned char *bitmap;
        i32 bitmapWidth, bitmapHeight;
        i32 advanceWidth, leftSideBearing;
        i32 xoffset, yoffset;
        bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, c, &bitmapWidth,
                                          &bitmapHeight, &xoffset, &yoffset);
        
        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);
        
        advanceWidth = (i32)floorf(advanceWidth*scale);
        leftSideBearing = (i32)floorf(leftSideBearing*scale);
        
        if (atlasXOffset + bitmapWidth >= atlas->width)
        {
            atlasXOffset = border;
            atlasYOffset += (ascent - descent) + border;
        }
        
        for (i32 y = 0; y < bitmapHeight; y++)
        {
            for (i32 x = 0; x < bitmapWidth; x++)
            {
                i32 fontatlasIndex = ((y + atlasYOffset)*atlas->width) + x + atlasXOffset;
                i32 bitmapIndex = (y*bitmapWidth) + x;
                
                fontatlasBuffer[fontatlasIndex] = bitmap[bitmapIndex];
                fontatlasBuffer[fontatlasIndex + 1] = 0;
                fontatlasBuffer[fontatlasIndex + 2] = 0;
                fontatlasBuffer[fontatlasIndex + 3] = 0;
            }
        }
        
        atlas->g[c].width = bitmapWidth;
        atlas->g[c].height = bitmapHeight;
        atlas->g[c].xoffset = xoffset;
        atlas->g[c].yoffset = yoffset;
        atlas->g[c].leftSideBearing = leftSideBearing;
        atlas->g[c].advance = advanceWidth;
        atlas->g[c].atlasXoffset = atlasXOffset;
        atlas->g[c].atlasYoffset = atlasYOffset;
        
        atlasXOffset += bitmapWidth + border;
    }
}

GLuint generateFontAtlasTexture(GLuint width, unsigned char *data)
{
    GLuint texId;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, width, 0, GL_RED,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texId;
}

void editorFontInit(unsigned char *fontBuffer, unsigned char *fontAtlasBuffer,
                    FontAtlas *fa, Batch *fb, u32 *indices)
{
    generateFontAtlas(fontBuffer, fontAtlasBuffer, fa);
    fb->texId = generateFontAtlasTexture(fa->width, fontAtlasBuffer);

    const char *vertexShader = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    layout (location = 2) in vec4 aColor;
    
    out vec2 TexCoord;
    out vec4 Color;
    
    uniform mat4 projection;
    uniform float scale;
    
    void main()
    {
        vec3 position = vec3(aPos.x, aPos.y, 0.0);
        gl_Position = projection*vec4(position*scale, 1.0);
        TexCoord = aTexCoord;
        Color = aColor;
    }
    )";

    const char *fragmentShader = R"(
    #version 330 core
    
    out vec4 FragColor;
    
    in vec2 TexCoord;
    in vec4 Color;
    
    uniform sampler2D texture0;
    
    void main()
    {
       float color = texture(texture0, TexCoord).r;
       FragColor = vec4(1.0, 1.0, 1.0, color)*Color;
    }
    )";

    fb->shaderProgId = editorCreateShaderProg(vertexShader, fragmentShader);

    allocateBatchMemGpu(fb, indices);
}

void editorDrawBuffer(f32 x, f32 y, GapBuffer *gb, FontAtlas *a, Batch *batch,
                      Vertex *vertices)
{
    f32 lineGap = 0.0f;

    for (size_t i = 0; i < gb->bufferSize; i++)
    {
        size_t cursorOffset = gb->cursorOffset;
        size_t backStart = gb->cursorOffset + gb->gapSize;
        size_t backLength = gb->bufferSize - gb->cursorOffset - gb->gapSize;
        size_t backEnd = backStart + backLength;

        if (i < cursorOffset || (i >= backStart && backEnd > i))
        {
            char ascii = gb->buffer[i];
            if (ascii == '\n')
            {
                x = 0;
                lineGap += (f32)(a->f.ascent - a->f.descent);
                continue;
            }

            if (ascii == '\r')
            {
                continue;
            }

            f32 baseline = (f32)(a->g[ascii].yoffset + a->f.ascent);
            y = baseline + lineGap;

            Vec2f v1;
            v1.x = x;
            v1.y = y;

            Vec2f v2;
            v2.x = (f32)(v1.x + a->g[ascii].width);
            v2.y = (f32)(v1.y + a->g[ascii].height);

            Vec2f texV1;
            texV1.x = ((f32)a->g[ascii].atlasXoffset/(f32)a->width);
            texV1.y = ((f32)a->g[ascii].atlasYoffset/(f32)a->width);

            Vec2f texV2;
            texV2.x = ((f32)(texV1.x + ((f32)a->g[ascii].width/(f32)a->width)));
            texV2.y = ((f32)(texV1.y + ((f32)a->g[ascii].height/(f32)a->width)));

            pushCharToGpu(&v1, &v2, &texV1, &texV2, batch, vertices);
            x += a->g[ascii].advance;
        }
    }
}

#endif // EDITOR_OPENGL_TEXT_H
