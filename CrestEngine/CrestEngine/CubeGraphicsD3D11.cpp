#include "OpenXrProgram.h"

namespace sample {

class NullGraphicsPluginD3D11 final : public IGraphicsPluginD3D11 {
public:
    NullGraphicsPluginD3D11()
        : m_colorFormats{ DXGI_FORMAT_R8G8B8A8_UNORM }
        , m_depthFormats{ DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D32_FLOAT } {}

    ID3D11Device* InitializeDevice(LUID adapterLuid, const std::vector<D3D_FEATURE_LEVEL>& featureLevels) override {
        (void)adapterLuid;
        (void)featureLevels;
        // Minimal stub: no device created.
        return nullptr;
    }

    const std::vector<DXGI_FORMAT>& SupportedColorFormats() const override {
        return m_colorFormats;
    }

    const std::vector<DXGI_FORMAT>& SupportedDepthFormats() const override {
        return m_depthFormats;
    }

    void RenderView(const XrRect2Di& imageRect,
                    const float renderTargetClearColor[4],
                    const std::vector<xr::math::ViewProjection>& viewProjections,
                    DXGI_FORMAT colorSwapchainFormat,
                    ID3D11Texture2D* colorTexture,
                    DXGI_FORMAT depthSwapchainFormat,
                    ID3D11Texture2D* depthTexture,
                    const std::vector<const sample::Cube*>& cubes) override {
        (void)imageRect;
        (void)renderTargetClearColor;
        (void)viewProjections;
        (void)colorSwapchainFormat;
        (void)colorTexture;
        (void)depthSwapchainFormat;
        (void)depthTexture;
        (void)cubes;
        // No-op stub.
    }

private:
    std::vector<DXGI_FORMAT> m_colorFormats;
    std::vector<DXGI_FORMAT> m_depthFormats;
};

std::unique_ptr<IGraphicsPluginD3D11> CreateCubeGraphics() {
    return std::make_unique<NullGraphicsPluginD3D11>();
}

} // namespace sample