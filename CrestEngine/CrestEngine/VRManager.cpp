#include "VRManager.h"
#include <iostream>
#include <cstring>

bool VRManager::Initialize(GLFWwindow* window) {
    // STEP 2a: Fill out application information
    // This tells the VR runtime (SteamVR/Oculus/etc) about your app
    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    strcpy_s(createInfo.applicationInfo.applicationName, "CrestEngine");
    createInfo.applicationInfo.applicationVersion = 1;
    createInfo.applicationInfo.engineName[0] = '\0';
    createInfo.applicationInfo.engineVersion = 1;
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    
    // STEP 2b: Request the OpenGL extension
    // Without this, OpenXR won't let you use OpenGL (it could use DirectX/Vulkan instead)
    const char* extensions[] = { XR_KHR_OPENGL_ENABLE_EXTENSION_NAME };
    createInfo.enabledExtensionCount = 1;
    createInfo.enabledExtensionNames = extensions;
    
    // STEP 2c: Create the instance
    // This connects to the VR runtime running on your PC
    XrResult result = xrCreateInstance(&createInfo, &m_instance);
    if (XR_FAILED(result)) {
        std::cerr << "xrCreateInstance failed: " << result << std::endl;
        return false;
    }
    
    std::cout << "OpenXR instance created!" << std::endl;
    
    // 3a: Ask for a head-mounted display
    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    
    // 3b: Get the system
    result = xrGetSystem(m_instance, &systemInfo, &m_systemId);
    if (XR_FAILED(result)) {
        std::cerr << "xrGetSystem failed: " << result << std::endl;
        return false;
    }
    
    std::cout << "VR system found!" << std::endl;
    
    
    m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>(m_instance, m_systemId);
    // 4a: Get OpenGL context handles (Windows-specific)
#ifdef _WIN32
    XrGraphicsBindingOpenGLWin32KHR graphicsBinding{XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
    graphicsBinding.hDC = wglGetCurrentDC();
    graphicsBinding.hGLRC = wglGetCurrentContext();
#endif
    
    
    // 4b: Create session with OpenGL binding
    XrSessionCreateInfo sessionInfo{XR_TYPE_SESSION_CREATE_INFO};
	sessionInfo.next = m_graphicsAPI->GetGraphicsBinding();
    sessionInfo.systemId = m_systemId;
    
    result = xrCreateSession(m_instance, &sessionInfo, &m_session);
    if (XR_FAILED(result)) {
        std::cerr << "xrCreateSession failed: " << result << std::endl;
        return false;
    }
    
    std::cout << "VR session created!" << std::endl;
    return true;
}

void VRManager::Shutdown() {
    if (m_instance != XR_NULL_HANDLE) {
        xrDestroyInstance(m_instance);
    }
}