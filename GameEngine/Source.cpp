#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "cLoadModels.h"
#include "FlyCam.h"
#include "cShaderCompiler.h"
#include "cVAOManager.h"
#include "cScene.h"
#include "IPlayer.h"
#include "cPlayer.h"
#include "cAiEnemy.h"
#include "cLightManager.h"
#include "cLightMover.h"
#include "cPhysicsUpdated.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version (3.3 core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Refer cShaderCompiler class for more info.
    cShaderCompiler shader;
    GLuint shaderProgram = shader.CompileShader();

    // Import Scene
    cScene scene;
    scene.CreateScene("../Question1/sceneFileWithNormals.txt");
    //scene.ExportMaterial(shaderProgram, scene.numberOfMeshesToLoad);            // Considering number of materials = number of meshes to load

    GLuint VAO, VBO;
    cVAOManager VAOManager;
    VAO = VAOManager.BindVAOVBO(VBO, scene.numberOfMeshesToLoad, scene.pModels);
    
    int numberOfVerticesToRenderForAllModels = 0;
    for (int index = 0; index != scene.pModels.size(); index++) {
        numberOfVerticesToRenderForAllModels += scene.pModels[index].numberOfVerticesToRender;
    }

    cLightManager lightManager;
    lightManager.LoadLights("../Question2/lightsFile.txt");     // 1.0  0.000000000000001  0.0000000000000001  10000000000000000.0  best attenuation for now
    // Camera Initialization
    FlyCam flyCam(1280, 720, glm::vec3(18029.0f, 63312.0f, -7641.0f), 180.0f);
    flyCam.camSpeed = 10.0f;
    flyCam.camRoll = -260.0f;
    flyCam.camYaw = 165.0f;
    flyCam.camPitch = -66.0f;

    cLightMover lightMover(lightManager, flyCam, 14);
    lightMover.lightSpeed = 1000.0f;

    float deltaTime = 0;
    float startTime, endTime;

    for (int modelIndex = 0; modelIndex != scene.numberOfMeshesToLoad; modelIndex++) {
        scene.pModels[modelIndex].pTransformedVertices = new cLoadModels::sTransformedVertex[scene.pModels[modelIndex].numberOfVertices];
    }   // Used for initializing the pTransformedVertices, Nothing new xD

    bool increasing = true;
    bool is7Pressed = false;
    bool is8Pressed = false;
    bool isKeyComboActivated = false;
    bool is90Pressed = false;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {

        // Input handling
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glfwGetWindowSize(window, &flyCam.camWidth, &flyCam.camHeight);

        std::string cameraPositionInTitle = "Camera Location: (" + to_string(flyCam.camLocation.x) + ", " + to_string(flyCam.camLocation.y) + ", " + to_string(flyCam.camLocation.z) + ") Camera Speed: " + to_string(flyCam.camSpeed) + " Camera Roll: " + to_string(flyCam.camRoll) + " Cam yaw: " + to_string(flyCam.camYaw) + " Cam pitch: " + to_string(flyCam.camPitch);

        startTime = glfwGetTime();
        
        glfwSetWindowTitle(window, cameraPositionInTitle.c_str());
        //std::cout << deltaTime << " Time passed" << std::endl;

        
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(19113.0f, -66651.0f, -6177.0f);
            flyCam.camRoll = -280.0f;
            flyCam.camYaw = 161.0f;
            flyCam.camPitch = 58.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(18029.0f, 63312.0f, -7641.0f);
            flyCam.camRoll = -260.0f;
            flyCam.camYaw = 165.0f;
            flyCam.camPitch = -66.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(365890.0f, 122300.0f, -13637.0f);
            flyCam.camRoll = 491.0f;
            flyCam.camYaw = -200.0f;
            flyCam.camPitch = -40.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            is7Pressed = true;
        }

        if (is7Pressed) {
            for (int i = 4; i <= 8; i++) {
                scene.pModels[i].pMeshTransform.xRotation += 0.001 * static_cast<float>(rand()) * deltaTime;
                scene.pModels[i].pMeshTransform.yRotation += 0.001 * static_cast<float>(rand()) * deltaTime;
                scene.pModels[i].pMeshTransform.zRotation += 0.001 * static_cast<float>(rand()) * deltaTime;
            }
        }

        if (is7Pressed) {
            int i = 4;
            while (true) {
                scene.pModels[i].pMeshTransform.x += 0.01 * static_cast<float>(rand()) * deltaTime;
                scene.pModels[i].pMeshTransform.y += 0.01 * static_cast<float>(rand()) * deltaTime;
                scene.pModels[i].pMeshTransform.z += 0.01 * static_cast<float>(rand()) * deltaTime;
                i++;
                if (i > 8) {
                    i = 4;
                    break;
                }
            }
        }

        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
            is8Pressed = true;
        }

        if (is8Pressed) {
            for (int i = 11; i <= 15; i++) {
                scene.pModels[i].pMeshTransform.x -= 1000 * deltaTime;
            }
            for (int j = 9; j <= 13; j++) {
                lightManager.lights[j].position.x -= 1000 * deltaTime;
            }
        }


        //flyCam.camControls(window);
        flyCam.cameraMatrix(45.0f, 0.1f, 100000000.0f, shaderProgram, "camMatrix", window);

        // Rendering commands here
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);   // Enable depth testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader program and draw the triangle
        glUseProgram(shaderProgram);

        float& directionY = lightManager.lights[8].direction.y;

        if (increasing) {
            directionY += 1.0;

            if (directionY >= 5.0) {
                directionY = 5.0;
                increasing = false;
            }
        }
        else {
            directionY -= 1.0;
            if (directionY <= -5.0) {
                directionY = -5.0;
                increasing = true;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(364976.0f, 121510.0f, -13057.0f);
            flyCam.camRoll = 613.0f;
            flyCam.camYaw = 11.60f;
            flyCam.camPitch = 19.96f;
        }

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            flyCam.camLocation = glm::vec3(361568.0f, 119389.0f, -12312.0f);
            flyCam.camRoll = 452.0f;
            flyCam.camYaw = -199.0f;
            flyCam.camPitch = -31.0f;
        }

        // ------------------------------------------------------------------------------------------------------------------------------
        // You can call movement functions from light mover class for lights here now and then call turn on lights function of light manager
        lightManager.TurnOnLights(shaderProgram, 14);
        // ------------------------------------------------------------------------------------------------------------------------------
        // You can create player objects here and make them move from here
        // ------------------------------------------------------------------------------------------------------------------------------

        glBindVertexArray(VAO);
        shader.SetSceneView(window);        // Press 1, 2 or 3 for different viewModes like wireframe, fill or point

        int offset = 0;
        for (int index = 0; index != scene.pModels.size(); index++) {

            glm::mat4 model = scene.pModels[index].CreateModelMatrix(shaderProgram, scene.pModels[index]);      // Creation of model matrix with arguements passed in sceneFile.txt
            scene.pModels[index].GenerateTransformedVertices(model);            // this is here because collisions want transformed vertices

            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, offset, scene.pModels[index].numberOfVerticesToRender);
            offset += scene.pModels[index].numberOfVerticesToRender;
        }

        endTime = glfwGetTime();
        deltaTime = endTime - startTime;
        //physicsEngine.CheckCollision(scene);

        // Swap buffers and poll IO events (keys pressed/released, mouse moved, etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // Cleanup
    VAOManager.VAOVBOCleanup();
    
    // Terminate GLFW
    glfwTerminate();
    return 0;
}