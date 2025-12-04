#pragma once

#if defined(_WIN32)
#define XR_USE_PLATFORM_WIN32
#endif

#include <GraphicsAPI_OpenGL.h>
#include <openxr/openxr.h>           // Core OpenXR types and functions
#include <openxr/openxr_platform.h>  // Platform-specific extensions (Windows + OpenGL)

#include "../Dependencies/Common/DebugOutput.h"
#include "../Dependencies/Common/OpenXRDebugUtils.h"

#include <glad/gl.h>                // OpenGL function loader
#include <GLFW/glfw3.h>               // Your existing window library
#include <glm/glm.hpp>                // Your existing math library

typedef XrResult(XRAPI_PTR* PFN_xrGetOpenGLGraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLKHR* graphicsRequirements);


class VRManager {
private: 
    struct RenderLayerInfo;

public:
    bool Initialize(GLFWwindow* window);
    void Shutdown();

    bool IsActive() const { return m_xrInstance != XR_NULL_HANDLE; }

	void PollEvents();
	void PollSystemEvents();

    void LocateEyes();

    void GetInstanceProperties();
    void DestroyInstance();

	void CreateDebugMessenger();
    void DestroyDebugMessenger();

	void CreateSession();
	void DestroySession();
    
	void CreateSwapchains();
	void DestroySwapchains();

    void GetViewConfigurationViews();

    void GetEnvironmentBlendModes();

    void CreateReferenceSpace();
	void DestroyReferenceSpace();

    void RenderFrame();

	bool RenderLayer(RenderLayerInfo& renderLayerInfo);

private:
    XrInstance m_xrInstance = {};
    XrSystemId m_systemID = {};
    XrSession m_session = XR_NULL_HANDLE;
    XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;


    std::unique_ptr<GraphicsAPI> m_graphicsAPI = nullptr;

    XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

    XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };
    GraphicsAPI_Type m_apiType = UNKNOWN;

    //PFN_xrGetOpenGLGraphicsRequirementsKHR GetOpenGLGraphicsRequirementsFunc = nullptr;

    bool m_applicationRunning = true;
    bool m_sessionRunning = false;

    std::vector<const char*> m_activeAPILayers = {};
    std::vector<const char*> m_activeInstanceExtensions = {};
    std::vector<std::string> m_apiLayers = {};
    std::vector<std::string> m_instanceExtensions = {};

    std::vector<XrViewConfigurationType> m_applicationViewConfigurations = { XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO };
    std::vector<XrViewConfigurationType> m_viewConfigurations;
    XrViewConfigurationType m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
    std::vector<XrViewConfigurationView> m_viewConfigurationViews;

    struct SwapchainInfo {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        int64_t swapchainFormat = 0;
        std::vector<void*> imageViews;
    };
    std::vector<SwapchainInfo> m_colorSwapchainInfos = {};
    std::vector<SwapchainInfo> m_depthSwapchainInfos = {};

    std::vector<XrEnvironmentBlendMode> m_applicationEnvironmentBlendModes = { XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE };
    std::vector<XrEnvironmentBlendMode> m_environmentBlendModes = {};
    XrEnvironmentBlendMode m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;

    XrSpace m_localSpace = XR_NULL_HANDLE;
    struct RenderLayerInfo {
        XrTime predictedDisplayTime;
        std::vector<XrCompositionLayerBaseHeader*> layers;
        XrCompositionLayerProjection layerProjection = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
        std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
    };
};