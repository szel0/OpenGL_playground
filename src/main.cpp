#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stb/stb_image.h>

#include "Texture.h"
#include "shaderClass.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"

using namespace std;

// Koordynaty wierzcholkow
GLfloat vertices[] = {
// KOODRYNATY WIERZCHOLKOW  |     KOLORY (RGB)       |     TexCoord
    -0.5f, -0.5f, 0.0f,         1.0f, 0.0f, 0.0f,         0.0f, 0.0f, // Lewy dolny
    -0.5f,  0.5f, 0.0f,         0.0f, 1.0f, 0.0f,         0.0f, 1.0f, // Lewy gorny
     0.5f,  0.5f, 0.0f,         0.0f, 0.0f, 1.0f,         1.0f, 1.0f, // Prawy gorny
     0.5f, -0.5f, 0.0f,         1.0f, 1.0f, 1.0f,         1.0f, 0.0f  // Lewy dolny
};

// Indeksy okreslajace kolejnosc wierzcholkow
GLuint indices[] = {
    0, 2, 1, // Gorny trojkat
    0, 3, 2 // Dolny Trojkat
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

    // Tworzymy obiekt okna GLFW o wielkosci 800x800px, nazwany "SzelkowskiOpenGL"
    GLFWwindow* window = glfwCreateWindow(800, 800, "SzelkowskiOpenGL", NULL, NULL);
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
    // W tym przypadku (x, y) = ([0, 800], [0, 800])
    glViewport(0, 0, 800, 800);

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

    // Glowna petla while
    while(!glfwWindowShouldClose(window)){
        // Okreslamy kolor tla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Czyscimy bufor tylni i przypisujemy mu nowy kolor
        glClear(GL_COLOR_BUFFER_BIT);
        // Przekazujemy OpenGL, ktorego programu shaderow chcemy uzyc
        shaderProgram.Activate();
        glUniform1f(uniID, 0.5f);
        chihuahua.Bind();
        // Wiazemy VAO, aby OpenGL wiedzial, ze ma go uzywac
        VAO1.Bind();
        // Rysujemy trojkaty
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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