#include "extension.h"

#include <GL/gl3w.h>

static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationPosition = 0;
static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;
static GLuint       g_VaoHandle = 0;

bool Custom_CreateDeviceObjects()
{
    GLint last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
        "attribute vec2 vPosition;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(vPosition, 0, 1);\n"
        "}\n";

    g_ShaderHandle = glCreateProgram();
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
    glCompileShader(g_VertHandle);
    glAttachShader(g_ShaderHandle, g_VertHandle);

    const GLchar* fragment_shader =
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
    "}\n";

    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(g_FragHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);

    glLinkProgram(g_ShaderHandle);

    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "vPosition");

    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    glGenVertexArrays(1, &g_VaoHandle);
    glBindVertexArray(g_VaoHandle);

    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);

    glEnableVertexAttribArray(g_AttribLocationPosition);
    glVertexAttribPointer(
        g_AttribLocationPosition,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(ImDrawVert),
        (GLvoid*)IM_OFFSETOF(ImDrawVert, pos)
    );

    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

void Custom_DeleteDeviceObjects()
{
    if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
    if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
    g_VboHandle = g_ElementsHandle = 0;

    if (g_ShaderHandle && g_VertHandle) glDetachShader(g_ShaderHandle, g_VertHandle);
    if (g_VertHandle) glDeleteShader(g_VertHandle);
    g_VertHandle = 0;

    if (g_ShaderHandle && g_FragHandle) glDetachShader(g_ShaderHandle, g_FragHandle);
    if (g_FragHandle) glDeleteShader(g_FragHandle);
    g_FragHandle = 0;

    if (g_ShaderHandle) glDeleteProgram(g_ShaderHandle);
    g_ShaderHandle = 0;

    if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
    g_VaoHandle = 0;
}

void Custom_ImDrawCallback (const ImDrawList* cmd_list, const ImDrawCmd* pcmd){
    auto clipRect = pcmd->ClipRect;

    ImGuiIO& io = ImGui::GetIO();
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);

    // Backup GL state
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vao; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vao);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);

    auto width = (GLsizei)(clipRect.z-clipRect.x);
    auto height = (GLsizei)(clipRect.w-clipRect.y);

    glViewport(clipRect.x, fb_height-clipRect.y-height, width, height);

    glUseProgram(g_ShaderHandle);
    glBindVertexArray(g_VaoHandle);

    static const ImDrawVert g_vertex_buffer_data[] = {
        {ImVec2(-1.0f, -1.0f), ImVec2(0, 0), ImU32(0)},
        {ImVec2(1.0f, -1.0f), ImVec2(0, 0), ImU32(0)},
        {ImVec2(0.0f, 1.0f), ImVec2(0, 0), ImU32(0)}
    };

    static const unsigned int indices[] = {0, 1, 2};

    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // Restore modified GL state
    glUseProgram(last_program);
    glBindVertexArray(last_vao);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);

}
