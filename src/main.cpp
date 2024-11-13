#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "shaderClass.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"

using namespace std;

// Koordynaty wierzcholkow
GLfloat vertices[] = {
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lewy dolny
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Prawy gorny
    0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // Gorny
    -0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f, // Wewnetrzny lewy
    0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f, // Wewnetrzny prawy
    0.0f / 2, -0.5f * float(sqrt(3)) / 3, 0.0f // Wewnetrzny dolny        
};

// Indeksy okreslajace kolejnosc wierzcholkow
GLuint indices[] = {
    0, 3, 5, // Lewy dolny trojkat
    3, 2, 4, // Prawy dolny trojkat
    5, 4, 1  // Gorny trojkat 
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
    VAO1.LinkVBO(VBO1, 0);

    // Odwiazujemy wszystko, zeby przez przypadek nie zmodyfikowac
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


    // Glowna petla while
    while(!glfwWindowShouldClose(window)){
        // Okreslamy kolor tla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Czyscimy bufor tylni i przypisujemy mu nowy kolor
        glClear(GL_COLOR_BUFFER_BIT);
        // Przekazujemy OpenGL, ktorego programu shaderow chcemy uzyc
        shaderProgram.Activate();
        // Wiazemy VAO, aby OpenGL wiedzial, ze ma go uzywac
        VAO1.Bind();
        // Rysujemy trojkat
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // Zmieniamy bufor tylni z przednim
        glfwSwapBuffers(window);

        // Obslugujemy wszystkie zdarzenia GLFW
        glfwPollEvents();
    }

    // Usuwamy wszystkie stworzone obiekty
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    // Usuwamy okno przed zakonczeniem programu
    glfwDestroyWindow(window);

    // Zwalniamy zasoby GLFW przed zakonczeniem programu
    glfwTerminate();
    return 0;
}