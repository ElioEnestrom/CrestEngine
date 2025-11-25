// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#define ENABLE_GLOBAL_XR_DISPATCH_TABLE
#include "xrHandle.h"
#include "xrMath.h"
#include "xrError.h"
#include "xrDispatchTable.h"
#include "xrString.h"
#include "pch.h"

#include <glad/glad.h>

namespace sample {
    struct Cube {
        xr::SpaceHandle Space{};
        std::optional<XrPosef> PoseInSpace{}; // Relative pose in cube Space. Default to identity.
        XrVector3f Scale{ 0.1f, 0.1f, 0.1f };

        XrPosef PoseInAppSpace = xr::math::Pose::Identity(); // Cube pose in app space that gets updated every frame
    };

    struct IOpenXrProgram {
        virtual ~IOpenXrProgram() = default;
        virtual void Run() = 0;
    };

    struct IGraphicsPluginD3D11 {
        virtual ~IGraphicsPluginD3D11() = default;

        // Create an instance of this graphics api for the provided instance and systemId.
        virtual ID3D11Device* InitializeDevice(LUID adapterLuid, const std::vector<D3D_FEATURE_LEVEL>& featureLevels) = 0;

        // List of color pixel formats supported by this app.
        virtual const std::vector<DXGI_FORMAT>& SupportedColorFormats() const = 0;
        virtual const std::vector<DXGI_FORMAT>& SupportedDepthFormats() const = 0;

        // Render to swapchain images using stereo image array
        virtual void RenderView(const XrRect2Di& imageRect,
            const float renderTargetClearColor[4],
            const std::vector<xr::math::ViewProjection>& viewProjections,
            DXGI_FORMAT colorSwapchainFormat,
            ID3D11Texture2D* colorTexture,
            DXGI_FORMAT depthSwapchainFormat,
            ID3D11Texture2D* depthTexture,
            const std::vector<const sample::Cube*>& cubes) = 0;
    };

    struct IGraphicsPluginOpenGL {
        virtual ~IGraphicsPluginOpenGL() = default;

        // Initialize OpenGL context and return the graphics binding for OpenXR
        virtual void InitializeDevice(HDC hdc, HGLRC hglrc) = 0;

        // Get the graphics binding structure for OpenXR session creation
        virtual XrGraphicsBindingOpenGLWin32KHR GetGraphicsBinding() const = 0;

        // List of color pixel formats supported by this app (OpenGL internal formats)
        virtual const std::vector<GLenum>& SupportedColorFormats() const = 0;
        virtual const std::vector<GLenum>& SupportedDepthFormats() const = 0;

        // Render to swapchain images using stereo image array
        virtual void RenderView(const XrRect2Di& imageRect,
       const float renderTargetClearColor[4],
            const std::vector<xr::math::ViewProjection>& viewProjections,
            GLenum colorSwapchainFormat,
            uint32_t colorTexture,
            GLenum depthSwapchainFormat,
            uint32_t depthTexture,
         const std::vector<const sample::Cube*>& cubes) = 0;
    };

    std::unique_ptr<IGraphicsPluginD3D11> CreateCubeGraphics();
    std::unique_ptr<IGraphicsPluginOpenGL> CreateCubeGraphicsOpenGL();
    std::unique_ptr<IOpenXrProgram> CreateOpenXrProgram(std::string applicationName, std::unique_ptr<IGraphicsPluginD3D11> graphicsPlugin);
    std::unique_ptr<IOpenXrProgram> CreateOpenXrProgramOpenGL(std::string applicationName, std::unique_ptr<IGraphicsPluginOpenGL> graphicsPlugin);

} // namespace sample
