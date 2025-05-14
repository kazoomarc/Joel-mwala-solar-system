#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Planet.h"
#include "Sphere.h"
#include "ModernSphere.h"
#include "Timer.h"
#include <iostream>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 0.0f, 160.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Timer timer;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Planet *sun;
vector<Planet *> planets;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader planetShader("shaders/planet.vs", "shaders/planet.fs");
    Shader sunShader("shaders/sun.vs", "shaders/sun.fs");

    Sphere sphereModel(1.0f, 36, 18, true);
    ModernSphere modernSphere(sphereModel);

    // Create the sun
    sun = new Planet(0.0f, 0.0f, 10.0f, 8.0f, "textures/sunmap.jpg");

    // creating 9 planets
    // Mercury
    Planet *mercury = new Planet(15.0f, 47.0f, 20.0f, 0.8f, "textures/mercurymap.jpg");
    planets.push_back(mercury);

    // Venus
    Planet *venus = new Planet(22.0f, 35.0f, 15.0f, 1.5f, "textures/venusmap.jpg");
    planets.push_back(venus);

    // Earth with Moon
    Planet *earth = new Planet(30.0f, 30.0f, 25.0f, 1.6f, "textures/earthmap1k.jpg");
    Planet *moon = new Planet(3.0f, 80.0f, 15.0f, 0.4f, "textures/moonmap1k.jpg");
    earth->addMoon(moon);
    planets.push_back(earth);

    // Mars
    Planet *mars = new Planet(40.0f, 24.0f, 20.0f, 1.2f, "textures/marsmap1k.jpg");
    planets.push_back(mars);

    // Jupiter
    Planet *jupiter = new Planet(55.0f, 13.0f, 12.0f, 4.0f, "textures/jupitermap.jpg");
    planets.push_back(jupiter);

    // Saturn
    Planet *saturn = new Planet(70.0f, 9.0f, 10.0f, 3.5f, "textures/saturnmap.png");
    planets.push_back(saturn);

    // Uranus
    Planet *uranus = new Planet(85.0f, 6.0f, 8.0f, 2.5f, "textures/uranusmap.png");
    planets.push_back(uranus);

    // Neptune
    Planet *neptune = new Planet(100.0f, 5.0f, 7.0f, 2.4f, "textures/neptunemap.jpg");
    planets.push_back(neptune);

    // Pluto
    Planet *pluto = new Planet(115.0f, 4.0f, 5.0f, 0.6f, "textures/plutomap.png");
    planets.push_back(pluto);

    timer.start();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = timer.getElapsedTimeInSec();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        sunShader.use();
        sunShader.setMat4("projection", projection);
        sunShader.setMat4("view", view);

        sun->update(deltaTime);
        sun->render(sunShader, modernSphere, view, projection);

        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);

        planetShader.setVec3("lightDir", glm::vec3(0.0f, -1.0f, 0.0f));
        planetShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.8f));

        planetShader.setVec3("pointLightPos", glm::vec3(0.0f, 10.0f, 0.0f));
        planetShader.setVec3("pointLightColor", glm::vec3(0.0f, 0.0f, 1.0f));

        planetShader.setVec3("viewPos", camera.Position);

        for (auto planet : planets)
        {
            planet->update(deltaTime);
            planet->render(planetShader, modernSphere, view, projection);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete sun;
    for (auto planet : planets)
    {
        delete planet;
    }

    glfwTerminate();
    return 0;
}

// Process all input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        sun->adjustRotationSpeed(0.4f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        sun->adjustRotationSpeed(-0.4f);

    Planet *earth = planets[2];
    if (earth && !earth->moons.empty())
    {
        Planet *moon = earth->moons[0];
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            moon->adjustOrbitSpeed(0.4f);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            moon->adjustOrbitSpeed(-0.4f);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}