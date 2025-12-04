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
    XrResult result = xrCreateInstance(&createInfo, &m_xrInstance);
    if (XR_FAILED(result)) {
        std::cerr << "xrCreateInstance failed: " << result << std::endl;
        return false;
    }
    
    std::cout << "OpenXR instance created!" << std::endl;
    
    // 3a: Ask for a head-mounted display
    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemInfo, &m_systemID), "Failed to get SystemID.");
    

    // Get the System's properties for some general information about the hardware and the vendor.
    OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_systemID, &m_systemProperties), "Failed to get SystemProperties.");

    //// 3b: Get the system
    //result = xrGetSystem(m_xrInstance, &systemInfo, &m_systemID);
    //if (XR_FAILED(result)) {
    //    std::cerr << "xrGetSystem failed: " << result << std::endl;
    //    return false;
    //}
    
    std::cout << "VR system found!" << std::endl;
    
    
    m_graphicsAPI = std::make_unique<GraphicsAPI_OpenGL>(m_xrInstance, m_systemID);
    // 4a: Get OpenGL context handles (Windows-specific)
#ifdef _WIN32
    XrGraphicsBindingOpenGLWin32KHR graphicsBinding{XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
    graphicsBinding.hDC = wglGetCurrentDC();
    graphicsBinding.hGLRC = wglGetCurrentContext();
#endif
    
	GetViewConfigurationViews();
	GetEnvironmentBlendModes();

	CreateSession();
	CreateReferenceSpace();
    CreateSwapchains();

    std::cout << "VR session created!" << std::endl;
    
    return true;
}

void VRManager::Shutdown() {
    if (m_xrInstance != XR_NULL_HANDLE) {
        xrDestroyInstance(m_xrInstance);
    }
}

void VRManager::GetInstanceProperties()
{
    // Get the instance's properties and log the runtime name and version.
    XrInstanceProperties instanceProperties{ XR_TYPE_INSTANCE_PROPERTIES };
    OPENXR_CHECK(xrGetInstanceProperties(m_xrInstance, &instanceProperties), "Failed to get InstanceProperties.");
    
    XR_TUT_LOG("OpenXR Runtime: " << instanceProperties.runtimeName << " - "
        << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
        << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << "."
        << XR_VERSION_PATCH(instanceProperties.runtimeVersion));
}

void VRManager::DestroyInstance()
{
    xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.";
}

void VRManager::CreateDebugMessenger()
{
    // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before creating an XrDebugUtilsMessengerEXT.
    if (IsStringInVector(m_activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        m_debugUtilsMessenger = CreateOpenXRDebugUtilsMessenger(m_xrInstance);  // From OpenXRDebugUtils.h.
    }
}

void VRManager::CreateSession()
{
    XrSessionCreateInfo sessionCreateInfo{ XR_TYPE_SESSION_CREATE_INFO };
    sessionCreateInfo.next = m_graphicsAPI->GetGraphicsBinding();
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = m_systemID;
	OPENXR_CHECK(xrCreateSession(m_xrInstance, &sessionCreateInfo, &m_session), "Failed to create Session.");
}

void VRManager::DestroySession()
{
    OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
}

void VRManager::CreateSwapchains()
{
    // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
    uint32_t formatCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, 0, &formatCount, nullptr), "Failed to enumerate Swapchain Formats");
    std::vector<int64_t> formats(formatCount);
    OPENXR_CHECK(xrEnumerateSwapchainFormats(m_session, formatCount, &formatCount, formats.data()), "Failed to enumerate Swapchain Formats");
    if (m_graphicsAPI->SelectDepthSwapchainFormat(formats) == 0) {
        std::cerr << "Failed to find depth format for Swapchain." << std::endl;
        DEBUG_BREAK;
    }

    //Resize the SwapchainInfo to match the number of view in the View Configuration.
    m_colorSwapchainInfos.resize(m_viewConfigurationViews.size());
    m_depthSwapchainInfos.resize(m_viewConfigurationViews.size());
    
    for (size_t i = 0; i < m_viewConfigurationViews.size(); i++) 
    {
        SwapchainInfo& colorSwapchainInfo = m_colorSwapchainInfos[i];
        SwapchainInfo& depthSwapchainInfo = m_depthSwapchainInfos[i];

        // Fill out an XrSwapchainCreateInfo structure and create an XrSwapchain.
        // Color.
        XrSwapchainCreateInfo swapchainCI{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
        swapchainCI.createFlags = 0;
        swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.format = m_graphicsAPI->SelectColorSwapchainFormat(formats);                // Use GraphicsAPI to select the first compatible format.
        swapchainCI.sampleCount = m_viewConfigurationViews[i].recommendedSwapchainSampleCount;  // Use the recommended values from the XrViewConfigurationView.
        swapchainCI.width = m_viewConfigurationViews[i].recommendedImageRectWidth;
        swapchainCI.height = m_viewConfigurationViews[i].recommendedImageRectHeight;
        swapchainCI.faceCount = 1;
        swapchainCI.arraySize = 1;
        swapchainCI.mipCount = 1;
        OPENXR_CHECK(xrCreateSwapchain(m_session, &swapchainCI, &colorSwapchainInfo.swapchain), "Failed to create Color Swapchain");
        colorSwapchainInfo.swapchainFormat = swapchainCI.format;  // Save the swapchain format for later use.

        // Depth.
        swapchainCI.createFlags = 0;
        swapchainCI.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        swapchainCI.format = m_graphicsAPI->SelectDepthSwapchainFormat(formats);                // Use GraphicsAPI to select the first compatible format.
        swapchainCI.sampleCount = m_viewConfigurationViews[i].recommendedSwapchainSampleCount;  // Use the recommended values from the XrViewConfigurationView.
        swapchainCI.width = m_viewConfigurationViews[i].recommendedImageRectWidth;
        swapchainCI.height = m_viewConfigurationViews[i].recommendedImageRectHeight;
        swapchainCI.faceCount = 1;
        swapchainCI.arraySize = 1;
        swapchainCI.mipCount = 1;
        OPENXR_CHECK(xrCreateSwapchain(m_session, &swapchainCI, &depthSwapchainInfo.swapchain), "Failed to create Depth Swapchain");
        depthSwapchainInfo.swapchainFormat = swapchainCI.format;  // Save the swapchain format for later use.

        // Get the number of images in the color/depth swapchain and allocate Swapchain image data via GraphicsAPI to store the returned array.
        uint32_t colorSwapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(colorSwapchainInfo.swapchain, 0, &colorSwapchainImageCount, nullptr), "Failed to enumerate Color Swapchain Images.");
        XrSwapchainImageBaseHeader* colorSwapchainImages = m_graphicsAPI->AllocateSwapchainImageData(colorSwapchainInfo.swapchain, GraphicsAPI::SwapchainType::COLOR, colorSwapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(colorSwapchainInfo.swapchain, colorSwapchainImageCount, &colorSwapchainImageCount, colorSwapchainImages), "Failed to enumerate Color Swapchain Images.");

        uint32_t depthSwapchainImageCount = 0;
        OPENXR_CHECK(xrEnumerateSwapchainImages(depthSwapchainInfo.swapchain, 0, &depthSwapchainImageCount, nullptr), "Failed to enumerate Depth Swapchain Images.");
        XrSwapchainImageBaseHeader* depthSwapchainImages = m_graphicsAPI->AllocateSwapchainImageData(depthSwapchainInfo.swapchain, GraphicsAPI::SwapchainType::DEPTH, depthSwapchainImageCount);
        OPENXR_CHECK(xrEnumerateSwapchainImages(depthSwapchainInfo.swapchain, depthSwapchainImageCount, &depthSwapchainImageCount, depthSwapchainImages), "Failed to enumerate Depth Swapchain Images.");

        // Per image in the swapchains, fill out a GraphicsAPI::ImageViewCreateInfo structure and create a color/depth image view.
        for (uint32_t j = 0; j < colorSwapchainImageCount; j++) {
            GraphicsAPI::ImageViewCreateInfo imageViewCI;
            imageViewCI.image = m_graphicsAPI->GetSwapchainImage(colorSwapchainInfo.swapchain, j);
            imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
            imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
            imageViewCI.format = colorSwapchainInfo.swapchainFormat;
            imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;
            imageViewCI.baseMipLevel = 0;
            imageViewCI.levelCount = 1;
            imageViewCI.baseArrayLayer = 0;
            imageViewCI.layerCount = 1;
            colorSwapchainInfo.imageViews.push_back(m_graphicsAPI->CreateImageView(imageViewCI));
        }
        for (uint32_t j = 0; j < depthSwapchainImageCount; j++) {
            GraphicsAPI::ImageViewCreateInfo imageViewCI;
            imageViewCI.image = m_graphicsAPI->GetSwapchainImage(depthSwapchainInfo.swapchain, j);
            imageViewCI.type = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
            imageViewCI.view = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
            imageViewCI.format = depthSwapchainInfo.swapchainFormat;
            imageViewCI.aspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
            imageViewCI.baseMipLevel = 0;
            imageViewCI.levelCount = 1;
            imageViewCI.baseArrayLayer = 0;
            imageViewCI.layerCount = 1;
            depthSwapchainInfo.imageViews.push_back(m_graphicsAPI->CreateImageView(imageViewCI));
        }
    }
}

void VRManager::DestroySwapchains()
{
    // Per view in the view configuration:
    for (size_t i = 0; i < m_viewConfigurationViews.size(); i++) {
        SwapchainInfo& colorSwapchainInfo = m_colorSwapchainInfos[i];
        SwapchainInfo& depthSwapchainInfo = m_depthSwapchainInfos[i];

        // Destroy the color and depth image views from GraphicsAPI.
        for (void*& imageView : colorSwapchainInfo.imageViews) {
            m_graphicsAPI->DestroyImageView(imageView);
        }
        for (void*& imageView : depthSwapchainInfo.imageViews) {
            m_graphicsAPI->DestroyImageView(imageView);
        }

        // Free the Swapchain Image Data.
        m_graphicsAPI->FreeSwapchainImageData(colorSwapchainInfo.swapchain);
        m_graphicsAPI->FreeSwapchainImageData(depthSwapchainInfo.swapchain);

        // Destroy the swapchains.
        OPENXR_CHECK(xrDestroySwapchain(colorSwapchainInfo.swapchain), "Failed to destroy Color Swapchain");
        OPENXR_CHECK(xrDestroySwapchain(depthSwapchainInfo.swapchain), "Failed to destroy Depth Swapchain");
    }
}

void VRManager::GetViewConfigurationViews()
{
    // Gets the View Configuration Types. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationCount = 0;
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_xrInstance, m_systemID, 0, &viewConfigurationCount, nullptr), "Failed to enumerate View Configurations.");
    m_viewConfigurations.resize(viewConfigurationCount);
    OPENXR_CHECK(xrEnumerateViewConfigurations(m_xrInstance, m_systemID, viewConfigurationCount, &viewConfigurationCount, m_viewConfigurations.data()), "Failed to enumerate View Configurations.");

    // Pick the first application supported View Configuration Type con supported by the hardware.
    for (const XrViewConfigurationType& viewConfiguration : m_applicationViewConfigurations) {
        if (std::find(m_viewConfigurations.begin(), m_viewConfigurations.end(), viewConfiguration) != m_viewConfigurations.end()) {
            m_viewConfiguration = viewConfiguration;
            break;
        }
    }
    if (m_viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
        std::cerr << "Failed to find a view configuration type. Defaulting to XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO." << std::endl;
        m_viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    }

    // Gets the View Configuration Views. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t viewConfigurationViewCount = 0;
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, 0, &viewConfigurationViewCount, nullptr), "Failed to enumerate ViewConfiguration Views.");
    m_viewConfigurationViews.resize(viewConfigurationViewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemID, m_viewConfiguration, viewConfigurationViewCount, &viewConfigurationViewCount, m_viewConfigurationViews.data()), "Failed to enumerate ViewConfiguration Views.");
}

void VRManager::GetEnvironmentBlendModes()
{
    // Retrieves the available blend modes. The first call gets the count of the array that will be returned. The next call fills out the array.
    uint32_t environmentBlendModeCount = 0;
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, 0, &environmentBlendModeCount, nullptr), "Failed to enumerate EnvironmentBlend Modes.");
    m_environmentBlendModes.resize(environmentBlendModeCount);
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(m_xrInstance, m_systemID, m_viewConfiguration, environmentBlendModeCount, &environmentBlendModeCount, m_environmentBlendModes.data()), "Failed to enumerate EnvironmentBlend Modes.");

    // Pick the first application supported blend mode supported by the hardware.
    for (const XrEnvironmentBlendMode& environmentBlendMode : m_applicationEnvironmentBlendModes) {
        if (std::find(m_environmentBlendModes.begin(), m_environmentBlendModes.end(), environmentBlendMode) != m_environmentBlendModes.end()) {
            m_environmentBlendMode = environmentBlendMode;
            break;
        }
    }
    if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM) {
        XR_TUT_LOG_ERROR("Failed to find a compatible blend mode. Defaulting to XR_ENVIRONMENT_BLEND_MODE_OPAQUE.");
        m_environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    }
}

void VRManager::CreateReferenceSpace()
{
    // Fill out an XrReferenceSpaceCreateInfo structure and create a reference XrSpace, specifying a Local space with an identity pose as the origin.
    XrReferenceSpaceCreateInfo referenceSpaceCI{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    referenceSpaceCI.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    referenceSpaceCI.poseInReferenceSpace = { {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} };
    OPENXR_CHECK(xrCreateReferenceSpace(m_session, &referenceSpaceCI, &m_localSpace), "Failed to create ReferenceSpace.");
}

void VRManager::DestroyReferenceSpace()
{
    // Destroy the reference XrSpace.
    OPENXR_CHECK(xrDestroySpace(m_localSpace), "Failed to destroy Space.")
}

void VRManager::RenderFrame()
{
    // Get the XrFrameState for timing and rendering info.
    XrFrameState frameState{ XR_TYPE_FRAME_STATE };
    XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
    OPENXR_CHECK(xrWaitFrame(m_session, &frameWaitInfo, &frameState), "Failed to wait for XR Frame.");

    // Tell the OpenXR compositor that the application is beginning the frame.
    XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
    OPENXR_CHECK(xrBeginFrame(m_session, &frameBeginInfo), "Failed to begin the XR Frame.");

    // Variables for rendering and layer composition.
    bool rendered = false;
    RenderLayerInfo renderLayerInfo;
    renderLayerInfo.predictedDisplayTime = frameState.predictedDisplayTime;

    // Check that the session is active and that we should render.
    bool sessionActive = (m_sessionState == XR_SESSION_STATE_SYNCHRONIZED || m_sessionState == XR_SESSION_STATE_VISIBLE || m_sessionState == XR_SESSION_STATE_FOCUSED);
    if (sessionActive && frameState.shouldRender) {
        // Render the stereo image and associate one of swapchain images with the XrCompositionLayerProjection structure.
        rendered = RenderLayer(renderLayerInfo);
        if (rendered) {
            renderLayerInfo.layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&renderLayerInfo.layerProjection));
        }
    }

    // Tell OpenXR that we are finished with this frame; specifying its display time, environment blending and layers.
    XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = m_environmentBlendMode;
    frameEndInfo.layerCount = static_cast<uint32_t>(renderLayerInfo.layers.size());
    frameEndInfo.layers = renderLayerInfo.layers.data();
    OPENXR_CHECK(xrEndFrame(m_session, &frameEndInfo), "Failed to end the XR Frame.");
}

bool VRManager::RenderLayer(RenderLayerInfo& renderLayerInfo)
{
    // Locate the views from the view configuration within the (reference) space at the display time.
    std::vector<XrView> views(m_viewConfigurationViews.size(), { XR_TYPE_VIEW });

    XrViewState viewState{ XR_TYPE_VIEW_STATE };  // Will contain information on whether the position and/or orientation is valid and/or tracked.
    XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
    viewLocateInfo.viewConfigurationType = m_viewConfiguration;
    viewLocateInfo.displayTime = renderLayerInfo.predictedDisplayTime;
    viewLocateInfo.space = m_localSpace;
    uint32_t viewCount = 0;
    XrResult result = xrLocateViews(m_session, &viewLocateInfo, &viewState, static_cast<uint32_t>(views.size()), &viewCount, views.data());
    if (result != XR_SUCCESS) {
        XR_TUT_LOG("Failed to locate Views.");
        return false;
    }

    // Resize the layer projection views to match the view count. The layer projection views are used in the layer projection.
    renderLayerInfo.layerProjectionViews.resize(viewCount, { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW });

    // Per view in the view configuration:
    for (uint32_t i = 0; i < viewCount; i++) {
        SwapchainInfo& colorSwapchainInfo = m_colorSwapchainInfos[i];
        SwapchainInfo& depthSwapchainInfo = m_depthSwapchainInfos[i];

        // Acquire and wait for an image from the swapchains.
        // Get the image index of an image in the swapchains.
        // The timeout is infinite.
        uint32_t colorImageIndex = 0;
        uint32_t depthImageIndex = 0;
        XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
        OPENXR_CHECK(xrAcquireSwapchainImage(colorSwapchainInfo.swapchain, &acquireInfo, &colorImageIndex), "Failed to acquire Image from the Color Swapchian");
        OPENXR_CHECK(xrAcquireSwapchainImage(depthSwapchainInfo.swapchain, &acquireInfo, &depthImageIndex), "Failed to acquire Image from the Depth Swapchian");

        XrSwapchainImageWaitInfo waitInfo = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
        waitInfo.timeout = XR_INFINITE_DURATION;
        OPENXR_CHECK(xrWaitSwapchainImage(colorSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Color Swapchain");
        OPENXR_CHECK(xrWaitSwapchainImage(depthSwapchainInfo.swapchain, &waitInfo), "Failed to wait for Image from the Depth Swapchain");

        // Get the width and height and construct the viewport and scissors.
        const uint32_t& width = m_viewConfigurationViews[i].recommendedImageRectWidth;
        const uint32_t& height = m_viewConfigurationViews[i].recommendedImageRectHeight;
        GraphicsAPI::Viewport viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
        GraphicsAPI::Rect2D scissor = { {(int32_t)0, (int32_t)0}, {width, height} };
        float nearZ = 0.05f;
        float farZ = 100.0f;

        // Fill out the XrCompositionLayerProjectionView structure specifying the pose and fov from the view.
        // This also associates the swapchain image with this layer projection view.
        renderLayerInfo.layerProjectionViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
        renderLayerInfo.layerProjectionViews[i].pose = views[i].pose;
        renderLayerInfo.layerProjectionViews[i].fov = views[i].fov;
        renderLayerInfo.layerProjectionViews[i].subImage.swapchain = colorSwapchainInfo.swapchain;
        renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.x = 0;
        renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.y = 0;
        renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.width = static_cast<int32_t>(width);
        renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.height = static_cast<int32_t>(height);
        renderLayerInfo.layerProjectionViews[i].subImage.imageArrayIndex = 0;  // Useful for multiview rendering.

        // Rendering code to clear the color and depth image views.
        m_graphicsAPI->BeginRendering();

        if (m_environmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) {
            // VR mode use a background color.
            m_graphicsAPI->ClearColor(colorSwapchainInfo.imageViews[colorImageIndex], 0.67f, 0.17f, 0.42f, 1.00f);
        }
        else {
            // In AR mode make the background color black.
            m_graphicsAPI->ClearColor(colorSwapchainInfo.imageViews[colorImageIndex], 0.00f, 0.00f, 0.00f, 1.00f);
        }
        m_graphicsAPI->ClearDepth(depthSwapchainInfo.imageViews[depthImageIndex], 1.0f);        
        
        m_graphicsAPI->EndRendering();

        // Give the swapchain image back to OpenXR, allowing the compositor to use the image.
        XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
        OPENXR_CHECK(xrReleaseSwapchainImage(colorSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Color Swapchain");
        OPENXR_CHECK(xrReleaseSwapchainImage(depthSwapchainInfo.swapchain, &releaseInfo), "Failed to release Image back to the Depth Swapchain");
    }

    // Fill out the XrCompositionLayerProjection structure for usage with xrEndFrame().
    renderLayerInfo.layerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    renderLayerInfo.layerProjection.space = m_localSpace;
    renderLayerInfo.layerProjection.viewCount = static_cast<uint32_t>(renderLayerInfo.layerProjectionViews.size());
    renderLayerInfo.layerProjection.views = renderLayerInfo.layerProjectionViews.data();

    return true;
}


void VRManager::DestroyDebugMessenger()
{
    // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
    if (m_debugUtilsMessenger != XR_NULL_HANDLE) {
        DestroyOpenXRDebugUtilsMessenger(m_xrInstance, m_debugUtilsMessenger);  // From OpenXRDebugUtils.h.
    }
}

void VRManager::PollEvents()
{
    // Poll OpenXR for a new event.
    XrEventDataBuffer eventData{ XR_TYPE_EVENT_DATA_BUFFER };
    auto XrPollEvents = [&]() -> bool {
        eventData = { XR_TYPE_EVENT_DATA_BUFFER };
        return xrPollEvent(m_xrInstance, &eventData) == XR_SUCCESS;
        };

    while (XrPollEvents()) {
        switch (eventData.type) {
        // Log the number of lost events from the runtime.
        case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
            XrEventDataEventsLost* eventsLost = reinterpret_cast<XrEventDataEventsLost*>(&eventData);
            XR_TUT_LOG("OPENXR: Events Lost: " << eventsLost->lostEventCount);
            break;
        }
        // Log that an instance loss is pending and shutdown the application.
        case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
            XrEventDataInstanceLossPending* instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData);
            XR_TUT_LOG("OPENXR: Instance Loss Pending at: " << instanceLossPending->lossTime);
            m_sessionRunning = false;
            m_applicationRunning = false;
            break;
        }
        // Log that the interaction profile has changed.
        case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
            XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData);
            XR_TUT_LOG("OPENXR: Interaction Profile changed for Session: " << interactionProfileChanged->session);
            if (interactionProfileChanged->session != m_session) {
                XR_TUT_LOG("XrEventDataInteractionProfileChanged for unknown Session");
                break;
            }
            break;
        }
        // Log that there's a reference space change pending.
        case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
            XrEventDataReferenceSpaceChangePending* referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData);
            XR_TUT_LOG("OPENXR: Reference Space Change pending for Session: " << referenceSpaceChangePending->session);
            if (referenceSpaceChangePending->session != m_session) {
                XR_TUT_LOG("XrEventDataReferenceSpaceChangePending for unknown Session");
                break;
            }
            break;
        }
        // Session State changes:
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
            XrEventDataSessionStateChanged* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
            if (sessionStateChanged->session != m_session) {
                XR_TUT_LOG("XrEventDataSessionStateChanged for unknown Session");
                break;
            }

            if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
                // SessionState is ready. Begin the XrSession using the XrViewConfigurationType.
                XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
                sessionBeginInfo.primaryViewConfigurationType = m_viewConfiguration;
                OPENXR_CHECK(xrBeginSession(m_session, &sessionBeginInfo), "Failed to begin Session.");
                m_sessionRunning = true;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
                // SessionState is stopping. End the XrSession.
                OPENXR_CHECK(xrEndSession(m_session), "Failed to end Session.");
                m_sessionRunning = false;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
                // SessionState is exiting. Exit the application.
                m_sessionRunning = false;
                m_applicationRunning = false;
            }
            if (sessionStateChanged->state == XR_SESSION_STATE_LOSS_PENDING) {
                // SessionState is loss pending. Exit the application.
                // It's possible to try a reestablish an XrInstance and XrSession, but we will simply exit here.
                m_sessionRunning = false;
                m_applicationRunning = false;
            }
            // Store state for reference across the application.
            m_sessionState = sessionStateChanged->state;
            break;
        }
        default: {
            break;
        }
        }
    }
}

void VRManager::PollSystemEvents()
{

}

void VRManager::LocateEyes()
{

    XrView m_views[2];
    XrFrameState frameState;
    //xrLocateViews(m_session, &locateInfo, &viewState, 2, &viewCountOutput, m_views);
}
