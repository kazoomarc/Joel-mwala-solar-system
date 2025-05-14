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

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// Camera
Camera camera(glm::vec3(0.0f, 10.0f, 30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
Timer timer;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Planets
Planet *sun;
std::vector<Planet *> planets;
Planet *moon;
Planet *earth;

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Build and compile shaders
    Shader planetShader("shaders/planet.vs", "shaders/planet.fs");
    Shader sunShader("shaders/sun.vs", "shaders/sun.fs");

    // Create sphere model for planets
    Sphere sphereModel(1.0f, 36, 18, true);
    ModernSphere modernSphere(sphereModel);

    // Create planets
    // Parameters: orbit radius, orbit speed, rotation speed, scale, texture path
    sun = new Planet(0.0f, 0.0f, 10.0f, 5.0f, "textures/sunmap.jpg");

    // Create planets
    planets.push_back(new Planet(15.0f, 47.0f, 20.0f, 1.0f, "textures/mercurymap.jpg")); // Mercury
    planets.push_back(new Planet(22.0f, 35.0f, 15.0f, 1.8f, "textures/venusmap.jpg"));   // Venus
    earth = new Planet(30.0f, 30.0f, 25.0f, 2.0f, "textures/earthmap1k.jpg");            // Earth
    planets.push_back(earth);
    planets.push_back(new Planet(40.0f, 24.0f, 20.0f, 1.5f, "textures/marsmap1k.jpg")); // Mars

    // Create moon
    moon = new Planet(5.0f, 80.0f, 15.0f, 0.5f, "textures/moonmap1k.jpg");

    // Initialize timer
    timer.start();

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        float currentFrame = timer.getElapsedTimeInSec();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.05f, 1.0f); // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Render sun with sun shader
        sunShader.use();
        sunShader.setMat4("projection", projection);
        sunShader.setMat4("view", view);

        // Update and render sun
        sun->update(deltaTime);
        sun->render(sunShader, modernSphere, view, projection);

        // Render planets with planet shader
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);

        // Set lighting uniforms
        planetShader.setVec3("lightDir", glm::vec3(0.0f, -1.0f, 0.0f));       // Directional light from above
        planetShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.9f));      // Yellow light
        planetShader.setVec3("pointLightPos", glm::vec3(0.0f, 10.0f, 0.0f));  // Blue point light above sun
        planetShader.setVec3("pointLightColor", glm::vec3(0.0f, 0.0f, 1.0f)); // Blue color
        planetShader.setVec3("viewPos", camera.Position);

        // Update and render planets
        for (auto planet : planets)
        {
            planet->update(deltaTime);
            planet->render(planetShader, modernSphere, view, projection);
        }

        // Update moon position relative to Earth
        glm::vec3 earthPos = earth->position;
        float moonOrbitAngle = moon->orbitAngle;

        // Calculate moon position relative to Earth
        glm::vec3 moonRelativePos;
        moonRelativePos.x = moon->orbitRadius * cos(glm::radians(moonOrbitAngle));
        moonRelativePos.z = moon->orbitRadius * sin(glm::radians(moonOrbitAngle));

        // Set moon position to be relative to Earth
        moon->position = earthPos + moonRelativePos;

        // Update moon rotation
        moon->update(deltaTime);

        // Render moon
        moon->render(planetShader, modernSphere, view, projection);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    delete sun;
    for (auto planet : planets)
    {
        delete planet;
    }
    delete moon;

    // Terminate GLFW
    glfwTerminate();
    return 0;
}

// Process all input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Sun rotation speed control
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        sun->adjustRotationSpeed(0.1f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        sun->adjustRotationSpeed(-0.1f);

    // Moon orbit speed control
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        moon->adjustOrbitSpeed(0.1f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        moon->adjustOrbitSpeed(-0.1f);
}

// GLFW: whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// GLFW: whenever the mouse moves, this callback is called
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// GLFW: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}