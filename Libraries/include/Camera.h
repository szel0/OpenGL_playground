#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "shaderClass.h"

class Camera{
    public:
        vec3 Position;
        vec3 Orientation = {0.0f, 0.0f, -1.0f};
        vec3 Up = {0.0f, 1.0f, 0.0f};


        int width;
        int height;

        float speed = 0.001f;
        float sens = 100.0f;

        Camera(int width, int height, vec3 position);

        void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* unifrom);

        void Inputs(GLFWwindow* window);
};

#endif