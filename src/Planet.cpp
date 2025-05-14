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
    position = glm::vec3(radius, 0.0f, 0.0f);

    loadTexture(texturePath);
}

Planet::~Planet()
{
    glDeleteTextures(1, &textureID);

    for (auto moon : moons)
    {
        delete moon;
    }
}

void Planet::update(float deltaTime)
{
    orbitAngle += orbitSpeed * deltaTime;
    if (orbitAngle > 360.0f)
        orbitAngle -= 360.0f;

    rotationAngle += rotationSpeed * deltaTime;
    if (rotationAngle > 360.0f)
        rotationAngle -= 360.0f;

    position.x = orbitRadius * cos(glm::radians(orbitAngle));
    position.z = orbitRadius * sin(glm::radians(orbitAngle));

    updateMoons(deltaTime);
}

void Planet::render(Shader &shader, ModernSphere &sphere, glm::mat4 view, glm::mat4 projection)
{
    shader.use();

    shader.setMat4("model", getModelMatrix());
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.setInt("texture1", 0);

    sphere.draw();

    renderMoons(shader, sphere, view, projection);
}

glm::mat4 Planet::getModelMatrix()
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

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
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

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
        moon->rotationAngle += moon->rotationSpeed * deltaTime;
        if (moon->rotationAngle > 360.0f)
            moon->rotationAngle -= 360.0f;

        moon->orbitAngle += moon->orbitSpeed * deltaTime;
        if (moon->orbitAngle > 360.0f)
            moon->orbitAngle -= 360.0f;

        glm::vec3 moonRelativePos;
        moonRelativePos.x = moon->orbitRadius * cos(glm::radians(moon->orbitAngle));
        moonRelativePos.z = moon->orbitRadius * sin(glm::radians(moon->orbitAngle));

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