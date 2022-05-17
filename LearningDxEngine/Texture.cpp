#include "Texture.h"
#include "LearningDxEngine.h"
#include "Game.h"
#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Engine {
	void LoadTexture(uint8_t* data, uint32_t height, uint32_t width, Texture* texture) {
		D3D11_SAMPLER_DESC samplerDesc{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
			.AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
			.AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
			.AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.BorderColor = {1.0f, 1.0f, 1.0f, 1.0f},
		};

		Renderer& renderer = Game::Get()->mRenderer;
		HRESULT result = renderer.Device->CreateSamplerState(&samplerDesc, &texture->TextureSampler);
		assert(!FAILED(result));

		D3D11_TEXTURE2D_DESC textureDesc{
			.Width = width,
			.Height = height,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.SampleDesc = {.Count = 1,.Quality = 0},
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		};

		D3D11_SUBRESOURCE_DATA textureData{ .pSysMem = data, .SysMemPitch = 4 * width };
		result = renderer.Device->CreateTexture2D(&textureDesc, &textureData, &texture->Texture2D);
		assert(!FAILED(result));

		result = renderer.Device->CreateShaderResourceView(texture->Texture2D, nullptr, &texture->TextureView);
		assert(!FAILED(result));
	}

	PUBLICAPI Texture::Texture(std::string_view sourcePath)
	{
		int w, h, c;
		auto* textureData = stbi_load(sourcePath.data(), &w, &h, &c, 4);
		assert(textureData != nullptr);
		Width = w;
		Height = h;
		LoadTexture(textureData, h, w, this);
		stbi_image_free(textureData);
	}

	PUBLICAPI Texture::Texture(uint32_t width, uint32_t height, uint8_t* data)
	{
		LoadTexture(data, height, width, this);
	}

	Texture::~Texture()
	{
	/*	this->TextureView->Release();
		this->TextureSampler->Release();
		this->Texture2D->Release();*/
	}
}