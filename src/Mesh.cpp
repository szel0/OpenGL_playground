#include "Mesh.h"

Mesh::Mesh() {

}

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, Texture& texture) 
    : vertices(vertices), indices(indices), texture(texture) {
        VAO.Bind();

        VBO VBO(vertices);
        EBO EBO(indices);

        VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
        VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
        VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
        VAO.LinkAttrib(VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

        VAO.Unbind();
        VBO.Unbind();
        EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    VAO.Bind();

    texture.texUnit(shader, "tex0", GL_TEXTURE0);
    texture.Bind();

    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position[0], camera.Position[1], camera.Position[2]);
    camera.Matrix(shader);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}