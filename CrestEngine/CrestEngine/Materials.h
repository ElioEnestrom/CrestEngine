#pragma once
#ifndef MATERIALS_H
#define MATERIALS_H

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};
// Define constant materials
const Material BRONZE = {
    glm::vec3(0.2125f, 0.1275f, 0.054f),  // Ambient
    glm::vec3(0.714f, 0.4284f, 0.18144f), // Diffuse
    glm::vec3(0.393548f, 0.271906f, 0.166721f), // Specular
    25.6f // Shininess
};

const Material WOOD = {
    glm::vec3(0.2f, 0.1f, 0.05f),  // Ambient
    glm::vec3(0.4f, 0.2f, 0.1f),   // Diffuse
    glm::vec3(0.04f, 0.02f, 0.01f), // Specular
    10.0f // Shininess
};


// Materials from the website
const Material EMERALD = {
    glm::vec3(0.0215f, 0.1745f, 0.0215f),  // Ambient
    glm::vec3(0.07568f, 0.61424f, 0.07568f), // Diffuse
    glm::vec3(0.633f, 0.727811f, 0.633f), // Specular
    76.8f // Shininess
};

const Material JADE = {
    glm::vec3(0.135f, 0.2225f, 0.1575f),  // Ambient
    glm::vec3(0.54f, 0.89f, 0.63f),       // Diffuse
    glm::vec3(0.316228f, 0.316228f, 0.316228f), // Specular
    12.8f // Shininess
};

const Material OBSIDIAN = {
    glm::vec3(0.05375f, 0.05f, 0.06625f),  // Ambient
    glm::vec3(0.18275f, 0.17f, 0.22525f),  // Diffuse
    glm::vec3(0.332741f, 0.328634f, 0.346435f), // Specular
    38.4f // Shininess
};

const Material PEARL = {
    glm::vec3(0.25f, 0.20725f, 0.20725f),  // Ambient
    glm::vec3(1.0f, 0.829f, 0.829f),       // Diffuse
    glm::vec3(0.296648f, 0.296648f, 0.296648f), // Specular
    11.264f // Shininess
};

const Material RUBY = {
    glm::vec3(0.1745f, 0.01175f, 0.01175f),  // Ambient
    glm::vec3(0.61424f, 0.04136f, 0.04136f), // Diffuse
    glm::vec3(0.727811f, 0.626959f, 0.626959f), // Specular
    76.8f // Shininess
};

const Material TURQUOISE = {
    glm::vec3(0.1f, 0.18725f, 0.1745f),  // Ambient
    glm::vec3(0.396f, 0.74151f, 0.69102f), // Diffuse
    glm::vec3(0.297254f, 0.30829f, 0.306678f), // Specular
    12.8f // Shininess
};

const Material GOLD = {
    glm::vec3(0.24725f, 0.1995f, 0.0745f),  // Ambient
    glm::vec3(0.75164f, 0.60648f, 0.22648f), // Diffuse
    glm::vec3(0.628281f, 0.555802f, 0.366065f), // Specular
    51.2f // Shininess
};

const Material SILVER = {
    glm::vec3(0.19225f, 0.19225f, 0.19225f),  // Ambient
    glm::vec3(0.50754f, 0.50754f, 0.50754f),  // Diffuse
    glm::vec3(0.508273f, 0.508273f, 0.508273f), // Specular
    51.2f // Shininess
};

const Material CHROME = {
    glm::vec3(0.25f, 0.25f, 0.25f),  // Ambient
    glm::vec3(0.4f, 0.4f, 0.4f),     // Diffuse
    glm::vec3(0.774597f, 0.774597f, 0.774597f), // Specular
    76.8f // Shininess
};

const Material COPPER = {
    glm::vec3(0.19125f, 0.0735f, 0.0225f),  // Ambient
    glm::vec3(0.7038f, 0.27048f, 0.0828f),  // Diffuse
    glm::vec3(0.256777f, 0.137622f, 0.086014f), // Specular
    12.8f // Shininess
};
// Add more materials as needed...

#endif