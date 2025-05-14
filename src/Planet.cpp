#include "Planet.h"
#include <stb_image.h>
#include <iostream>

Planet::Planet(float radius, float orbSpeed, float rotSpeed, float size, const char *texturePath)
{
    orbitRadius = radius;
    orbitSpeed = orbSpeed;
    rotationSpeed = rotSpeed;
    scale = size;
    orbitAngle = 0.0f;
    rotationAngle = 0.0f;
    position = glm::vec3(radius, 0.0f, 0.0f); // Start at x = radius, y = 0, z = 0

    // Load texture
    loadTexture(texturePath);
}

Planet::~Planet()
{
    glDeleteTextures(1, &textureID);

    // Delete moons
    for (auto moon : moons)
    {
        delete moon;
    }
}

void Planet::update(float deltaTime)
{
    // Update orbit angle
    orbitAngle += orbitSpeed * deltaTime;
    if (orbitAngle > 360.0f)
        orbitAngle -= 360.0f;

    // Update rotation angle
    rotationAngle += rotationSpeed * deltaTime;
    if (rotationAngle > 360.0f)
        rotationAngle -= 360.0f;

    // Update position based on orbit angle
    position.x = orbitRadius * cos(glm::radians(orbitAngle));
    position.z = orbitRadius * sin(glm::radians(orbitAngle));

    // Update moons
    updateMoons(deltaTime);
}

void Planet::render(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection)
{
    // Activate shader
    shader.use();

    // Set matrices
    shader.setMat4("model", getModelMatrix());
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.setInt("texture1", 0);

    // Draw sphere
    sphere.draw();

    // Render moons
    renderMoons(shader, sphere, view, projection);
}

glm::mat4 Planet::getModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);

    // Translate to orbit position
    model = glm::translate(model, position);

    // Rotate around Y axis
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    // Scale
    model = glm::scale(model, glm::vec3(scale));

    return model;
}

void Planet::adjustRotationSpeed(float amount)
{
    rotationSpeed += amount;
    if (rotationSpeed < 0.0f)
        rotationSpeed = 0.0f;
}

void Planet::adjustOrbitSpeed(float amount)
{
    orbitSpeed += amount;
    if (orbitSpeed < 0.0f)
        orbitSpeed = 0.0f;
}

void Planet::loadTexture(const char *texturePath)
{
    // Generate texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip textures vertically

    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // std::cout << "Loaded texture: " << texturePath << " (" << width << "x" << height << ")" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
    }

    stbi_image_free(data);
}

void Planet::addMoon(Planet *moon)
{
    moons.push_back(moon);
}

void Planet::updateMoons(float deltaTime)
{
    for (auto moon : moons)
    {
        // Update moon's own rotation
        moon->rotationAngle += moon->rotationSpeed * deltaTime;
        if (moon->rotationAngle > 360.0f)
            moon->rotationAngle -= 360.0f;

        // Update moon's orbit around this planet
        moon->orbitAngle += moon->orbitSpeed * deltaTime;
        if (moon->orbitAngle > 360.0f)
            moon->orbitAngle -= 360.0f;

        // Calculate moon position relative to this planet
        glm::vec3 moonRelativePos;
        moonRelativePos.x = moon->orbitRadius * cos(glm::radians(moon->orbitAngle));
        moonRelativePos.z = moon->orbitRadius * sin(glm::radians(moon->orbitAngle));

        // Set moon position to be relative to this planet
        moon->position = position + moonRelativePos;
    }
}

void Planet::renderMoons(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection)
{
    for (auto moon : moons)
    {
        moon->render(shader, sphere, view, projection);
    }
}