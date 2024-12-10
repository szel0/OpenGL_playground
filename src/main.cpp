#define _USE_MATH_DEFINES

#include <iostream>
#include "Mesh.h"
// #include <glad/gl.h>
// #include <GLFW/glfw3.h>
// //#include <math.h>
// #include <stb/stb_image.h>

// #include "linmath.h"
// #include "Texture.h"
// #include "shaderClass.h"
// #include "VBO.h"
// #include "EBO.h"
// #include "VAO.h"
// #include "Camera.h"

using namespace std;

const unsigned int width = 800;
const unsigned int height = 800;
vec3 lightPos = {2.0f, 2.0f, 0.0f};

bool ifCamera = true;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);   
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        ifCamera = !ifCamera;
}


// Koordynaty wierzcholkow
Vertex vertices[] = {
    //              Coordinates             Normals                    Colors              TexCoords
    // Dolna podstawa
    Vertex { { -0.5f, -0.5f, 0.5f }, { -0.577f, -0.577f, 0.577f }, { 1.f, 0.f, 0.f }, { 0.0f, 0.0f } },  // A
    Vertex { {  0.5f, -0.5f, 0.5f }, {  0.577f, -0.577f, 0.577f }, { 1.f, 0.f, 0.f }, { 1.0f, 0.0f } },  // B
    Vertex { {  0.5f, -0.5f, -0.5f }, {  0.577f, -0.577f, -0.577f }, { 1.f, 0.f, 0.f }, { 1.0f, 1.0f } }, // C
    Vertex { { -0.5f, -0.5f, -0.5f }, { -0.577f, -0.577f, -0.577f }, { 1.f, 0.f, 0.f }, { 0.0f, 1.0f } },  // D

    // Przednia ściana
    Vertex { { -0.5f, -0.5f, 0.5f }, { -0.577f, -0.577f, 0.577f }, { 0.f, 1.f, 0.f }, { 0.0f, 0.0f } },  // A
    Vertex { {  0.5f, -0.5f, 0.5f }, {  0.577f, -0.577f, 0.577f }, { 0.f, 1.f, 0.f }, { 1.0f, 0.0f } },  // B
    Vertex { {  0.5f,  0.5f, 0.5f }, {  0.577f,  0.577f, 0.577f }, { 0.f, 1.f, 0.f }, { 1.0f, 1.0f } },  // F
    Vertex { { -0.5f,  0.5f, 0.5f }, { -0.577f,  0.577f, 0.577f }, { 0.f, 1.f, 0.f }, { 0.0f, 1.0f } },  // E

    // Prawa ściana
    Vertex { {  0.5f, -0.5f, 0.5f }, {  0.577f, -0.577f, 0.577f }, { 0.f, 0.f, 1.f }, { 0.0f, 0.0f } },  // B
    Vertex { {  0.5f, -0.5f, -0.5f }, {  0.577f, -0.577f, -0.577f }, { 0.f, 0.f, 1.f }, { 1.0f, 0.0f } },  // C
    Vertex { {  0.5f,  0.5f, -0.5f }, {  0.577f,  0.577f, -0.577f }, { 0.f, 0.f, 1.f }, { 1.0f, 1.0f } }, // G
    Vertex { {  0.5f,  0.5f, 0.5f }, {  0.577f,  0.577f, 0.577f }, { 0.f, 0.f, 1.f }, { 0.0f, 1.0f } },  // F

    // Tylna ściana
    Vertex { { -0.5f, -0.5f, -0.5f }, { -0.577f, -0.577f, -0.577f }, { 1.f, 1.f, 0.f }, { 1.0f, 0.0f } },  // D
    Vertex { {  0.5f, -0.5f, -0.5f }, {  0.577f, -0.577f, -0.577f }, { 1.f, 1.f, 0.f }, { 0.0f, 0.0f } },  // C
    Vertex { {  0.5f,  0.5f, -0.5f }, {  0.577f,  0.577f, -0.577f }, { 1.f, 1.f, 0.f }, { 0.0f, 1.0f } }, // G
    Vertex { { -0.5f,  0.5f, -0.5f }, { -0.577f,  0.577f, -0.577f }, { 1.f, 1.f, 0.f }, { 1.0f, 1.0f } },  // H

    // Lewa ściana
    Vertex { { -0.5f, -0.5f, 0.5f }, { -0.577f, -0.577f, 0.577f }, { 1.f, 0.f, 1.f }, { 1.0f, 0.0f } },  // A
    Vertex { { -0.5f, -0.5f, -0.5f }, { -0.577f, -0.577f, -0.577f }, { 1.f, 0.f, 1.f }, { 0.0f, 0.0f } },  // D
    Vertex { { -0.5f,  0.5f, -0.5f }, { -0.577f,  0.577f, -0.577f }, { 1.f, 0.f, 1.f }, { 0.0f, 1.0f } },  // H
    Vertex { { -0.5f,  0.5f, 0.5f }, { -0.577f,  0.577f, 0.577f }, { 1.f, 0.f, 1.f }, { 1.0f, 1.0f } },  // E

    // Górna podstawa
    Vertex { { -0.5f,  0.5f, 0.5f }, { -0.577f,  0.577f, 0.577f }, { 1.f, 1.f, 0.f }, { 1.0f, 1.0f } },  // E
    Vertex { {  0.5f,  0.5f, 0.5f }, {  0.577f,  0.577f, 0.577f }, { 1.f, 1.f, 0.f }, { 0.0f, 1.0f } },  // F
    Vertex { {  0.5f,  0.5f, -0.5f }, {  0.577f,  0.577f, -0.577f }, { 1.f, 1.f, 0.f }, { 0.0f, 0.0f } }, // G
    Vertex { { -0.5f,  0.5f, -0.5f }, { -0.577f,  0.577f, -0.577f }, { 1.f, 1.f, 0.f }, { 1.0f, 0.0f } }  // H
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

Vertex lightVertices[] = {
    //     COORDINATES     //
    Vertex { { -0.1f, -0.1f,  0.1f } },
    Vertex { { -0.1f, -0.1f, -0.1f } },
    Vertex { {  0.1f, -0.1f, -0.1f } },
    Vertex { {  0.1f, -0.1f,  0.1f } },
    Vertex { { -0.1f,  0.1f,  0.1f } },
    Vertex { { -0.1f,  0.1f, -0.1f } },
    Vertex { {  0.1f,  0.1f, -0.1f } },
    Vertex { {  0.1f,  0.1f,  0.1f } }
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


    vector <Vertex> verts(vertices, 1 + vertices + sizeof(vertices)/sizeof(Vertex));
    vector <GLuint> inds(indices, indices + sizeof(indices)/sizeof(GLuint));
    Texture tex("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\chihuahua.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);

    Mesh cube(verts, inds, tex);



    // Tworzymy light shader
    Shader lightShader("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\light.vert", 
                         "C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\light.frag");

	vector <Vertex> lVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	vector <GLuint> lInds(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

	Mesh light(lVerts, lInds, tex);


    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};

    lightShader.Activate();
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);



    vec3 cubePos = {1.0f, 1.0f, 0.0f};
    mat4x4 cubeModel;
    mat4x4_identity(cubeModel);
    mat4x4_translate(cubeModel, cubePos[0], cubePos[1], cubePos[2]);

    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, (const GLfloat*)cubeModel);




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

        shaderProgram.Activate();
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
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



        cube.Draw(shaderProgram, camera);
        light.Draw(lightShader, camera);

        // Zmieniamy bufor
        glfwSwapBuffers(window);

        // Obsługa zdarzeń
        glfwPollEvents();
    }


    // Usuwamy wszystkie stworzone obiekty
    shaderProgram.Delete();
    lightShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}