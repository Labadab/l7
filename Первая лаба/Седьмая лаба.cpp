#define GLFW_DLL
#define GLEW_DLL

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
};

class Mesh {
public:
    Vertex* vertices;
    unsigned int* indices;
    int vertexCount;
    int indexCount;
    unsigned int VAO;
    string name;

    Mesh(Vertex* verts, int vertCount, unsigned int* idxs, int idxCount, const string& meshName = "")
    {
        vertices = verts;
        indices = idxs;
        vertexCount = vertCount;
        indexCount = idxCount;
        name = meshName;
        setupMesh();
    }

    ~Mesh()
    {
        delete[] vertices;
        delete[] indices;
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};

class Model3D
{
public:
    struct MeshInfo {
        Mesh* mesh;
        string name;
        glm::vec3 originalCenter;
    };

    vector<MeshInfo> meshes;
    GLuint shaderProgram;

    Model3D() : shaderProgram(0) {}

    void setShaderProgram(GLuint program) { shaderProgram = program; }

    void loadModel(const string& filePath)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenNormals);

        if (!scene) {
            cerr << "Ошибка: " << importer.GetErrorString() << endl;
            return;
        }

        processNode(scene->mRootNode, scene);
    }

    void Draw(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos,
        float xPos, float yPos, float rot)
    {
        if (!shaderProgram) return;

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));

        float angleRad = glm::radians(30.0f);
        float moveY = yPos * cos(angleRad);
        float moveZ = -yPos * sin(angleRad);

        float xOffset = xPos + 0.1f;

        for (auto& info : meshes) {
            glm::mat4 model = glm::mat4(1.0f);

            if (info.name == "Куб") {
                model = glm::mat4(1.0f);
            }
            else if (info.name == "Куб.002") {
                model = glm::translate(model, glm::vec3(xOffset, 0.0f, 0.0f));
            }
            else if (info.name == "Куб.003") {
                model = glm::translate(model, glm::vec3(xOffset, 0.0f, 0.0f));
                model = glm::translate(model, glm::vec3(0.0f, moveY, moveZ));
            }
            else if (info.name == "Куб.004") {
                glm::vec3 center = info.originalCenter;
                model = glm::translate(model, glm::vec3(xOffset, 0.0f, 0.0f));
                model = glm::translate(model, glm::vec3(0.0f, moveY, moveZ));
                model = glm::translate(model, center);
                model = glm::rotate(model, glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::translate(model, -center);
            }
            else if (info.name == "Куб.001") {
                glm::vec3 center = info.originalCenter;
                model = glm::translate(model, center);
                model = glm::rotate(model, glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::translate(model, -center);
            }

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            info.mesh->Draw();
        }
    }

private:
    void processNode(aiNode* node, const aiScene* scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            string meshName = node->mName.C_Str();
            if (meshName.empty()) meshName = "Part_" + to_string(meshes.size());
            addMesh(mesh, meshName);
        }
        for (int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void addMesh(aiMesh* mesh, const string& meshName)
    {
        Vertex* verts = new Vertex[mesh->mNumVertices];

        glm::vec3 center(0.0f);

        for (int i = 0; i < mesh->mNumVertices; i++) {
            verts[i].pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            verts[i].norm = mesh->HasNormals() ?
                glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) :
                glm::vec3(0.0f);
            center += verts[i].pos;
        }
        center /= (float)mesh->mNumVertices;

        int totalIndices = 0;
        for (int i = 0; i < mesh->mNumFaces; i++)
            totalIndices += mesh->mFaces[i].mNumIndices;

        unsigned int* idxs = new unsigned int[totalIndices];
        int pos = 0;
        for (int i = 0; i < mesh->mNumFaces; i++) {
            for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
                idxs[pos++] = mesh->mFaces[i].mIndices[j];
            }
        }

        MeshInfo info;
        info.mesh = new Mesh(verts, mesh->mNumVertices, idxs, totalIndices, meshName);
        info.name = meshName;
        info.originalCenter = center;
        meshes.push_back(info);
    }
};

const char* vertexShaderSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec3 FragPos;
    out vec3 Normal;
    
    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* fragmentShaderSrc = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec3 FragPos;
    in vec3 Normal;
    
    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };
    
    struct Light {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    
    uniform Material material;
    uniform Light light;
    uniform vec3 viewPos;
    
    void main() {
        vec3 ambient = light.ambient * material.ambient;
        
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);
        
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
)";

glm::vec3 cameraPos = glm::vec3(4.0f, 3.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastTime = 0.0f;

float yaw = -90.0f;
float pitch = -15.0f;
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

void processCameraInput(GLFWwindow* window)
{
    float speed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "CNC Machine - Lab 7", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewInit();
    glEnable(GL_DEPTH_TEST);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs, 1, &vertexShaderSrc, NULL);
    glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
    glCompileShader(vs);
    glCompileShader(fs);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.7f, 0.7f, 0.7f);
    glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), 5.0f, 8.0f, 5.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.3f, 0.3f, 0.3f);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

    Model3D myModel;
    myModel.setShaderProgram(shaderProgram);
    myModel.loadModel("model.obj");

    float xPos = 0.0f;
    float yPos = 0.0f;
    float rot = 0.0f;

    const float xMin = 0.0f, xMax = 0.4f;
    const float yMin = -0.65f, yMax = 0.0f;

    cout << "\n========== УПРАВЛЕНИЕ ==========\n";
    cout << "R / F\n";
    cout << "T / G\n";
    cout << "Q / E\n";
    cout << "WASD + МЫШЬ\n";
    cout << "==============================================\n\n";

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processCameraInput(window);

        float speed = 2.0f * deltaTime;
        float rotSpeed = 80.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) xPos -= speed;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) xPos += speed;
        xPos = max(xMin, min(xMax, xPos));

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) yPos -= speed;
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) yPos += speed;
        yPos = max(yMin, min(yMax, yPos));

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) rot += rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) rot -= rotSpeed;

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        myModel.Draw(view, projection, cameraPos, xPos, yPos, rot);

        // Тень под движущимися частями
        glUseProgram(0);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        float angleRad = glm::radians(30.0f);
        float moveY = yPos * cos(angleRad);
        float moveZ = -yPos * sin(angleRad);
        float xOffset = xPos + 0.1f;

        // Тень под Куб.002
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
        glVertex3f(xOffset - 0.3f, -0.8f, 1.0f);
        glVertex3f(xOffset + 0.3f, -0.8f, 1.0f);
        glVertex3f(xOffset + 0.3f, -0.8f, 1.6f);
        glVertex3f(xOffset - 0.3f, -0.8f, 1.6f);
        glEnd();

        // Тень под Куб.003
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
        glVertex3f(xOffset - 0.25f + moveZ * 0.3f, -0.8f, 1.2f + moveZ);
        glVertex3f(xOffset + 0.25f + moveZ * 0.3f, -0.8f, 1.2f + moveZ);
        glVertex3f(xOffset + 0.25f + moveZ * 0.3f, -0.8f, 1.8f + moveZ);
        glVertex3f(xOffset - 0.25f + moveZ * 0.3f, -0.8f, 1.8f + moveZ);
        glEnd();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glUseProgram(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}