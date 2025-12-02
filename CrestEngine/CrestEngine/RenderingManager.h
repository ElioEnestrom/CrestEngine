#pragma once
#include <glad/gl.h>
#include <unordered_map>
#include <string>
#include "RenderingTypes.h"
#include "Entities.h"

struct Mesh;
struct ShaderResources;
struct TextureResources;
class EntityManager;


class RenderingManager {
public:
    void Initialize(unsigned int screenWidth, unsigned int screenHeight,
                   ShaderResources* shaders,
                   const std::unordered_map<std::string, Mesh*>* meshes,
                   const std::unordered_map<unsigned int, unsigned int>* vaos,
                   TextureResources* textures);

    void RenderShadowPass(
        EntityManager& entityManager,
        const RenderResources& renderResources,
        const TextureResources& textureResources,
        Shader& simpleDepthShader,
        unsigned int depthMapFBO,
        const glm::mat4& lightSpaceMatrix);
    
    void RenderFrame(EntityManager& entityManager, bool drawCubes, bool viewNormals);
    void Cleanup();
    
    ShaderResources* shaderResources = nullptr;
    TextureResources* textureResources = nullptr;
    unsigned int GetDepthMap() const { return depthMap; }
    
private:
    void SetupShadowMapping(unsigned int width, unsigned int height);
    void RenderShadowPass(EntityManager& entityManager);
    void RenderMainPass(EntityManager& entityManager, bool viewNormals);
    
    unsigned int depthMapFBO = 0;
    unsigned int depthMap = 0;
    unsigned int shadowWidth = 1024;
    unsigned int shadowHeight = 1024;
    unsigned int screenWidth = 0;
    unsigned int screenHeight = 0;
    
    const std::unordered_map<std::string, Mesh*>* meshMap = nullptr;
    const std::unordered_map<unsigned int, unsigned int>* VAOs = nullptr;
};
