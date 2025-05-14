#ifndef MODERN_SPHERE_H
#define MODERN_SPHERE_H

#include <glad/glad.h>
#include <vector>
#include "Sphere.h"

class ModernSphere
{
public:
    ModernSphere(const Sphere &sphere);
    ~ModernSphere();

    void draw() const;

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};

#endif