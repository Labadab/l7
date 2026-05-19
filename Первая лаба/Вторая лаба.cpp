//Первое задание
// #define GLFW_DLL
//#define GLEW_DLL
//#include "glew-2.1.0/include/GL/glew.h"
//#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
//#include <iostream>
//
//float vertex_cord[] = {
//    0.0f,  0.5f, 0.0f,
//    0.5f, -0.5f, 0.0f,
//   -0.5f, -0.5f, 0.0f
//};
//
//GLuint index[] = {
//   0, 1, 2
//};
//
//int main()
//{
//    if (!glfwInit()) {
//        fprintf(stderr, "ERROR GLFW Init: \n");
//        return -1;
//    }
//
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    GLFWwindow* Okno;
//    Okno = glfwCreateWindow(512, 512, "Okno", NULL, NULL);
//
//    if (!Okno) {
//        glfwTerminate();
//        return -1;
//    }
//
//    glfwMakeContextCurrent(Okno);
//
//    GLenum ret = glewInit();
//    if (GLEW_OK != ret) {
//        fprintf(stderr, "ERROR GLEW Init: %s\n", glewGetErrorString(ret));
//        return -2;
//    }
//
//    GLuint VAO, VBO, EBO;
//
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//
//    glBindVertexArray(VAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_cord), vertex_cord, GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glBindVertexArray(0);
//
//    const char* vertex_shader =
//        "#version 330 core\n"
//        "layout (location=0) in vec3 vp;"
//        "void main() {"
//        "gl_Position = vec4(vp, 1.0);"
//        "}";
//
//    const char* fragment_shader =
//        "#version 330 core\n"
//        "out vec4 Color;"
//        "uniform vec4 dynamicColor;"
//        "void main() {"
//        "Color = dynamicColor;"
//        "}";
//
//    GLuint vert_SH = glCreateShader(GL_VERTEX_SHADER);
//    GLuint frag_SH = glCreateShader(GL_FRAGMENT_SHADER);
//
//    glShaderSource(vert_SH, 1, &vertex_shader, NULL);
//    glShaderSource(frag_SH, 1, &fragment_shader, NULL);
//
//    glCompileShader(vert_SH);
//    glCompileShader(frag_SH);
//
//    GLuint shader_prog = glCreateProgram();
//
//    glAttachShader(shader_prog, vert_SH);
//    glAttachShader(shader_prog, frag_SH);
//
//    glLinkProgram(shader_prog);
//
//    glUseProgram(shader_prog);
//
//    while (!glfwWindowShouldClose(Okno)) {
//        glClearColor(0.2f, 1.0f, 0.5f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        float timeValue = glfwGetTime();
//        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
//
//        int vertexColorLocation = glGetUniformLocation(shader_prog, "dynamicColor");
//        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
//
//        glBindVertexArray(VAO);
//        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
//
//        glfwSwapBuffers(Okno);
//        glfwPollEvents();
//    }
//
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
//    glDeleteProgram(shader_prog);
//    glDeleteShader(vert_SH);
//    glDeleteShader(frag_SH);
//    glfwTerminate();
//}

//Второе задание
#define GLFW_DLL
#define GLEW_DLL
#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>

GLuint LoadShader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vFile(vertexPath), fFile(fragmentPath);
    vertexCode = std::string((std::istreambuf_iterator<char>(vFile)), std::istreambuf_iterator<char>());
    fragmentCode = std::string((std::istreambuf_iterator<char>(fFile)), std::istreambuf_iterator<char>());

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* vSrc = vertexCode.c_str();
    const char* fSrc = fragmentCode.c_str();
    glShaderSource(vertex, 1, &vSrc, NULL);
    glShaderSource(fragment, 1, &fSrc, NULL);
    glCompileShader(vertex);
    glCompileShader(fragment);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

void setUniform(GLuint program, const char* name, float v0, float v1, float v2, float v3)
{
    glUniform4f(glGetUniformLocation(program, name), v0, v1, v2, v3);
}

float vertex_cord[] = {
    0.0f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
   -0.5f, -0.5f, 0.0f
};

GLuint index[] = { 0, 1, 2 };

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Okno = glfwCreateWindow(512, 512, "Okno", NULL, NULL);
    if (!Okno) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(Okno);
    glewInit();

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_cord), vertex_cord, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint shaderProg = LoadShader("vertex.glsl", "fragment.glsl");

    while (!glfwWindowShouldClose(Okno)) {
        glClearColor(0.2f, 1.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float greenValue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
        glUseProgram(shaderProg);
        setUniform(shaderProg, "dynamicColor", 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(Okno);
        glfwPollEvents();
    }

    glfwTerminate();
}
