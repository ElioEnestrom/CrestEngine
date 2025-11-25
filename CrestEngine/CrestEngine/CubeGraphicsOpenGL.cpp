// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "OpenXrProgram.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
    namespace CubeShader {
        struct Vertex {
            XrVector3f Position;
            XrVector3f Color;
        };

        constexpr XrVector3f Red{1, 0, 0};
        constexpr XrVector3f DarkRed{0.25f, 0, 0};
        constexpr XrVector3f Green{0, 1, 0};
        constexpr XrVector3f DarkGreen{0, 0.25f, 0};
        constexpr XrVector3f Blue{0, 0, 1};
        constexpr XrVector3f DarkBlue{0, 0, 0.25f};

        // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
        constexpr XrVector3f LBB{-0.5f, -0.5f, -0.5f};
        constexpr XrVector3f LBF{-0.5f, -0.5f, 0.5f};
        constexpr XrVector3f LTB{-0.5f, 0.5f, -0.5f};
        constexpr XrVector3f LTF{-0.5f, 0.5f, 0.5f};
        constexpr XrVector3f RBB{0.5f, -0.5f, -0.5f};
        constexpr XrVector3f RBF{0.5f, -0.5f, 0.5f};
        constexpr XrVector3f RTB{0.5f, 0.5f, -0.5f};
        constexpr XrVector3f RTF{0.5f, 0.5f, 0.5f};

#define CUBE_SIDE(V1, V2, V3, V4, V5, V6, COLOR) {V1, COLOR}, {V2, COLOR}, {V3, COLOR}, {V4, COLOR}, {V5, COLOR}, {V6, COLOR},

        constexpr Vertex c_cubeVertices[] = {
              CUBE_SIDE(LTB, LBF, LBB, LTB, LTF, LBF, DarkRed)   // -X
              CUBE_SIDE(RTB, RBB, RBF, RTB, RBF, RTF, Red)       // +X
              CUBE_SIDE(LBB, LBF, RBF, LBB, RBF, RBB, DarkGreen) // -Y
              CUBE_SIDE(LTB, RTB, RTF, LTB, RTF, LTF, Green)  // +Y
              CUBE_SIDE(LBB, RBB, RTB, LBB, RTB, LTB, DarkBlue)  // -Z
              CUBE_SIDE(LBF, LTF, RTF, LBF, RTF, RBF, Blue)   // +Z
        };

        // Winding order is clockwise. Each side uses a different color.
        constexpr unsigned short c_cubeIndices[] = {
            0,  1,  2,  3,  4,  5,  // -X
            6,  7,  8,  9,  10, 11, // +X
            12, 13, 14, 15, 16, 17, // -Y
            18, 19, 20, 21, 22, 23, // +Y
            24, 25, 26, 27, 28, 29, // -Z
            30, 31, 32, 33, 34, 35, // +Z
        };

        constexpr uint32_t MaxViewInstance = 2;

        // Vertex shader
        const char* VertexShaderGLSL = R"_(
            #version 330 core
            layout (location = 0) in vec3 aPos;
                layout (location = 1) in vec3 aColor;

                uniform mat4 ModelViewProjection;
                uniform int ViewID;

                out vec3 Color;

            void main() {
                gl_Position = ModelViewProjection * vec4(aPos, 1.0);
                Color = aColor;
                gl_Layer = ViewID;
            }
)_";

        // Fragment shader
        const char* FragmentShaderGLSL = R"_(
          #version 330 core
                in vec3 Color;
                out vec4 FragColor;

                void main() {
                FragColor = vec4(Color, 1.0);
            }
        )_";
    } // namespace CubeShader

    struct CubeGraphicsOpenGL : sample::IGraphicsPluginOpenGL {
        
        void InitializeDevice(HDC hdc, HGLRC hglrc) override {
            m_hdc = hdc;
            m_hglrc = hglrc;
            
            InitializeGLResources();
        }

        void InitializeGLResources() {
            // Compile and link shaders
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &CubeShader::VertexShaderGLSL, nullptr);
            glCompileShader(vertexShader);
            CheckShaderCompilation(vertexShader, "Vertex");

            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &CubeShader::FragmentShaderGLSL, nullptr);
            glCompileShader(fragmentShader);
            CheckShaderCompilation(fragmentShader, "Fragment");

            m_shaderProgram = glCreateProgram();
            glAttachShader(m_shaderProgram, vertexShader);
            glAttachShader(m_shaderProgram, fragmentShader);
            glLinkProgram(m_shaderProgram);
            CheckProgramLinking(m_shaderProgram);

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            // Create VAO and VBO
            glGenVertexArrays(1, &m_vao);
            glGenBuffers(1, &m_vbo);
            glGenBuffers(1, &m_ebo);

            glBindVertexArray(m_vao);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(CubeShader::c_cubeVertices), CubeShader::c_cubeVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeShader::c_cubeIndices), CubeShader::c_cubeIndices, GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubeShader::Vertex), (void*)0);
            glEnableVertexAttribArray(0);

            // Color attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CubeShader::Vertex), (void*)offsetof(CubeShader::Vertex, Color));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);

            // Enable depth testing
            glEnable(GL_DEPTH_TEST);
        }

        void CheckShaderCompilation(GLuint shader, const char* type) {
            GLint success;
            GLchar infoLog[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success) 
            {
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);

                throw std::runtime_error(std::string(type) + " Shader compilation failed: " + infoLog);
                
            }
        }

        void CheckProgramLinking(GLuint program) {
            GLint success;
            GLchar infoLog[512];
            glGetProgramiv(program, GL_LINK_STATUS, &success);

            if (!success) 
            {
                glGetProgramInfoLog(program, 512, nullptr, infoLog);
                throw std::runtime_error(std::string("Shader program linking failed: ") + infoLog);
            }
            
        }

        XrGraphicsBindingOpenGLWin32KHR GetGraphicsBinding() const override 
        {
            XrGraphicsBindingOpenGLWin32KHR binding{};
            binding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR;
            binding.hDC = m_hdc;
            binding.hGLRC = m_hglrc;
            return binding;
        }

            const std::vector<GLenum>& SupportedColorFormats() const override {
            static const std::vector<GLenum> formats = {GL_RGBA8, GL_SRGB8_ALPHA8,};
              return formats;
            }

        const std::vector<GLenum>& SupportedDepthFormats() const override 
        {
        static const std::vector<GLenum> formats = {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT24, GL_DEPTH24_STENCIL8, GL_DEPTH32F_STENCIL8,};
           return formats;    
        }

        void RenderView(
            const XrRect2Di& imageRect,
            const float renderTargetClearColor[4],
            const std::vector<xr::math::ViewProjection>& viewProjections,
            GLenum colorSwapchainFormat,
            uint32_t colorTexture,
            GLenum depthSwapchainFormat,
            uint32_t depthTexture,
            const std::vector<const sample::Cube*>& cubes) override 
        {const uint32_t viewInstanceCount = (uint32_t)viewProjections.size();
        CHECK_MSG(viewInstanceCount <= CubeShader::MaxViewInstance,
            "Sample shader supports 2 or fewer view instances. Adjust shader to accommodate more.")

            // Bind framebuffer
            if (m_fbo == 0) {
                glGenFramebuffers(1, &m_fbo);
            }
        
            glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

            // Set viewport
            glViewport(imageRect.offset.x, imageRect.offset.y, imageRect.extent.width, imageRect.extent.height);

            // Clear
            const bool reversedZ = viewProjections[0].NearFar.Near > viewProjections[0].NearFar.Far;
            const float depthClearValue = reversedZ ? 0.0f : 1.0f;
 
            glClearColor(renderTargetClearColor[0], renderTargetClearColor[1], renderTargetClearColor[2], renderTargetClearColor[3]);
            glClearDepth(depthClearValue);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (reversedZ) {
                glDepthFunc(GL_GREATER);
            } 
            else 
            {
                glDepthFunc(GL_LESS);
            }

            // Use shader program
            glUseProgram(m_shaderProgram);
            glBindVertexArray(m_vao);

            // Render for each view
            for (uint32_t viewIndex = 0; viewIndex < viewInstanceCount; viewIndex++) {

                const auto& vp = viewProjections[viewIndex];
               
                // Compute view matrix (inverse of pose)

                glm::mat4 view = glm::inverse(glm::mat4(
                    1.0f - 2.0f * (vp.Pose.orientation.y * vp.Pose.orientation.y + vp.Pose.orientation.z * vp.Pose.orientation.z),
                    2.0f * (vp.Pose.orientation.x * vp.Pose.orientation.y + vp.Pose.orientation.w * vp.Pose.orientation.z),
                    2.0f * (vp.Pose.orientation.x * vp.Pose.orientation.z - vp.Pose.orientation.w * vp.Pose.orientation.y),
                    0.0f,
                    2.0f * (vp.Pose.orientation.x * vp.Pose.orientation.y - vp.Pose.orientation.w * vp.Pose.orientation.z),
                    1.0f - 2.0f * (vp.Pose.orientation.x * vp.Pose.orientation.x + vp.Pose.orientation.z * vp.Pose.orientation.z),
                    2.0f * (vp.Pose.orientation.y * vp.Pose.orientation.z + vp.Pose.orientation.w * vp.Pose.orientation.x),
                    0.0f,
          
                    2.0f * (vp.Pose.orientation.x * vp.Pose.orientation.z + vp.Pose.orientation.w * vp.Pose.orientation.y),
                    2.0f * (vp.Pose.orientation.y * vp.Pose.orientation.z - vp.Pose.orientation.w * vp.Pose.orientation.x),
                    1.0f - 2.0f * (vp.Pose.orientation.x * vp.Pose.orientation.x + vp.Pose.orientation.y * vp.Pose.orientation.y),
                    0.0f,
             
                    vp.Pose.position.x, vp.Pose.position.y, vp.Pose.position.z, 1.0f
                ));

            // Compute projection matrix
            glm::mat4 projection = ComposeProjectionMatrix(vp.Fov, vp.NearFar);

            GLint viewIDLocation = glGetUniformLocation(m_shaderProgram, "ViewID");
            glUniform1i(viewIDLocation, viewIndex);

            // Render each cube
            for (const sample::Cube* cube : cubes) {

                // Compute model matrix
                glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(cube->Scale.x, cube->Scale.y, cube->Scale.z));
                glm::mat4 model = XrPoseToGlmMat4(cube->PoseInAppSpace) * scale;
                glm::mat4 mvp = projection * view * model;

                GLint mvpLocation = glGetUniformLocation(m_shaderProgram, "ModelViewProjection");
                glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

                // Draw
                glDrawElements(GL_TRIANGLES, (GLsizei)std::size(CubeShader::c_cubeIndices), GL_UNSIGNED_SHORT, 0);
                }
            }

            glBindVertexArray(0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        ~CubeGraphicsOpenGL() 
        {
            if (m_vao) glDeleteVertexArrays(1, &m_vao);
            if (m_vbo) glDeleteBuffers(1, &m_vbo);
            if (m_ebo) glDeleteBuffers(1, &m_ebo);
            if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
            if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
        }

private:
        glm::mat4 XrPoseToGlmMat4(const XrPosef& pose) {
            return glm::mat4(
                1.0f - 2.0f * (pose.orientation.y * pose.orientation.y + pose.orientation.z * pose.orientation.z),
                2.0f * (pose.orientation.x * pose.orientation.y + pose.orientation.w * pose.orientation.z),
                2.0f * (pose.orientation.x * pose.orientation.z - pose.orientation.w * pose.orientation.y),
                0.0f,
               
                2.0f * (pose.orientation.x * pose.orientation.y - pose.orientation.w * pose.orientation.z),
                1.0f - 2.0f * (pose.orientation.x * pose.orientation.x + pose.orientation.z * pose.orientation.z),
                2.0f * (pose.orientation.y * pose.orientation.z + pose.orientation.w * pose.orientation.x),
                0.0f,
        
                2.0f * (pose.orientation.x * pose.orientation.z + pose.orientation.w * pose.orientation.y),
                2.0f * (pose.orientation.y * pose.orientation.z - pose.orientation.w * pose.orientation.x),
                1.0f - 2.0f * (pose.orientation.x * pose.orientation.x + pose.orientation.y * pose.orientation.y),
                0.0f,
                
                pose.position.x, pose.position.y, pose.position.z, 1.0f
            );
        }

        glm::mat4 ComposeProjectionMatrix(const XrFovf& fov, const xr::math::NearFar& nearFar) 
        {
        
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
            projection[2][2] = -nearFar.Far / (nearFar.Far - nearFar.Near);
            projection[2][3] = -1.0f;
            projection[3][2] = -(nearFar.Far * nearFar.Near) / (nearFar.Far - nearFar.Near);

            return projection;
        }

        HDC m_hdc = nullptr;
        HGLRC m_hglrc = nullptr;
        GLuint m_shaderProgram = 0;
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_ebo = 0;
        GLuint m_fbo = 0;
    };
} // namespace

namespace sample {
    std::unique_ptr<sample::IGraphicsPluginOpenGL> CreateCubeGraphicsOpenGL() {
        return std::make_unique<CubeGraphicsOpenGL>();
    }
} // namespace sample
