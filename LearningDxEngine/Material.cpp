#include "Material.h"
#include "LearningDxEngine.h"
#include <d3dcompiler.h>
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"

namespace Engine {
	PUBLICAPI Material::Material(std::wstring_view vertexShaderPath, std::wstring_view pixelShaderPath)
	{
		Renderer& renderer = Game::Get()->mRenderer;
		HRESULT result;
		ID3DBlob* vertexShaderBlob;
		{
			result = D3DReadFileToBlob(vertexShaderPath.data(), &vertexShaderBlob);
			assert(!FAILED(result));

			result = renderer.Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &this->VertexShader);
			assert(!FAILED(result));

			ID3DBlob* pixelShaderBlob;
			result = D3DReadFileToBlob(pixelShaderPath.data(), &pixelShaderBlob);
			assert(!FAILED(result));

			result = renderer.Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &this->PixelShader);
			assert(!FAILED(result));
			pixelShaderBlob->Release();
		}
		{
			D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			result = renderer.Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &this->VertexInputLayout);
			assert(!FAILED(result));
			vertexShaderBlob->Release();
		}
	}
	Material::~Material()
	{
		//this->VertexShader->Release();
		//this->PixelShader->Release();
		//this->VertexInputLayout->Release();
	}
}