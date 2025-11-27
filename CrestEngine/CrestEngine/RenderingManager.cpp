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
