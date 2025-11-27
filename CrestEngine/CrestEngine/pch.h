// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <array>
#include <map>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>


// Support both D3D11 and OpenGL backends
#ifdef USE_OPENGL_BACKEND
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_WGL
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
    
    #define XR_USE_PLATFORM_WIN32
    #define XR_USE_GRAPHICS_API_OPENGL
#else
    #include <d3d11.h>
    
    #define XR_USE_PLATFORM_WIN32
    #define XR_USE_GRAPHICS_API_D3D11
#endif

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_PLATFORM_WIN32
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <openxr/openxr_reflection.h>

/*#define ENABLE_GLOBAL_XR_DISPATCH_TABLE
#include <xrDispatchTable.h>
#include <xrError.h>
#include <xrHandle.h>
#include <xrMath.h>
#include <xrString.h>*/

#include <winrt/base.h> // winrt::com_ptr
