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

using namespace std;

const unsigned int width = 800;
const unsigned int height = 800;


// Koordynaty wierzcholkow
GLfloat vertices[] = {
	// Dolna podstawa
    -0.5f, -0.5f, -0.5f,    1.f, 0.f, 0.f,  0.0f, 0.0f, // A
     0.5f, -0.5f, -0.5f,    1.f, 0.f, 0.f,  0.0f, 0.1f, // B
     0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f,  1.0f, 1.0f, // C
    -0.5f, -0.5f,  0.5f,    1.f, 0.f, 0.f,  1.0f, 0.0f, // D

    // Przednia ściana
    -0.5f, -0.5f, -0.5f,    0.f, 1.f, 0.f,  0.0f, 0.0f, // A
     0.5f, -0.5f, -0.5f,    0.f, 1.f, 0.f,  1.0f, 0.0f, // B
     0.5f,  0.5f, -0.5f,    0.f, 1.f, 0.f,  1.0f, 1.0f, // F
    -0.5f,  0.5f, -0.5f,    0.f, 1.f, 0.f,  0.0f, 1.0f, // E

    // Prawa ściana
     0.5f, -0.5f, -0.5f,    0.f, 0.f, 1.f,  0.0f, 0.0f, // B
     0.5f, -0.5f,  0.5f,    0.f, 0.f, 1.f,  1.0f, 0.0f, // C
     0.5f,  0.5f,  0.5f,    0.f, 0.f, 1.f,  1.0f, 1.0f, // G
     0.5f,  0.5f, -0.5f,    0.f, 0.f, 1.f,  0.0f, 1.0f, // F

    // Tylna ściana
    -0.5f, -0.5f,  0.5f,    1.f, 1.f, 0.f,  1.0f, 0.0f, // D
     0.5f, -0.5f,  0.5f,    1.f, 1.f, 0.f,  0.0f, 0.0f, // C
     0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f,  0.0f, 1.0f, // G
    -0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f,  1.0f, 1.0f, // H

    // Lewa ściana
    -0.5f, -0.5f, -0.5f,    1.f, 0.f, 1.f,  1.0f, 0.0f, // A
    -0.5f, -0.5f,  0.5f,    1.f, 0.f, 1.f,  0.0f, 0.0f, // D
    -0.5f,  0.5f,  0.5f,    1.f, 0.f, 1.f,  0.0f, 1.0f, // H
    -0.5f,  0.5f, -0.5f,    1.f, 0.f, 1.f,  1.0f, 1.0f, // E

    // Górna podstawa
    -0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f,  0.0f, 0.0f, // E
     0.5f,  0.5f, -0.5f,    1.f, 1.f, 0.f,  1.0f, 0.0f, // F
     0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f,  1.0f, 1.0f, // G
    -0.5f,  0.5f,  0.5f,    1.f, 1.f, 0.f,  0.0f, 1.0f  // H
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

int main(){
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
    // Sprawdzamy, czy okno poprawnie sie utworzylo 
    if (window == NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    // Wprowadzamy okno do bieżącego kontekstu
    glfwMakeContextCurrent(window);

    // Ladujemy GLAD, aby skonfigurowal OpenGL'a
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Okreslamy obszar widoku OpenGL'a w oknie
    // W tym przypadku (x, y) = ([0, width], [0, height])
    glViewport(0, 0, width, height);

    // Tworzymy obiekt shaderProgram typu Shader uzywajac shaderow z Recource Files\Shaders
    Shader shaderProgram("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\default.vert", 
                         "C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Shaders\\default.frag");


    // Tworzymy Vertex Array Object i go wiazemy
    VAO VAO1;
    VAO1.Bind();

    // Tworzymy Vertex Buffer Object i laczymy go z wierzcholkami
    VBO VBO1(vertices, sizeof(vertices));
    // Tworzymy Element Buffer Object i laczymy go z indeksami
    EBO EBO1(indices, sizeof(indices));

    // Laczymy VBO z VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Odwiazujemy wszystko, zeby przez przypadek nie zmodyfikowac
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

    Texture chihuahua("C:\\Users\\user\\Desktop\\Szelo\\VR\\graniastoslup\\Resource Files\\Textures\\chihuahua.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	chihuahua.texUnit(shaderProgram, "tex0", 0);

    mat4x4 model, view, proj;

    glEnable(GL_DEPTH_TEST);

    // Glowna petla while
    while(!glfwWindowShouldClose(window)){
        // Okreslamy kolor tla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Czyscimy bufor tylni i przypisujemy mu nowy kolor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Przekazujemy OpenGL, ktorego programu shaderow chcemy uzyc
        shaderProgram.Activate();

        mat4x4_identity(model);
        mat4x4_identity(view);
        mat4x4_identity(proj);

        mat4x4_rotate(model, model, 0.5f, 1.0f, 0.3f, (float) glfwGetTime());
        mat4x4_translate(view, 0.0f, 0.0f, -5.0f);
        mat4x4_perspective(proj, 45.0f * M_PI / 180.0f, float(width/height), 0.1f, 100.0f);

        int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*) model);

        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*) view);

        int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (const GLfloat*) proj);


        glUniform1f(uniID, 0.5f);
        chihuahua.Bind();
        // Wiazemy VAO, aby OpenGL wiedzial, ze ma go uzywac
        VAO1.Bind();
        // Rysujemy trojkaty
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

        // Zmieniamy bufor tylni z przednim
        glfwSwapBuffers(window);

        // Obslugujemy wszystkie zdarzenia GLFW
        glfwPollEvents();
    }

    // Usuwamy wszystkie stworzone obiekty
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    chihuahua.Delete();
    shaderProgram.Delete();
    // Usuwamy okno przed zakonczeniem programu
    glfwDestroyWindow(window);

    // Zwalniamy zasoby GLFW przed zakonczeniem programu
    glfwTerminate();
    return 0;
}