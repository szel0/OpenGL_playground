#define _USE_MATH_DEFINES

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
//#include <math.h>
#include <stb/stb_image.h>

#include "linmath.h"
#include "Texture.h"
#include "shaderClass.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "Camera.h"

using namespace std;

const unsigned int width = 800;
const unsigned int height = 800;
vec3 lightPos = {2.0f, 2.0f, 0.0f};

bool ifCamera = true;

int shaderType = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);   
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        ifCamera = !ifCamera; 
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        shaderType += 1;
        shaderType %= 4;
    }
}

// Tablica z pozycjami sześcianów
vec3 cubePositions[5] = {
    { 1.0f,  1.0f,  0.0f}, 
    { 2.5f,  1.0f,  0.5f},
    { 1.5f,  2.5f, -1.5f}, 
    { 1.5f,  3.5f,  1.0f}, 
    { 3.0f,  3.0f,  0.0f}
};


// Koordynaty wierzcholkow
GLfloat vertices[] = {
    //    coordinates           COLOR           texCoords           vertex normals
	// Dolna podstawa
    -0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f,    0.0f, 0.0f,    -0.577f, -0.577f,  0.577f,  // A
     0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f,    1.0f, 0.0f,     0.577f, -0.577f,  0.577f,  // B
     0.5f, -0.5f, -0.5f,    1.f, 0.f, 0.f,    1.0f, 1.0f,     0.577f, -0.577f, -0.577f,  // C
    -0.5f, -0.5f, -0.5f,    1.f, 0.f, 0.f,    0.0f, 1.0f,    -0.577f, -0.577f, -0.577f,  // D

    // Przednia ściana
    -0.5f, -0.5f,  0.5f,    0.f, 1.f, 0.f,    0.0f, 0.0f,    -0.577f, -0.577f,  0.577f,  // A
     0.5f, -0.5f,  0.5f,    0.f, 1.f, 0.f,    1.0f, 0.0f,     0.577f, -0.577f,  0.577f,  // B
     0.5f,  0.5f,  0.5f,    0.f, 1.f, 0.f,    1.0f, 1.0f,     0.577f,  0.577f,  0.577f,  // F
    -0.5f,  0.5f,  0.5f,    0.f, 1.f, 0.f,    0.0f, 1.0f,    -0.577f,  0.577f,  0.577f,  // E

    // Prawa ściana
     0.5f, -0.5f,  0.5f,    0.f, 0.f, 1.f,    0.0f, 0.0f,     0.577f, -0.577f,  0.577f,  // B
     0.5f, -0.5f, -0.5f,    0.f, 0.f, 1.f,    1.0f, 0.0f,     0.577f, -0.577f, -0.577f,  // C
     0.5f,  0.5f, -0.5f,    0.f, 0.f, 1.f,    1.0f, 1.0f,     0.577f,  0.577f, -0.577f,  // G
     0.5f,  0.5f,  0.5f,    0.f, 0.f, 1.f,    0.0f, 1.0f,     0.577f,  0.577f,  0.577f,  // F

    // Tylna ściana
    -0.5f, -0.5f, -0.5f,    1.f, 1.f, 0.f,    1.0f, 0.0f,    -0.577f, -0.577f, -0.577f,  // D
     0.5f, -0.5f, -0.5f,    1.f, 1.f, 0.f,    0.0f, 0.0f,     0.577f, -0.577f, -0.577f,  // C
     0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f,    0.0f, 1.0f,     0.577f,  0.577f, -0.577f,  // G
    -0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f,    1.0f, 1.0f,    -0.577f,  0.577f, -0.577f,  // H

    // Lewa ściana
    -0.5f, -0.5f,  0.5f,    1.f, 0.f, 1.f,    1.0f, 0.0f,    -0.577f, -0.577f,  0.577f,  // A
    -0.5f, -0.5f, -0.5f,    1.f, 0.f, 1.f,    0.0f, 0.0f,    -0.577f, -0.577f, -0.577f,  // D
    -0.5f,  0.5f, -0.5f,    1.f, 0.f, 1.f,    0.0f, 1.0f,    -0.577f,  0.577f, -0.577f,  // H
    -0.5f,  0.5f,  0.5f,    1.f, 0.f, 1.f,    1.0f, 1.0f,    -0.577f,  0.577f,  0.577f,  // E

    // Górna podstawa
    -0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f,    1.0f, 1.0f,    -0.577f,  0.577f,  0.577f,  // E
     0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f,    0.0f, 1.0f,     0.577f,  0.577f,  0.577f,  // F
     0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f,    0.0f, 0.0f,     0.577f,  0.577f, -0.577f,  // G
    -0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f,    1.0f, 0.0f,    -0.577f,  0.577f, -0.577f,  // H
};

// Indeksy okreslajace kolejnosc wierzcholkow
GLuint indices[] = {
    // Dolna podstawa
    0, 1, 2,  0, 2, 3, // ABC, ACD

    // Przednia ściana
    4, 5, 6,  4, 6, 7, // ABF, AFE

    // Prawa ściana
    8, 9, 10,  8, 10, 11, // BCG, BGF

    // Tylna ściana
    12, 13, 14,  12, 14, 15, // DCG, DGH

    // Lewa ściana
    16, 17, 18,  16, 18, 19, // ADH, AHE

    // Górna podstawa
    20, 21, 22,  20, 22, 23  // EFG, EGH
};

GLfloat lightVertices[] =
{ //     COORDINATES     //
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


int main() {
    // Inicjalizacja GLFW
    glfwInit();

    // Informujemy GLFW, ze uzywam OpenGL'a w wersji 3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Informujemy GLFW, ze uzywamy profilu CORE
    // Oznacza to, ze mamy dostep tylko do nowoczesnych funkcji
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzymy obiekt okna GLFW o wielkosci width x heigth [px], nazwany "SzelkowskiOpenGL"
    GLFWwindow* window = glfwCreateWindow(width, height, "SzelkowskiOpenGL", NULL, NULL);
    if (window == NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    // Wprowadzamy okno do bieżącego kontekstu
    glfwMakeContextCurrent(window);

    // Ladujemy GLAD
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, width, height);

    // Tworzymy shader program
    Shader shaderProgram("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\default.vert", 
                         "C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\default.frag");

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    Shader lightShader("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\light.vert", 
                         "C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\light.frag");

    VAO lightVAO;
    lightVAO.Bind();

    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));

    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};


    lightShader.Activate();

    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);

    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);



    // Inicjalizujemy tekstury
    Texture textures[5] = {
        Texture("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\chihuahua.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\monalisa.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\nice.png", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\cat.png", GL_TEXTURE_2D, GL_TEXTURE3, GL_RGBA, GL_UNSIGNED_BYTE),
        Texture("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\shaq.png", GL_TEXTURE_2D, GL_TEXTURE4, GL_RGBA, GL_UNSIGNED_BYTE)
    };

    // Przypisanie tekstur do uniformów jednorazowo
    GLuint textureLoc = glGetUniformLocation(shaderProgram.ID, "textures");
    int textureIndices[5] = {0, 1, 2, 3, 4};
    glUniform1iv(textureLoc, 5, textureIndices);


    // Włączamy testowanie głębokości
    glEnable(GL_DEPTH_TEST);

    vec3 position = {2.0f, 2.0f, 5.0f};
    Camera camera(width, height, position);

    float FOV = 45.0f;

    // Pętla renderująca
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 lightModel;
        mat4x4_identity(lightModel);
        mat4x4_translate(lightModel, lightPos[0], lightPos[1], lightPos[2]);


        lightShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, (const GLfloat*)lightModel);

        // Aktywuj shader
        shaderProgram.Activate();
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "shaderType"), shaderType);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position[0], camera.Position[1], camera.Position[2]);


        // Wektor pomocniczy do ruchu po plaszcznie X kamery
        vec3 right;
        vec3_mul_cross(right, camera.Orientation, camera.Up);
        vec3_norm(right, right);


        if (ifCamera){
            // Zmiana FOV
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
                if (FOV - 0.01f >= 10.0f) FOV -= 0.01f;
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) 
                if (FOV + 0.01f <= 120.0f) FOV += 0.01f;

            // Zmiana kamery
            camera.Inputs(window);
            camera.updateMatrix(FOV, 0.1f, 100.0f);
            camera.Matrix(shaderProgram);
        } else { 
            // Zmiana pozycji światła
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, camera.Orientation, 0.0003f); // Ruch do przodu
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, camera.Orientation, -0.0003f); // Ruch do tyłu
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, right, -0.0003f); // Ruch w lewo
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, right, 0.0003f); // Ruch w prawo
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                lightPos[1] += 0.0003f;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                lightPos[1] -= 0.0003f;  
        }

        // Renderowanie sześcianów
        for (int i = 0; i < 5; i++) {
            mat4x4 model;
            mat4x4_identity(model);
            mat4x4_translate(model, cubePositions[i][0], cubePositions[i][1], cubePositions[i][2]);

            // Przekazanie macierzy modelu do shadera
            GLuint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)model);

            // Aktywowanie odpowiedniej jednostki tekstury
            glActiveTexture(GL_TEXTURE0 + i); // Aktywuj jednostkę tekstury

            // Wiązanie odpowiedniej tekstury
            textures[i].Bind();

            // Ustawienie 'type' do wyboru tekstury
            glUniform1i(glGetUniformLocation(shaderProgram.ID, "type"), i); // 'i' będzie wskazywać, która tekstura ma być użyta

            // Wiązanie VAO
            VAO1.Bind();

            // Rysowanie sześcianu
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        }

        lightShader.Activate();
        camera.Matrix(lightShader);
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Zmieniamy bufor
        glfwSwapBuffers(window);

        // Obsługa zdarzeń
        glfwPollEvents();
    }


    // Usuwamy wszystkie stworzone obiekty
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    for (int i = 0; i < 5; i++) {
        textures[i].Delete();
    }
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}