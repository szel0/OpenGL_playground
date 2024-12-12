#define _USE_MATH_DEFINES

#include "Camera.h"

Camera::Camera(int width, int height, vec3 position){
    Camera::width = width;
    Camera::height = height;
    Position[0] = position[0];
    Position[1] = position[1];
    Position[2] = position[2];
    mat4x4_identity(crntView);
    mat4x4_identity(crntProj);
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane){
    mat4x4 view, proj, pv;
    mat4x4_identity(view);
    mat4x4_identity(proj);

    vec3 Center;
    vec3_add(Center, Position, Orientation);

    mat4x4_look_at(view, Position, Center, Up);
    mat4x4_perspective(proj, FOVdeg * M_PI / 180.0f, float(width/height), nearPlane, farPlane);

    mat4x4_dup(crntView, view);
    mat4x4_dup(crntProj, proj);
}

void Camera::Matrix(Shader& shader){
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, (const GLfloat*) crntView);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj"), 1, GL_FALSE, (const GLfloat*) crntProj);
}


void Camera::Inputs(GLFWwindow* window, float speed){
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        vec3 temp;
        vec3_scale(temp, Orientation, speed);
        vec3_add(Position, Position, temp);
        std::cout << "Position: (" << Position[0] << ", " << Position[1] << ", " << Position[2] << ")\n";
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        vec3 temp;
        vec3_scale(temp, Orientation, -speed);
        vec3_add(Position, Position, temp);
        std::cout << "Position: (" << Position[0] << ", " << Position[1] << ", " << Position[2] << ")\n";
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        vec3 temp;
        vec3_mul_cross(temp, Orientation, Up);
        vec3_norm(temp, temp);
        vec3_scale(temp, temp, -speed);
        vec3_add(Position, Position, temp);
        std::cout << "Position: (" << Position[0] << ", " << Position[1] << ", " << Position[2] << ")\n";
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        vec3 temp;
        vec3_mul_cross(temp, Orientation, Up);
        vec3_norm(temp, temp);
        vec3_scale(temp, temp, speed);
        vec3_add(Position, Position, temp);
        std::cout << "Position: (" << Position[0] << ", " << Position[1] << ", " << Position[2] << ")\n";
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        vec3 temp;
        vec3_scale(temp, Up, speed);
        vec3_add(Position, Position, temp);
        std::cout << "Position: (" << Position[0] << ", " << Position[1] << ", " << Position[2] << ")\n";
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        vec3 temp;
        vec3_scale(temp, Up, -speed);
        vec3_add(Position, Position, temp);
        std::cout << "Position: (" << Position[0] << ", " << Position[1] << ", " << Position[2] << ")\n";
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick){
            glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
        }

        double mouseX, mouseY;

        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sens * (float)(mouseY - (height / 2)) / height;
		float rotY = sens * (float)(mouseX - (width / 2)) / width;

        vec3 newOrientation, temp;
        vec3_mul_cross(temp, Orientation, Up);
        vec3_norm(temp, temp);

        vec3_rotate(newOrientation, Orientation, -rotX * M_PI / 180.0f, temp);

        float angle = vec3_angle(newOrientation, Up);

        if (fabs(angle - M_PI_2) <= 85.0f * M_PI / 180.0f){
            Orientation[0] = newOrientation[0];
            Orientation[1] = newOrientation[1];
            Orientation[2] = newOrientation[2];
        }

        vec3_rotate(Orientation, Orientation, -rotY * M_PI / 180.0f, Up);


        glfwSetCursorPos(window, (width / 2), (height / 2));
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        // Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
    }
}