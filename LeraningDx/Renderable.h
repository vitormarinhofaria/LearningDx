#pragma once
#include "pch.h"
#include "Renderer.h"
#include "Vertex.h"
#include "tiny_gltf.h"

struct ConstantBuffer {
	DirectX::XMMATRIX mvp;
	DirectX::SimpleMath::Vector3 light;
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX modelInvert;
};

struct LightBuffer
{
	DirectX::XMVECTOR lightPosition;
	DirectX::XMVECTOR lightColor;
	DirectX::XMVECTOR ambientLightColor;
};

struct Renderable {
	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3D11InputLayout* input_layout = nullptr;
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

	ID3D11SamplerState* sampler_state = nullptr;
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* texture_view = nullptr;

	ID3D11Buffer* cbuffer = nullptr;

	uint32_t stride = 0;
	uint32_t offset = 0;
	uint32_t num_verts = 0;
	uint32_t num_inds = 0;
	Renderable() {};
	~Renderable();
	Renderable(std::wstring_view vertex_shader_file, std::wstring_view pixel_shader_file, std::vector<Vertex>& vertices, std::vector<uint32_t>& inds, size_t stride, std::string_view texture_name);
	Renderable(std::wstring_view vertex_shader_file, std::wstring_view pixel_shader_file, std::vector<Vertex>& vertices, std::vector<uint32_t>& inds, tinygltf::Image& image);
};