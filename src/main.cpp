#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

using namespace std;

// Kod zrodlowy vertex shader'a
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
// Kod zrodlowy fragment shader'a
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";



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
    gladLoadGL(((GLADloadfunc)glfwGetProcAddress));

    // Okreslamy obszar widoku OpenGL'a w oknie
    // W tym przypadku (x, y) = ([0, 800], [0, 800])
    glViewport(0, 0, 800, 800);

    // Okreslamy kolor tla
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    // Czyscimy bufor tla i przypisujemy nowy kolor
    glClear(GL_COLOR_BUFFER_BIT);

    // Zamieniamy bufor tla z buforem poprzednim
    glfwSwapBuffers(window);

    
    // Glowna petla while
    while(!glfwWindowShouldClose(window)){
        // Obslugujemy wszystkie zdarzenia GLFW
        glfwPollEvents();
    }

    // Usuwamy okno przed zakonczeniem programu
    glfwDestroyWindow(window);

    // Zwalniamy zasoby GLFW przed zakonczeniem programu
    glfwTerminate();
    return 0;
}