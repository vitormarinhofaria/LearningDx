#pragma once
#include "LearningDxEngine.h"

namespace Engine {
	class Texture {
	public:
		uint32_t Width;
		uint32_t Height;
		std::string SourcePath;

		ID3D11Texture2D* Texture2D;
		ID3D11ShaderResourceView* TextureView;
		ID3D11SamplerState* TextureSampler;

		/// <summary>
		/// Load and create a texture from an asset path
		/// </summary>
		/// <param name="sourcePath"></param>
		PUBLICAPI Texture(std::string_view sourcePath);
		PUBLICAPI ~Texture();
		/// <summary>
		/// Create a texture from memory data
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="data">Owned by caller, can be freed after this call</param>
		PUBLICAPI Texture(uint32_t width, uint32_t height, uint8_t* data);

	};
}