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
    glm::vec3 position;
    float orbitRadius;
    float orbitSpeed;
    float rotationSpeed;
    float scale;
    float orbitAngle;
    float rotationAngle;
    unsigned int textureID;
    std::vector<Planet *> moons;

    Planet(float radius, float orbSpeed, float rotSpeed, float size, const char *texturePath);
    ~Planet();

    void update(float deltaTime);
    void render(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection);
    void adjustRotationSpeed(float amount);
    void adjustOrbitSpeed(float amount);
    glm::mat4 getModelMatrix();
    void addMoon(Planet *moon);
    void updateMoons(float deltaTime);
    void renderMoons(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection);

private:
    void loadTexture(const char *texturePath);
};

#endif