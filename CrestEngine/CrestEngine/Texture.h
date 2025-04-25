#pragma once

namespace Texture {
	enum class MipmapSetting
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};
	// Function declaration
	unsigned int loadTexture(const char* filename);
	void switchMipmapSetting(unsigned int textureID, MipmapSetting setting);
}