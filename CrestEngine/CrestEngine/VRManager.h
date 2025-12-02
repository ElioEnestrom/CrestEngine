#pragma once

#if defined(_WIN32)
#define XR_USE_PLATFORM_WIN32
#endif

#include <GraphicsAPI_OpenGL.h>
#include <openxr/openxr.h>           // Core OpenXR types and functions
#include <openxr/openxr_platform.h>  // Platform-specific extensions (Windows + OpenGL)

#include <glad/gl.h>                // OpenGL function loader
#include <GLFW/glfw3.h>               // Your existing window library
#include <glm/glm.hpp>                // Your existing math library

typedef XrResult(XRAPI_PTR* PFN_xrGetOpenGLGraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLKHR* graphicsRequirements);


class VRManager {
public:
    bool Initialize(GLFWwindow* window);
    void Shutdown();

    bool IsActive() const { return m_instance != XR_NULL_HANDLE; }

    
private:
    XrInstance m_instance = XR_NULL_HANDLE;
    XrSystemId m_systemId = XR_NULL_SYSTEM_ID;
    XrSession m_session = XR_NULL_HANDLE;


    std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;


    PFN_xrGetOpenGLGraphicsRequirementsKHR GetOpenGLGraphicsRequirementsFunc = nullptr;
};