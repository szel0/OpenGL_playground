#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <array>
#include <windows.h>
#include <unordered_map>

#include "Mesh.h"

using namespace std;

struct KeyFrame {
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

KeyFrame torsoFrames[] = {
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};

KeyFrame limbFrames[] = {
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.0f, 0.0f, 0.0f}, {0.7f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    {{0.0f, 0.0f, 0.0f}, {-0.7f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};

void vec3_interpolate(vec3& out, const vec3& start, const vec3& end, float t) {
    out[0] = start[0] + t * (end[0] - start[0]);
    out[1] = start[1] + t * (end[1] - start[1]);
    out[2] = start[2] + t * (end[2] - start[2]);
}

void vec3_interpolate_scale(mat4x4& transform, const vec3& start, const vec3& end, float t) {
    vec3 scale;
    vec3_interpolate(scale, start, end, t);
    mat4x4_scale_aniso(transform, transform, scale[0], scale[1], scale[2]);
}

void mat4x4_set_rotation(mat4x4& transform, const mat4x4& rotationMatrix) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            transform[i][j] = rotationMatrix[i][j];
        }
    }
}

void mat4x4_set_scale(mat4x4& transform, const vec3& scale) {
    // Ustaw skalę na diagonalnych elementach
    transform[0][0] = scale[0]; // Skala X
    transform[1][1] = scale[1]; // Skala Y
    transform[2][2] = scale[2]; // Skala Z

    // Upewniamy się, że inne wartości nie zmieniają transformacji
    transform[0][1] = transform[0][2] = transform[0][3] = 0.0f; // Zerowanie reszty w wierszu 1
    transform[1][0] = transform[1][2] = transform[1][3] = 0.0f; // Zerowanie reszty w wierszu 2
    transform[2][0] = transform[2][1] = transform[2][3] = 0.0f; // Zerowanie reszty w wierszu 3

    // Wartości dla ostatniego wiersza i kolumny w macierzy
    transform[3][0] = transform[3][1] = transform[3][2] = 0.0f; // Zerowanie przesunięcia
    transform[3][3] = 1.0f; // Współczynnik jednorodności
}

class SceneNode {
    public:
        Mesh* mesh;
        mat4x4 localTransform;
        vector<SceneNode*> children;
        vector<KeyFrame> keyFrames;
        float currentTime;
        float speedFactor;
        array<float, 3> hipOffset;

        SceneNode(Mesh* mesh = nullptr, array<float, 3> hipOffset = {0.0f, 0.0f, 0.0f}): mesh(mesh), speedFactor(0.4f), hipOffset(hipOffset) {
            mat4x4_identity(localTransform);
            currentTime = 0.0f;
        }

        void addKeyFrame(const KeyFrame& keyFrame) {
            keyFrames.push_back(keyFrame);
        }

        void updateAnimation(float deltaTime) {
            if (keyFrames.empty()) return;

            // Aktualizacja indeksu klatki
            currentTime += deltaTime * speedFactor;
            if (currentTime >= 1.0f) {
                currentTime = 0.0f;  // Reset do pierwszej klatki po przejściu przez wszystkie
            }

            // Oblicz indeksy poprzedniej i następnej klatki
            size_t prevIndex = static_cast<size_t>(currentTime * (keyFrames.size() - 1));
            size_t nextIndex = (prevIndex + 1) % keyFrames.size();

            KeyFrame* prev = &keyFrames[prevIndex];
            KeyFrame* next = &keyFrames[nextIndex];

            // Interpolacja między poprzednią a następną klatką
            float t = currentTime * (keyFrames.size() - 1) - prevIndex;

            // Interpolacja pozycji, rotacji i skali
            vec3 interpolatedPosition, interpolatedRotation, interpolatedScale;
            vec3_interpolate(interpolatedPosition, prev->position, next->position, t);
            vec3_interpolate(interpolatedRotation, prev->rotation, next->rotation, t);
            vec3_interpolate(interpolatedScale, prev->scale, next->scale, t);



            // Kumulatywna transformacja
            mat4x4 tempTransform;
            mat4x4_identity(tempTransform);

            mat4x4_translate(tempTransform, interpolatedPosition[0], interpolatedPosition[1], interpolatedPosition[2]);


            mat4x4_rotate_X(tempTransform, tempTransform, interpolatedRotation[0]);
            mat4x4_rotate_Y(tempTransform, tempTransform, interpolatedRotation[1]);
            mat4x4_rotate_Z(tempTransform, tempTransform, interpolatedRotation[2]);

            mat4x4_scale_aniso(tempTransform, tempTransform, interpolatedScale[0], interpolatedScale[1], interpolatedScale[2]);

            mat4x4_dup(localTransform, tempTransform);

            for (SceneNode* child : children) {
                child->updateAnimation(deltaTime);
            }
        }

        void addChild(SceneNode* child) {
            children.push_back(child);
        }

        void draw(Shader& shader, Camera& camera, mat4x4 parentTransform) {
            mat4x4 globalTransform;
            mat4x4_mul(globalTransform, parentTransform, localTransform);

            if (mesh) {
                shader.Activate();
                glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, (const GLfloat*)globalTransform);
                mesh->Draw(shader, camera);
            }

            for (SceneNode* child: children) {
                child->draw(shader, camera, globalTransform);
            }
        }
};

const unsigned int width = 1600;
const unsigned int height = 800;
vec3 lightPos = {0.0f, 13.0f, 0.0f};

bool ifCamera = true;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);   
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        ifCamera = !ifCamera;
}

string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    filesystem::path exePath(buffer);
    return exePath.parent_path().string();
}
string basePath = getExecutablePath();

void calculateNormal(vec3& triangleNormal, vector <array<float, 3>> tempPositions, unsigned int vIdx[3]) {

    vector <vec3> triangle(3);
    triangle[0][0] = tempPositions[vIdx[0] - 1][0];
    triangle[0][1] = tempPositions[vIdx[0] - 1][1];
    triangle[0][2] = tempPositions[vIdx[0] - 1][2];

    triangle[1][0] = tempPositions[vIdx[1] - 1][0];
    triangle[1][1] = tempPositions[vIdx[1] - 1][1];
    triangle[1][2] = tempPositions[vIdx[1] - 1][2];

    triangle[2][0] = tempPositions[vIdx[2] - 1][0];
    triangle[2][1] = tempPositions[vIdx[2] - 1][1];
    triangle[2][2] = tempPositions[vIdx[2] - 1][2];



    vec3 edge1, edge2;
    vec3_sub(edge1, triangle[1], triangle[0]);
    vec3_sub(edge2, triangle[2], triangle[0]);

    vec3_mul_cross(triangleNormal, edge1, edge2);
}

struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Ns;
    string map_Kd;
};

void loadMaterial(const string& materialFile, Material& material) {
    string materialPath = basePath + "\\Resource Files\\Materials\\" + materialFile;
    ifstream file(materialPath);
    if (!file.is_open()){
        cerr << "Failed to open *.mtl file " << materialPath << endl;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "Ka") {
            iss >> material.Ka[0] >> material.Ka[1] >> material.Ka[2];
            // cout << material.Ka[0] << " " << material.Ka[1] << " " << material.Ka[2] << endl;
        } else if (prefix == "Kd") {
            iss >> material.Kd[0] >> material.Kd[1] >> material.Kd[2];
            // cout << material.Kd[0] << " " << material.Kd[1] << " " << material.Kd[2] << endl;
        } else if (prefix == "Ks") {
            iss >> material.Ks[0] >> material.Ks[1] >> material.Ks[2];
            // cout << material.Ks[0] << " " << material.Ks[1] << " " << material.Ks[2] << endl;
        } else if (prefix == "Ns") {
            iss >> material.Ns;
            // cout << material.Ns << endl;
        } else if (prefix == "map_Kd") {
            iss >> material.map_Kd;
            // cout << material.map_Kd << endl;
        }
    }

    file.close();

    cout << "zaladowano " << materialPath<< endl;
}

bool loadObjFile(const string& path, vector <Vertex>& vertices, vector <GLuint>& indices, string& material) {
    vector <array<float, 3>> tempPositions;
    vector <array<float, 2>> tempTexCoords;
    vector <array<float, 3>> tempNormals; 
    vector <GLuint> tempIndices;
    unordered_map <string, GLuint> uniqueVertexMap;

    bool isShaded = false;

    string line;

    ifstream file(path);

    if (!file.is_open()){
        cerr << "Failed to open *.obj file " << path << endl;
        return false;
    }

    while (getline(file, line)) {
        istringstream  ss(line);
        string lineHeader;
        ss >> lineHeader;
        if (lineHeader == "mtllib") {
            ss >> material;
        }
        if (lineHeader == "v") {
            array <float, 3> position;
            ss >> position[0] >> position[1] >> position[2];
            tempPositions.push_back(position);
        } else if (lineHeader == "vt") {
            array <float, 2> texCoord;
            ss >> texCoord[0] >> texCoord[1];
            tempTexCoords.push_back(texCoord);          
        } else if (lineHeader == "vn") {
            array <float, 3> normal;
            ss >> normal[0] >> normal[1] >> normal[2];
            tempNormals.push_back(normal); 
            isShaded = true;         
        } else if (lineHeader == "f") {
            if (isShaded) {
                unsigned int vIdx[3], tIdx[3], nIdx[3];
                char slash;
                ss >> vIdx[0] >> slash >> tIdx[0] >> slash >> nIdx[0]
                >> vIdx[1] >> slash >> tIdx[1] >> slash >> nIdx[1]
                >> vIdx[2] >> slash >> tIdx[2] >> slash >> nIdx[2];
                
                for (int i = 0; i < 3; ++ i) {
                    string key = to_string(vIdx[i]) + "/" + to_string(tIdx[i]) + "/" + to_string(nIdx[i]);
                    
                    if (uniqueVertexMap.find(key) != uniqueVertexMap.end()) {
                        indices.push_back(uniqueVertexMap[key]);
                    } else {
                        Vertex vertex;
                        vertex.position[0] = tempPositions[vIdx[i] - 1][0];
                        vertex.position[1] = tempPositions[vIdx[i] - 1][1];
                        vertex.position[2] = tempPositions[vIdx[i] - 1][2];
                        vertex.normal[0] = tempNormals[nIdx[i] - 1][0];
                        vertex.normal[1] = tempNormals[nIdx[i] - 1][1];
                        vertex.normal[2] = tempNormals[nIdx[i] - 1][2];
                        vertex.color[0] = 1.0f;
                        vertex.color[1] = 0.0f;
                        vertex.color[2] = 0.0f;
                        vertex.texUV[0] = tempTexCoords[tIdx[i] - 1][0];
                        vertex.texUV[1] = tempTexCoords[tIdx[i] - 1][1];
                        
                        vertices.push_back(vertex);
                        indices.push_back(vertices.size() - 1);
                        uniqueVertexMap[key] = vertices.size() - 1;
                    }
                }
            } else {
                unsigned int vIdx[3];
                ss >> vIdx[0] >> vIdx[1] >> vIdx[2];

                vec3 triangleNormal = {1.0f, 1.0f, 1.0f};
                calculateNormal(triangleNormal, tempPositions, vIdx);

                for (int i = 0; i < 3; ++ i) {
                    Vertex vertex;
                    vertex.position[0] = tempPositions[vIdx[i] - 1][0];
                    vertex.position[1] = tempPositions[vIdx[i] - 1][1];
                    vertex.position[2] = tempPositions[vIdx[i] - 1][2];
                    vertex.normal[0] = triangleNormal[0];
                    vertex.normal[1] = triangleNormal[1];
                    vertex.normal[2] = triangleNormal[2];
                    vertex.color[0] = 1.0f;
                    vertex.color[1] = 0.0f;
                    vertex.color[2] = 0.0f;
                    vertex.texUV[0] = 1.0f;
                    vertex.texUV[1] = 1.0f;

                    vertices.push_back(vertex);
                    indices.push_back(vertices.size() - 1);
                }
            }
        }
    }

    file.close();
    vertices.resize(vertices.size() + 1);
    return true;
}

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
    Shader shaderProgram((basePath + "\\Resource Files\\Shaders\\default.vert").c_str(), 
                         (basePath + "\\Resource Files\\Shaders\\default.frag").c_str());

    Texture texDefault((basePath + "\\Resource Files\\Textures\\red.png").c_str(), GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);

    vector <Mesh> objMeshes;
    vector <Mesh> lightMeshes;

    vector <Vertex> objVerts;
    vector <GLuint> objInds;
    string objMaterialFile = "12345";

    Mesh leftLeg, rightLeg, torso, rightArm, leftArm;
    Material objMaterial;

    loadObjFile(basePath + "\\Resource Files\\Objects\\LeftLeg.obj", objVerts, objInds, objMaterialFile);

    loadMaterial(objMaterialFile, objMaterial);
    cout << objMaterial.Ka[0] << " " << objMaterial.Ka[1] << " " << objMaterial.Ka[2] << endl; 
    cout << objMaterial.Kd[0] << " " << objMaterial.Kd[1] << " " << objMaterial.Kd[2] << endl; 
    cout << objMaterial.Ks[0] << " " << objMaterial.Ks[1] << " " << objMaterial.Ks[2] << endl; 
    cout << objMaterial.Ns <<  endl; 
    cout << objMaterial.map_Kd <<  endl;
    Texture tex((basePath + "\\Resource Files\\Textures\\" + objMaterial.map_Kd).c_str(), GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
    leftLeg = Mesh(objVerts, objInds, tex);

    objVerts.clear();
    objInds.clear();
    loadObjFile(basePath + "\\Resource Files\\Objects\\RightLeg.obj", objVerts, objInds, objMaterialFile);
    rightLeg = Mesh(objVerts, objInds, tex);

    objVerts.clear();
    objInds.clear();
    loadObjFile(basePath + "\\Resource Files\\Objects\\Torso.obj", objVerts, objInds, objMaterialFile);
    torso = Mesh(objVerts, objInds, tex);
    
    objVerts.clear();
    objInds.clear();
    loadObjFile(basePath + "\\Resource Files\\Objects\\RightArm.obj", objVerts, objInds, objMaterialFile);
    rightArm = Mesh(objVerts, objInds, tex);
    
    objVerts.clear();
    objInds.clear();
    loadObjFile(basePath + "\\Resource Files\\Objects\\LeftArm.obj", objVerts, objInds, objMaterialFile);
    leftArm = Mesh(objVerts, objInds, tex);  

    SceneNode torsoNode(&torso, {0.0f, 0.0f, 0.0f});
    SceneNode leftLegNode(&leftLeg, {0.0f, 4.0f, 0.0f});
    SceneNode rightLegNode(&rightLeg, {0.0f, 0.0f, 0.0f});
    SceneNode leftArmNode(&leftArm, {0.0f, 0.0f, 0.0f});
    SceneNode rightArmNode(&rightArm, {0.0f, 0.0f, 0.0f});

    torsoNode.addChild(&leftLegNode);
    torsoNode.addChild(&rightLegNode);
    torsoNode.addChild(&leftArmNode);
    torsoNode.addChild(&rightArmNode);


    torsoNode.addKeyFrame(torsoFrames[0]);
    torsoNode.addKeyFrame(torsoFrames[1]);
    torsoNode.addKeyFrame(torsoFrames[0]);
    torsoNode.addKeyFrame(torsoFrames[2]);
    torsoNode.addKeyFrame(torsoFrames[0]);

    leftLegNode.addKeyFrame(limbFrames[0]);    
    leftLegNode.addKeyFrame(limbFrames[1]); 
    leftLegNode.addKeyFrame(limbFrames[0]); 
    leftLegNode.addKeyFrame(limbFrames[2]); 
    leftLegNode.addKeyFrame(limbFrames[0]);
    leftLegNode.addKeyFrame(limbFrames[1]); 
    leftLegNode.addKeyFrame(limbFrames[0]); 
    leftLegNode.addKeyFrame(limbFrames[2]); 
    leftLegNode.addKeyFrame(limbFrames[0]);
    leftLegNode.addKeyFrame(limbFrames[1]); 
    leftLegNode.addKeyFrame(limbFrames[0]); 
    leftLegNode.addKeyFrame(limbFrames[2]); 
    leftLegNode.addKeyFrame(limbFrames[0]);
    leftLegNode.addKeyFrame(limbFrames[1]); 
    leftLegNode.addKeyFrame(limbFrames[0]); 
    leftLegNode.addKeyFrame(limbFrames[2]); 
    leftLegNode.addKeyFrame(limbFrames[0]);

    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[2]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[1]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[2]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[1]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[2]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[1]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[2]);
    rightLegNode.addKeyFrame(limbFrames[0]);
    rightLegNode.addKeyFrame(limbFrames[1]);
    rightLegNode.addKeyFrame(limbFrames[0]);


    rightArmNode.addKeyFrame(limbFrames[0]);    
    rightArmNode.addKeyFrame(limbFrames[1]); 
    rightArmNode.addKeyFrame(limbFrames[0]); 
    rightArmNode.addKeyFrame(limbFrames[2]); 
    rightArmNode.addKeyFrame(limbFrames[0]);
    rightArmNode.addKeyFrame(limbFrames[1]); 
    rightArmNode.addKeyFrame(limbFrames[0]); 
    rightArmNode.addKeyFrame(limbFrames[2]); 
    rightArmNode.addKeyFrame(limbFrames[0]);
    rightArmNode.addKeyFrame(limbFrames[1]); 
    rightArmNode.addKeyFrame(limbFrames[0]); 
    rightArmNode.addKeyFrame(limbFrames[2]); 
    rightArmNode.addKeyFrame(limbFrames[0]);
    rightArmNode.addKeyFrame(limbFrames[1]); 
    rightArmNode.addKeyFrame(limbFrames[0]); 
    rightArmNode.addKeyFrame(limbFrames[2]); 
    rightArmNode.addKeyFrame(limbFrames[0]);


    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[2]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[1]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[2]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[1]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[2]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[1]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[2]);
    leftArmNode.addKeyFrame(limbFrames[0]);
    leftArmNode.addKeyFrame(limbFrames[1]);
    leftArmNode.addKeyFrame(limbFrames[0]);


    shaderProgram.Activate();
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "Ka"), objMaterial.Ka[0], objMaterial.Ka[1], objMaterial.Ka[2]);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "Kd"), objMaterial.Kd[0], objMaterial.Kd[1], objMaterial.Kd[2]);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "Ks"), objMaterial.Ks[0], objMaterial.Ks[1], objMaterial.Ks[2]);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, "Ns"), objMaterial.Ns);

    // Tworzymy light shader
    Shader lightShader((basePath + "\\Resource Files\\Shaders\\light.vert").c_str(), 
                         (basePath + "\\Resource Files\\Shaders\\light.frag").c_str());

	vector <Vertex> lVerts;
	vector <GLuint> lInds;
    string lightMaterialFile = "12345";
    loadObjFile(basePath + "\\Resource Files\\Objects\\light.obj", lVerts, lInds, lightMaterialFile);

    // cout << objMaterialFile << " " << lightMaterialFile << endl;

	Mesh light(lVerts, lInds, texDefault);


    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};

    lightShader.Activate();
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);



    vec3 modelPos = {1.0f, 1.0f, 0.0f};
    mat4x4 model;
    mat4x4_identity(model);
    mat4x4_translate(model, modelPos[0], modelPos[1], modelPos[2]);

    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, (const GLfloat*)model);




    // Włączamy testowanie głębokości
    glEnable(GL_DEPTH_TEST);

    vec3 camPosition = {-14.0f, 11.0f, 2.0f};
    Camera camera(width, height, camPosition);

    float FOV = 45.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    int counter = 0;

    // Pętla renderująca
    while (!glfwWindowShouldClose(window)) {
        float crntFrame = glfwGetTime();
        counter = 0;
        deltaTime = crntFrame - lastFrame;
        lastFrame = crntFrame;

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

        float speed = 2.0f * deltaTime;
        float fovSpeed = 20.0f * deltaTime;

        // Wektor pomocniczy do ruchu po plaszcznie X kamery
        vec3 right;
        vec3_mul_cross(right, camera.Orientation, camera.Up);
        vec3_norm(right, right);
        if (ifCamera){
            // Zmiana FOV
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
                if (FOV - 0.01f >= 10.0f) FOV -= fovSpeed;
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) 
                if (FOV + 0.01f <= 120.0f) FOV += fovSpeed;

            // Zmiana kamery
            camera.Inputs(window, speed);
            camera.updateMatrix(FOV, 0.1f, 100.0f);
            camera.Matrix(shaderProgram);
        } else { 
            // Zmiana pozycji światła
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, camera.Orientation, speed); // Ruch do przodu
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, camera.Orientation, -speed); // Ruch do tyłu
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, right, -speed); // Ruch w lewo
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                vec3 temp;
                vec3_scale(temp, right, speed); // Ruch w prawo
                vec3_add(lightPos, lightPos, temp);
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                lightPos[1] += speed;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                lightPos[1] -= speed;  
        }

        light.Draw(lightShader, camera);

        // Aktualizacja animacji z deltaTime
        torsoNode.updateAnimation(deltaTime);
        

        // Rysowanie węzła korzenia z transformacją globalną
        mat4x4 identityMatrix;
        mat4x4_identity(identityMatrix);  // Macierz jednostkowa dla korzenia

        torsoNode.draw(shaderProgram, camera, identityMatrix);

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