#include "editor.h"

static void
LoadGLMVP(u32 windowWidth, u32 windowHeight)
{
    mat4 orthographic;
    float orthoLeft = 0.0f;
    float orthoRight = (float)windowWidth;
    float orthoBottom = (float)windowHeight;
    float orthoTop = 0.0f;
    float orthoNear = 1.0f;
    float orthoFar = -1.0f;
    glm_ortho(orthoLeft, orthoRight, orthoBottom,
              orthoTop, orthoNear, orthoFar, orthographic);
    
    glUseProgram(textShaderProgId);
    GLuint projectionLocation = glGetUniformLocation(textShaderProgId, "projection");
    if (projectionLocation == GL_INVALID_VALUE)
    {
        Assert(!"Location Error");
    }
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (GLfloat *)orthographic);
    glUseProgram(0);
    
    glUseProgram(cursorShaderProgId);
    projectionLocation = glGetUniformLocation(cursorShaderProgId, "projection");
    if (projectionLocation == GL_INVALID_VALUE)
    {
        Assert(!"Location Error");
    }
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE,
                       (GLfloat *)orthographic);
    glUseProgram(0);
}

static void
LoadGLBuffers()
{
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &cursorVAO);
    glGenBuffers(1, &cursorVBO);
    glBindVertexArray(cursorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cursorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

#if 0
static void LoadCodepointTextures(unsigned char* fontBuffer)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    stbtt_fontinfo font;
    int32_t font_offset = 0;
    
    font_offset = stbtt_GetFontOffsetForIndex(fontBuffer, 0);
    stbtt_InitFont(&font, fontBuffer, font_offset);
    float scale = stbtt_ScaleForPixelHeight(&font, 15.0f);
    
    int32_t ascent;
    int32_t descent;
    int32_t line_gap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
    
    for (unsigned int c = 0; c < (ASCII_HIGH + 1); c++) {
        unsigned char *bitmap;
        int32_t bitmap_width, bitmap_height;
        int32_t advance_width, left_side_bearing;
        int32_t xoffset, yoffset;
        bitmap = stbtt_GetCodepointBitmap(&font, scale, scale, c, &bitmap_width,
                                          &bitmap_height, &xoffset, &yoffset);
        stbtt_GetCodepointHMetrics(&font, c, &advance_width, &left_side_bearing);
        
        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap_width, bitmap_height, 0, GL_RED,
                     GL_UNSIGNED_BYTE, bitmap);
        
        Characters[c].tex_id = texture_id;
        Characters[c].bitmap = bitmap;
        Characters[c].width = bitmap_width;
        Characters[c].height = bitmap_height;
        Characters[c].xoffset = xoffset;
        Characters[c].yoffset = yoffset;
        Characters[c].advance = advance_width * scale;
        Characters[c].left_side_bearing = left_side_bearing * scale;
        Characters[c].ascent = ascent * scale;
        Characters[c].descent = descent * scale;
        Characters[c].line_gap = line_gap * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}
#endif

GLuint LoadGLShaders(char *vertShaderBuffer, char *fragShaderBuffer)
{
    GLchar errorLog[1024];
    GLint status = 0;
    
    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderId, 1, &vertShaderBuffer, 0);
    glCompileShader(vertShaderId);
    glGetShaderiv(vertShaderId, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(vertShaderId, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    
    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderId, 1, &fragShaderBuffer, 0);
    glCompileShader(fragShaderId);
    glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(fragShaderId, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    
    GLuint shaderProgId = glCreateProgram();
    glAttachShader(shaderProgId, vertShaderId);
    glAttachShader(shaderProgId, fragShaderId);
    glLinkProgram(shaderProgId);
    glGetProgramiv(shaderProgId, GL_LINK_STATUS, &status);
    if (!status) {
        glGetProgramInfoLog(shaderProgId, 1024, 0, errorLog);
        OutputDebugStringA(errorLog);
        Assert(status == 1);
    }
    return shaderProgId;
}

#if 0
void DrawText(text_buffer *original, text_buffer *add) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    editor.background_color[0] = 0.0f;
    editor.background_color[1] = 0.0f;
    editor.background_color[2] = 0.0f;
    editor.background_color[3] = 1.0f;
    glClearColor(editor.background_color[0], editor.background_color[1],
                 editor.background_color[2], editor.background_color[3]);
    
    f32 cursor_xpos = (f32)editor.cursor_offset[0];
    f32 cursor_ypos = (f32)editor.cursor_offset[1];
    f32 cursor_width = Characters[ASCII_LOW].advance;
    f32 cursor_height = (Characters[ASCII_LOW].ascent - Characters[ASCII_LOW].descent);
    
    GLfloat cursor_vertex[] = {
        cursor_xpos,                cursor_ypos,                 0.0f,
        cursor_xpos,                cursor_ypos + cursor_height, 0.0f,
        cursor_xpos + cursor_width, cursor_ypos,                 0.0f,
        
        cursor_xpos,                cursor_ypos + cursor_height, 0.0f,
        cursor_xpos + cursor_width, cursor_ypos,                 0.0f,
        cursor_xpos + cursor_width, cursor_ypos + cursor_height, 0.0f
    };
    
    glUseProgram(editor.cursor_shader_prog);
    GLuint c_color_location = glGetUniformLocation(editor.cursor_shader_prog,
                                                   "c_color");
    if(c_color_location == GL_INVALID_VALUE) {
        Assert(!"Location Error")
    }
    glUniform4fv(c_color_location, 1, (GLfloat *)&editor.cursor_color);
    glBindVertexArray(editor.cursor_vao);
    glBindBuffer(GL_ARRAY_BUFFER, editor.cursor_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cursor_vertex), cursor_vertex);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUseProgram(0);
    
    f32 xpos = 0.0f;
    f32 ypos = 0.0f;
    
    for (int64_t i = 0; i < 3600; i++) {
        character c = Characters[original->memory[i]];
        
        f32 w = (f32)c.width;
        f32 h = (f32)c.height;
        f32 line_gap = (f32)(c.ascent - c.descent + c.line_gap);
        
        f32 char_offset = c.yoffset + c.ascent;
        
        if (original->memory[i] != '\r') {
            GLfloat text_vertex[] = {
                xpos + c.xoffset,     ypos + char_offset + h, 0.0f, 0.0f, 1.0f,
                xpos + c.xoffset,     ypos + char_offset,     0.0f, 0.0f, 0.0f,
                xpos + c.xoffset + w, ypos + char_offset,     0.0f, 1.0f, 0.0f,
                
                xpos + c.xoffset,     ypos + char_offset + h, 0.0f, 0.0f, 1.0f,
                xpos + c.xoffset + w, ypos + char_offset,     0.0f, 1.0f, 0.0f,
                xpos + c.xoffset + w, ypos + char_offset + h, 0.0f, 1.0f, 1.0f
            };
            
            glUseProgram(editor.text_shader_prog);
            GLuint f_color_location = glGetUniformLocation(editor.text_shader_prog,
                                                           "f_color");
            
            if (f_color_location == GL_INVALID_VALUE) {
                Assert(!"Location Error");
            }
            glUniform4fv(f_color_location, 1, (GLfloat *)&editor.text_color);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(editor.text_vao);
            glBindTexture(GL_TEXTURE_2D, c.tex_id);
            glBindBuffer(GL_ARRAY_BUFFER, editor.text_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(text_vertex), text_vertex);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glUseProgram(0);
            xpos += c.advance;
        } else {
            i++;
        }
        
        if (original->memory[i] == '\n') {
            ypos += line_gap;
            xpos = 0.0f;
        }
    }
}
#endif