#ifndef EDITOR_OPENGL_CURSOR_H
#define EDITOR_OPENGL_CURSOR_H

struct Cursor {
    Vec2f v1;
    Vec2f v2;
    u32 vao;
    u32 vbo;
    u32 ebo;
    u32 shaderProgId;
    i32 width;
    i32 height;
};

void allocateCursorMemGpu(Cursor *c, i32 width, i32 height)
{
    c->width = width;
    c->height = height;

    c->v1.x = 0.0f;
    c->v1.y = 0.0f;
    c->v2.x = (f32)(width);
    c->v2.y = (f32)(height);

    u32 indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    u32 vao;
    u32 vbo;
    u32 ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32)*5*4, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(f32)*5, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(f32)*5,
                          (void*)(sizeof(f32)*2));
    glEnableVertexAttribArray(1);

    c->vao = vao;
    c->vbo = vbo;
    c->ebo = ebo;

}

void loadCursorShader(Cursor *c)
{
    const char *vertexShader = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec3 aColor;
    
    out vec4 Color;
    
    uniform mat4 projection;
    uniform float scale;
    
    void main()
    {
        vec3 position = vec3(aPos.x, aPos.y, 0.0);
        gl_Position = projection*vec4(position*scale, 1.0);
        Color = vec4(aColor, 1.0);
    }
    )";

    const char *fragShader = R"(
    #version 330 core
    
    out vec4 FragColor;
    in vec4 Color;
    
    void main()
    {
       FragColor = Color;
    }
    )";

    c->shaderProgId = editorCreateShaderProg(vertexShader, fragShader);
}

void editorCursorInit(Cursor *c, i32 width, i32 height)
{
    allocateCursorMemGpu(c, width, height);
    loadCursorShader(c);
}

void editorDrawCursor(Cursor *c)
{

    f32 vertices[5*4];
    vertices[0] = c->v2.x;
    vertices[1] = c->v1.y;
    vertices[2] = 1.0f;
    vertices[3] = 1.0f;
    vertices[4] = 1.0f;
    vertices[5] = c->v2.x;
    vertices[6] = c->v2.y;
    vertices[7] = 1.0f;
    vertices[8] = 1.0f;
    vertices[9] = 1.0f;
    vertices[10] = c->v1.x;
    vertices[11] = c->v2.y;
    vertices[12] = 1.0f;
    vertices[13] = 1.0f;
    vertices[14] = 1.0f;
    vertices[15] = c->v1.x;
    vertices[16] = c->v1.y;
    vertices[17] = 1.0f;
    vertices[18] = 1.0f;
    vertices[19] = 1.0f;

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glUseProgram(c->shaderProgId);

    glBindVertexArray(c->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void editorCursorLeft(Cursor *c, i32 width)
{
    c->v1.x -= width;
    c->v2.x -= width;
}

void editorCursorRight(Cursor *c, i32 width)
{
    c->v1.x += width;
    c->v2.x += width;
}

void editorCursorUp(Cursor *c)
{
    c->v1.y -= c->height;
    c->v2.y -= c->height;
}

void editorCursorDown(Cursor *c)
{
    c->v1.y += c->height;
    c->v2.y += c->height;
}

#endif // EDITOR_OPENGL_CURSOR_H
