#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>

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

    // Tworzymy obiekt vertexShader i uzyskujemy jego referencję
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Dolaczamy kod zrodlowy vertexShadera do obiektu
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // Kompilujemy vertexShader do kodu maszynowego
    glCompileShader(vertexShader);

    // Tworzymy obiekt fragmentShader i uzyskujemy jego referencję
    GLuint fragmentShader = glCreateShader(GL_VERTEX_SHADER);
    // Dolaczamy kod zrodlowy fragmentShadera do obiektu
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // Kompilujemy fragmentShader do kodu maszynowego
    glCompileShader(fragmentShader);

    // Tworzymy obiekt shaderProgram i uzyskujemy jego referencję
    GLuint shaderProgram = glCreateProgram();
    
    // Dolaczamy vertexShader i fragmentShader do shaderProgram
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // Laczymy wszystkie shadery w shaderProgram
    glLinkProgram(shaderProgram);

    // Usuwamy niepotrzebne juz obiekty
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Koordynaty wierzcholkow
    GLfloat vertices[] = {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Lewy dolny
         0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Prawy gorny
         0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // Gorny
        -0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f, // Wewnetrzny lewy
         0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f, // Wewnetrzny prawy
         0.0f / 2, -0.5f * float(sqrt(3)) / 3, 0.0f // Wewnetrzny dolny        
    };

    GLuint indices[] = {
        0, 3, 5, // Lewy dolny trojkat
        3, 2, 4, // Prawy dolny trojkat
        5, 4, 1  // Gorny trojkat 
    };

    // Tworzymy kontenery referencyjne dla Vertex Array Object i Vertex Buffer Object
    GLuint VAO, VBO, EBO;    

    // Generujemy VAO i VBO, po jednym obiekcie
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Ustawiamy VAO jako bierzacy obiekt VertexArray
    glBindVertexArray(VAO);

    // Wiazemy VBO, okreslajac, ze jest to GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Wprowadzamy wierzcholki do VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Konfigurujemy atrybuty wierzcholkow, aby OpenGL wiedzial, jak odczytywac VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Wlaczamy atrybuty wierzcholkow, aby OpenGL wiedzial, ze ma ich uzywac
    glEnableVertexAttribArray(0);

    // Wiazemy VBO i VAO z 0, aby przypadkowo nie modyfikowac kontenerow
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Glowna petla while
    while(!glfwWindowShouldClose(window)){
        // Okreslamy kolor tla
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Czyscimy bufor tylni i przypisujemy mu nowy kolor
        glClear(GL_COLOR_BUFFER_BIT);
        // Przekazujemy OpenGL, ktorego programu shaderow chcemy uzyc
        glUseProgram(shaderProgram);
        // Wiazemy VAO, aby OpenGL wiedzial, ze ma go uzywac
        glBindVertexArray(VAO);

        // Rysujemy trojkat
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // Zmieniamy bufor tylni z przednim
        glfwSwapBuffers(window);

        // Obslugujemy wszystkie zdarzenia GLFW
        glfwPollEvents();
    }

    // Usuwamy wszystkie stworzone obiekty
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Usuwamy okno przed zakonczeniem programu
    glfwDestroyWindow(window);

    // Zwalniamy zasoby GLFW przed zakonczeniem programu
    glfwTerminate();
    return 0;
}