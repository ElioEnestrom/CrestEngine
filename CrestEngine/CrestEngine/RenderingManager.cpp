#include "RenderingManager.h"


void RenderingManager::Initialize(unsigned int screenWidth, unsigned int screenHeight,
                                   ShaderResources* shaders,
                                   const std::unordered_map<std::string, Mesh*>* meshes,
                                   const std::unordered_map<unsigned int, unsigned int>* vaos,
                                   TextureResources* textures)
{
    this->shaderResources = shaders;
    this->meshMap = meshes;
    this->VAOs = vaos;
    this->textureResources = textures;
    
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    
    // Setup shadow mapping framebuffer
    SetupShadowMapping(shadowWidth, shadowHeight);
    
    // Configure shader texture units
    shaderResources->ourShader.use();
    shaderResources->ourShader.setInt("diffuseTexture", 1);
    shaderResources->ourShader.setInt("shadowMap", 0);
    shaderResources->ourShader.setInt("material.specular", 3);
    shaderResources->ourShader.setInt("texture2", 2);
    glUniform1i(glGetUniformLocation(shaderResources->ourShader.ID, "texture1"), 1);
}

void RenderingManager::SetupShadowMapping(unsigned int width, unsigned int height)
{
    shadowWidth = width;
    shadowHeight = height;
    
    // Create framebuffer
    glGenFramebuffers(1, &depthMapFBO);
    
    // Create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Attach depth texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderingManager::RenderShadowPass(
    EntityManager& entityManager,
    const RenderResources& renderResources,
    const TextureResources& textureResources,
    Shader& simpleDepthShader,
    unsigned int depthMapFBO,
    const glm::mat4& lightSpaceMatrix)
{
    unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    // Verify the framebuffer is still valid, regenerate if needed
    if (!glIsFramebuffer(depthMapFBO)) {
        std::cerr << "WARNING: Shadow framebuffer was deleted, regenerating..." << std::endl;
        SetupShadowMapping(shadowWidth, shadowHeight);
        depthMapFBO = this->depthMapFBO;
    }

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    for (Entity* entity : entityManager.entities)
    {
        if (entity != nullptr)
        {
            glm::mat4 model = entity->CreateTransformMatrix(
                entity->entityPosition,
                entity->rotation,
                entity->scale);
            simpleDepthShader.setMat4("model", model);

            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, renderingManager.GetDepthMap());
            //glActiveTexture(GL_TEXTURE1);
            //glBindTexture(GL_TEXTURE_2D, textureResources.textureIDs[entity->textureIndex1]);
            //glActiveTexture(GL_TEXTURE2);
            //glBindTexture(GL_TEXTURE_2D, textureResources.textureIDs[entity->textureIndex2]);

            glBindVertexArray(renderResources.VAOs.at(renderResources.meshMap.at(entity->model)->id));
            glDrawArrays(GL_TRIANGLES, 0, renderResources.meshMap.at(entity->model)->vertices.size());
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}