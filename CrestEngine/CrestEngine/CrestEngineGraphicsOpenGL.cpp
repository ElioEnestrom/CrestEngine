// DON'T include pch.h - it conflicts with OpenGL defines
// #include "pch.h"

// Windows headers - Need to include COM support for OpenXR headers
#define NOMINMAX
#include <windows.h>
#include <unknwn.h>  // For IUnknown (needed by OpenXR platform headers)

// Define OpenGL graphics API BEFORE including OpenXR headers
#ifndef XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_GRAPHICS_API_OPENGL
#endif

#ifndef XR_USE_PLATFORM_WIN32
#define XR_USE_PLATFORM_WIN32
#endif

// Include OpenXR with OpenGL support
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include "OpenXrProgramOpenGL.h"

// Need to include OpenXrProgram.h AFTER OpenXrProgramOpenGL.h to get proper types
// But we need the xr::math namespace, so let's define what we need
#include "xrMath.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>

// Forward declare sample::Cube
namespace sample {
    struct Cube {
        XrVector3f Scale;
        XrPosef PoseInAppSpace;
    };
}

namespace {
    
    // Helper to convert XrFovf to glm::mat4 projection matrix
    glm::mat4 CreateProjectionMatrix(const XrFovf& fov, float nearZ, float farZ) {
        const float tanLeft = tanf(fov.angleLeft);
        const float tanRight = tanf(fov.angleRight);
        const float tanDown = tanf(fov.angleDown);
        const float tanUp = tanf(fov.angleUp);

        const float tanWidth = tanRight - tanLeft;
        const float tanHeight = tanUp - tanDown;

        glm::mat4 projection(0.0f);
        projection[0][0] = 2.0f / tanWidth;
        projection[1][1] = 2.0f / tanHeight;
        projection[2][0] = (tanRight + tanLeft) / tanWidth;
        projection[2][1] = (tanUp + tanDown) / tanHeight;
        projection[2][2] = -(farZ + nearZ) / (farZ - nearZ);
        projection[2][3] = -1.0f;
        projection[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);

        return projection;
    }

    // Helper to convert XrPosef to glm::mat4
    glm::mat4 XrPoseToGlmMat4(const XrPosef& pose) {
        const XrQuaternionf& q = pose.orientation;
        const XrVector3f& p = pose.position;

        glm::mat4 rotation = glm::mat4_cast(glm::quat(q.w, q.x, q.y, q.z));
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
        
        return translation * rotation;
    }

    struct CrestEngineGraphicsOpenGL : sample::IGraphicsPluginOpenGL {
        
        CrestEngineGraphicsOpenGL(void* glfwWindow) 
            : m_glfwWindow(static_cast<GLFWwindow*>(glfwWindow)) {
            
            if (!m_glfwWindow) {
                throw std::runtime_error("Invalid GLFW window provided to OpenGL graphics plugin");
            }
            
            // Initialize the graphics binding structure
            m_graphicsBinding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR;
            m_graphicsBinding.next = nullptr;
            m_graphicsBinding.hDC = nullptr;
            m_graphicsBinding.hGLRC = nullptr;
        }

        void InitializeResources(void* instance, uint64_t systemId) override {
            // Cast to proper types
            XrInstance xrInstance = static_cast<XrInstance>(instance);
            XrSystemId xrSystemId = static_cast<XrSystemId>(systemId);
            
            // Set up the graphics binding for OpenXR
            m_graphicsBinding.hDC = GetDC(glfwGetWin32Window(m_glfwWindow));
            m_graphicsBinding.hGLRC = glfwGetWGLContext(m_glfwWindow);

            // Create framebuffer for VR rendering
            glGenFramebuffers(1, &m_fbo);
            
            std::cout << "OpenGL Graphics Plugin initialized for VR" << std::endl;
            std::cout << "  GL Version: " << glGetString(GL_VERSION) << std::endl;
            std::cout << "  GL Renderer: " << glGetString(GL_RENDERER) << std::endl;
        }

        const void* GetGraphicsBinding() const override {
            return &m_graphicsBinding;
        }

        const std::vector<int64_t>& SupportedColorFormats() const override {
            static std::vector<int64_t> formats = {
                GL_RGBA8,
                GL_SRGB8_ALPHA8,
                GL_RGBA16F,
            };
            return formats;
        }

        const std::vector<int64_t>& SupportedDepthFormats() const override {
            static std::vector<int64_t> formats = {
                GL_DEPTH_COMPONENT32F,
                GL_DEPTH_COMPONENT24,
                GL_DEPTH24_STENCIL8,
                GL_DEPTH32F_STENCIL8,
            };
            return formats;
        }

        void RenderView(const void* imageRect,
                       const float renderTargetClearColor[4],
                       const void* viewProjections,
                       size_t viewProjectionCount,
                       uint32_t colorTexture,
                       uint32_t depthTexture,
                       const std::vector<const sample::Cube*>& cubes) override {
            
            // Cast imageRect to proper type
            const XrRect2Di& rect = *static_cast<const XrRect2Di*>(imageRect);
            
            // Cast viewProjections back to proper type
            const auto* vpArray = static_cast<const xr::math::ViewProjection*>(viewProjections);
            
            // Make sure the GLFW context is current
            glfwMakeContextCurrent(m_glfwWindow);

            // Save previous framebuffer
            GLint previousFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);

            // Bind our VR framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            
            // Attach VR swapchain textures
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

            // Check framebuffer completeness
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "VR Framebuffer incomplete: " << status << std::endl;
                glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
                return;
            }

            // Set viewport
            glViewport(rect.offset.x, rect.offset.y, 
                      rect.extent.width, rect.extent.height);

            // Clear buffers
            glClearColor(renderTargetClearColor[0], renderTargetClearColor[1], 
                        renderTargetClearColor[2], renderTargetClearColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Enable depth testing
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            // Render scene for each eye (stereo rendering)
            for (size_t viewIndex = 0; viewIndex < viewProjectionCount; ++viewIndex) {
                const auto& vp = vpArray[viewIndex];
                
                // Create view matrix (inverse of the camera pose)
                glm::mat4 viewMatrix = glm::inverse(XrPoseToGlmMat4(vp.Pose));
                
                // Create projection matrix from FOV
                glm::mat4 projectionMatrix = CreateProjectionMatrix(
                    vp.Fov, vp.NearFar.Near, vp.NearFar.Far);

                // For stereo rendering with texture arrays, you'd typically render to different
                // array layers. For simplicity, we'll render both eyes to the same texture
                // (you may want to use a texture array and layer rendering for proper stereo)
                
                RenderScene(viewMatrix, projectionMatrix, cubes);
            }

            // Restore previous framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
        }

    private:
        void RenderScene(const glm::mat4& view, const glm::mat4& projection, 
                        const std::vector<const sample::Cube*>& cubes) {
            
            // TODO: Integrate with your existing rendering system
            // This is where you'd call your MeshManager, EntityManager, etc.
            
            // Example: Render cubes using your existing engine
            // EntityManager& entityManager = EntityManager::Get();
            // for (auto& entity : entityManager.GetEntities()) {
            //     RenderEntity(entity, view, projection);
            // }

            // For now, just render the simple cubes provided by OpenXR sample
            RenderSimpleCubes(view, projection, cubes);
        }

        void RenderSimpleCubes(const glm::mat4& view, const glm::mat4& projection,
                              const std::vector<const sample::Cube*>& cubes) {
            
            // TODO: Use your Shader class to render cubes
            // For now, this is a placeholder showing the structure
            
            for (const sample::Cube* cube : cubes) {
                // Create model matrix from cube pose and scale
                glm::mat4 model = XrPoseToGlmMat4(cube->PoseInAppSpace);
                model = glm::scale(model, glm::vec3(cube->Scale.x, cube->Scale.y, cube->Scale.z));
                
                glm::mat4 mvp = projection * view * model;
                
                // Here you would:
                // 1. Bind your shader
                // 2. Set uniforms (mvp matrix, etc.)
                // 3. Draw cube geometry
                //
                // Example:
                // yourShader.use();
                // yourShader.setMat4("mvp", mvp);
                // RenderCubeMesh();
            }
        }

        GLFWwindow* m_glfwWindow = nullptr;
        XrGraphicsBindingOpenGLWin32KHR m_graphicsBinding;
        GLuint m_fbo = 0;  // Framebuffer for VR rendering
    };

} // namespace

namespace sample {
    std::unique_ptr<IGraphicsPluginOpenGL> CreateCrestEngineGraphicsOpenGL(void* glfwWindow) {
        return std::make_unique<CrestEngineGraphicsOpenGL>(glfwWindow);
    }
}