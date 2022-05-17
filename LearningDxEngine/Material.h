#pragma once
#include "LearningDxEngine.h"
#include "Texture.h"

namespace Engine {
	struct MaterialCBuffer {
		DirectX::SimpleMath::Matrix mvp;
	};

	class Material {
	public:
		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader;
		ID3D11InputLayout* VertexInputLayout;

		std::vector<Texture> Textures;

		ID3D11Buffer* ConstantBuffer;
		PUBLICAPI Material(std::wstring_view vertexShaderPath, std::wstring_view pixelShaderPath);
		PUBLICAPI ~Material();
	};
}