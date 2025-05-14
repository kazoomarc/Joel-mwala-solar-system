#ifndef PLANET_H
#define PLANET_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include "Shader.h"
#include "Sphere.h"
#include "ModernSphere.h" // Add this include

class Planet
{
public:
    // Planet properties
    glm::vec3 position;
    float orbitRadius;
    float orbitSpeed;
    float rotationSpeed;
    float scale;
    float orbitAngle;
    unsigned int textureID;

    // Constructor
    Planet(float radius, float orbSpeed, float rotSpeed, float size, const char *texturePath);

    // Destructor
    ~Planet();

    // Update planet position based on time
    void update(float deltaTime);

    // Render the planet
    void render(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection);

    // Adjust rotation speed
    void adjustRotationSpeed(float amount);

    // Adjust orbit speed
    void adjustOrbitSpeed(float amount);

    // Get model matrix
    glm::mat4 getModelMatrix();

private:
    float rotationAngle;

    // Load texture
    void loadTexture(const char *texturePath);
};

#endif