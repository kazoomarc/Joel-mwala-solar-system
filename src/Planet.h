#ifndef PLANET_H
#define PLANET_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "ModernSphere.h"

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
    float rotationAngle;
    unsigned int textureID;
    std::vector<Planet *> moons;

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

    // Add a moon to this planet
    void addMoon(Planet *moon);

    // Update moons
    void updateMoons(float deltaTime);

    // Render moons
    void renderMoons(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection);

private:
    // Load texture
    void loadTexture(const char *texturePath);
};

#endif