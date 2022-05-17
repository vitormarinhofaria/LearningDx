#include "pch.h"
#include "Renderer.h"
#include "Renderable.h"
#include "ApplicationState.h"
#include "stb_image.h"
#include "Vertex.h"

Renderable::~Renderable()
{
	/*vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
	index_buffer->Release();
	sampler_state->Release();
	texture->Release();
	texture_view->Release();
	cbuffer->Release();*/
}

Renderable::Renderable(std::wstring_view vertex_shader_file, std::wstring_view pixel_shader_file, std::vector<Vertex>& vertices, std::vector<uint32_t>& inds, size_t p_stride, std::string_view texture_name)
{
	auto& renderer = *(D3D11Renderer*)ApplicationState::Get().renderer;
	num_inds = inds.size();
	HRESULT result = S_OK;
	ID3DBlob* vertex_shader_blob = nullptr;
	{
		result = D3DReadFileToBlob(vertex_shader_file.data(), &vertex_shader_blob);
		//result = D3DCompileFromFile(vertex_shader_file.data(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertex_shader_blob, nullptr);
		assert(!FAILED(result));

		result = renderer.device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader);
		assert(!FAILED(result));

		ID3DBlob* pixel_shader_blob = nullptr;
		result = D3DReadFileToBlob(pixel_shader_file.data(), &pixel_shader_blob);
		//result = D3DCompileFromFile(pixel_shader_file.data(), nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixel_shader_blob, nullptr);
		assert(!FAILED(result));

		result = renderer.device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader);
		assert(!FAILED(result));
		pixel_shader_blob->Release();
	}

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
			{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		result = renderer.device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc),
			vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
		assert(!FAILED(result));

		vertex_shader_blob->Release();
	}

	{
		stride = sizeof(Vertex);
		num_verts = vertices.size();
		offset = 0;

		D3D11_BUFFER_DESC vertex_buffer_desc{};
		vertex_buffer_desc.ByteWidth = vertices.size() * sizeof(Vertex);
		vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertex_subres_data = { vertices.data() };

		result = renderer.device->CreateBuffer(&vertex_buffer_desc, &vertex_subres_data, &vertex_buffer);
		assert(!FAILED(result));

		D3D11_BUFFER_DESC index_buffer_desc{};
		index_buffer_desc.ByteWidth = inds.size() * sizeof(uint32_t);
		index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA index_buffer_data = { inds.data() };
		result = renderer.device->CreateBuffer(&index_buffer_desc, &index_buffer_data, &index_buffer);
	}

	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.BorderColor[0] = 1.0f;
		sampler_desc.BorderColor[1] = 1.0f;
		sampler_desc.BorderColor[2] = 1.0f;
		sampler_desc.BorderColor[3] = 1.0f;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		result = renderer.device->CreateSamplerState(&sampler_desc, &sampler_state);
		assert(!FAILED(result));

		int32_t tex_w, tex_h, tex_ch = 0;
		int32_t tex_force_ch = 4;

		uint8_t* texture_bytes = stbi_load(texture_name.data(), &tex_w, &tex_h, &tex_ch, tex_force_ch);
		assert(texture_bytes != nullptr);
		int32_t tex_bytes_per_row = 4 * tex_w;

		D3D11_TEXTURE2D_DESC tex_desc{};
		tex_desc.Width = tex_w;
		tex_desc.Height = tex_h;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA tex_subres{};
		tex_subres.pSysMem = texture_bytes;
		tex_subres.SysMemPitch = tex_bytes_per_row;

		result = renderer.device->CreateTexture2D(&tex_desc, &tex_subres, &texture);
		assert(!FAILED(result));

		result = renderer.device->CreateShaderResourceView(texture, nullptr, &texture_view);
		assert(!FAILED(result));

		stbi_image_free(texture_bytes);
	}

	{
		D3D11_BUFFER_DESC cb_desc{};
		//cb_desc.ByteWidth = sizeof(ConstantBuffer) + 0xf & 0xfffffff0;
		cb_desc.ByteWidth = sizeof(ConstantBuffer);
		cb_desc.Usage = D3D11_USAGE_DYNAMIC;
		cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		result = renderer.device->CreateBuffer(&cb_desc, nullptr, &cbuffer);
	}
}

Renderable::Renderable(std::wstring_view vertex_shader_file, std::wstring_view pixel_shader_file, std::vector<Vertex>& vertices, std::vector<uint32_t>& inds, tinygltf::Image& image)
{
	auto& renderer = *(D3D11Renderer*)ApplicationState::Get().renderer;
	num_inds = inds.size();
	HRESULT result = S_OK;
	ID3DBlob* vertex_shader_blob = nullptr;
	{
		result = D3DReadFileToBlob(vertex_shader_file.data(), &vertex_shader_blob);
		//result = D3DCompileFromFile(vertex_shader_file.data(), nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertex_shader_blob, nullptr);
		assert(!FAILED(result));

		result = renderer.device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader);
		assert(!FAILED(result));

		ID3DBlob* pixel_shader_blob = nullptr;
		result = D3DReadFileToBlob(pixel_shader_file.data(), &pixel_shader_blob);
		//result = D3DCompileFromFile(pixel_shader_file.data(), nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixel_shader_blob, nullptr);
		assert(!FAILED(result));

		result = renderer.device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader);
		assert(!FAILED(result));
		pixel_shader_blob->Release();
	}

	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
			{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		result = renderer.device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc),
			vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
		assert(!FAILED(result));

		vertex_shader_blob->Release();
	}

	{
		stride = sizeof(Vertex);
		num_verts = vertices.size();
		offset = 0;

		D3D11_BUFFER_DESC vertex_buffer_desc{};
		vertex_buffer_desc.ByteWidth = vertices.size() * sizeof(Vertex);
		vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertex_subres_data = { vertices.data() };

		result = renderer.device->CreateBuffer(&vertex_buffer_desc, &vertex_subres_data, &vertex_buffer);
		assert(!FAILED(result));

		D3D11_BUFFER_DESC index_buffer_desc{};
		index_buffer_desc.ByteWidth = inds.size() * sizeof(uint32_t);
		index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA index_buffer_data = { inds.data() };
		result = renderer.device->CreateBuffer(&index_buffer_desc, &index_buffer_data, &index_buffer);
	}

	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.BorderColor[0] = 1.0f;
		sampler_desc.BorderColor[1] = 1.0f;
		sampler_desc.BorderColor[2] = 1.0f;
		sampler_desc.BorderColor[3] = 1.0f;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		result = renderer.device->CreateSamplerState(&sampler_desc, &sampler_state);
		assert(!FAILED(result));

		D3D11_TEXTURE2D_DESC tex_desc{};
		tex_desc.Width = image.width;
		tex_desc.Height = image.height;
		tex_desc.MipLevels = 1;
		tex_desc.ArraySize = 1;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA tex_subres{};
		tex_subres.pSysMem = image.image.data();
		tex_subres.SysMemPitch = 4 * image.width;

		result = renderer.device->CreateTexture2D(&tex_desc, &tex_subres, &texture);
		assert(!FAILED(result));

		result = renderer.device->CreateShaderResourceView(texture, nullptr, &texture_view);
		assert(!FAILED(result));
	}

	{
		D3D11_BUFFER_DESC cb_desc{};
		//cb_desc.ByteWidth = sizeof(ConstantBuffer) + 0xf & 0xfffffff0;
		cb_desc.ByteWidth = sizeof(ConstantBuffer);
		cb_desc.Usage = D3D11_USAGE_DYNAMIC;
		cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		result = renderer.device->CreateBuffer(&cb_desc, nullptr, &cbuffer);
	}
	
}
