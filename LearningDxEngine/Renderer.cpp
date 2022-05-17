#include "Renderer.h"
#include "LearningDxEngine.h"
#include "Game.h"
#include "Components.h"
#include "Material.h"
#include "Renderable.h"

namespace Engine {
	Renderer::Renderer()
	{
	}
	Renderer::Renderer(HWND pWindow, uint32_t width, uint32_t height) {
		Window = pWindow;
		Width = width;
		Height = height;
		{
			ID3D11Device* base_device = nullptr;
			ID3D11DeviceContext* base_context = nullptr;
			D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
			uint32_t creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
			creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
			HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, nullptr, creation_flags,
				feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, &base_device, nullptr, &base_context);
			assert(!FAILED(result));

			result = base_device->QueryInterface(__uuidof(ID3D11Device4), (void**)&Device);
			assert(SUCCEEDED(result));
			base_device->Release();

			result = base_context->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)&Context);
			assert(SUCCEEDED(result));
			base_context->Release();
		}
#ifdef _DEBUG
		{
			ID3D11Debug* debug = nullptr;
			Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
			if (debug)
			{
				ID3D11InfoQueue* info_q = nullptr;
				if (!FAILED(debug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&info_q)))
				{
					info_q->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
					info_q->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
					info_q->Release();
				}
				debug->Release();
			}
		}
#endif

		{
			IDXGIFactory4* dxgi_factory = nullptr;
			{
				IDXGIDevice4* dxgi_device = nullptr;
				HRESULT result = Device->QueryInterface(__uuidof(IDXGIDevice4), (void**)&dxgi_device);
				assert(!FAILED(result));

				IDXGIAdapter* dxgi_adapter = nullptr;
				result = dxgi_device->GetAdapter(&dxgi_adapter);
				assert(!FAILED(result));
				dxgi_device->Release();

				DXGI_ADAPTER_DESC adapter_desc{};
				dxgi_adapter->GetDesc(&adapter_desc);

				char desc[512]{};
				WideCharToMultiByte(CP_UTF8, 0, adapter_desc.Description, lstrlenW(adapter_desc.Description), desc, 512, nullptr, FALSE);
				fmt::print("Graphics Device: {}\n", desc);

				result = dxgi_adapter->GetParent(__uuidof(IDXGIFactory4), (void**)&dxgi_factory);
				assert(!FAILED(result));
				dxgi_adapter->Release();
			}

			DXGI_SWAP_CHAIN_DESC1 scd_desc = {};
			scd_desc.Width = 0;
			scd_desc.Height = 0;
			scd_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			scd_desc.SampleDesc.Count = 1;
			scd_desc.SampleDesc.Quality = 0;
			scd_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scd_desc.BufferCount = 2;
			scd_desc.Scaling = DXGI_SCALING_STRETCH;
			scd_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scd_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			scd_desc.Flags = 0;

			HRESULT result = dxgi_factory->CreateSwapChainForHwnd(Device, Window, &scd_desc, nullptr, nullptr, &Swapchain);
			assert(!FAILED(result));
			dxgi_factory->Release();
		}

		{
			HRESULT result;

			D3D11_TEXTURE2D_DESC fb_desc{};
			fb_desc.Height = Height;
			fb_desc.Width = Width;
			fb_desc.ArraySize = 1;
			fb_desc.MipLevels = 1;
			fb_desc.SampleDesc.Count = 1;
			fb_desc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
			fb_desc.Usage = D3D11_USAGE_DEFAULT;
			fb_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			result = Device->CreateTexture2D(&fb_desc, nullptr, &RenderTexture);
			assert(!FAILED(result));

			result = Device->CreateShaderResourceView(RenderTexture, nullptr, &RenderTextureView);
			assert(!FAILED(result));

			result = Device->CreateRenderTargetView(RenderTexture, 0, &RenderTextureTargetView);
			assert(!FAILED(result));

			D3D11_TEXTURE2D_DESC depth_buffer_desc{};
			RenderTexture->GetDesc(&depth_buffer_desc);

			depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			ID3D11Texture2D* depth_buffer = nullptr;

			result = Device->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_buffer);
			assert(!FAILED(result));

			result = Device->CreateDepthStencilView(depth_buffer, nullptr, &RenderTextureDepthStencilView);
			assert(!FAILED(result));
		}

		{
			D3D11_RASTERIZER_DESC2 raster_desc{};
			raster_desc.FillMode = D3D11_FILL_SOLID;
			raster_desc.CullMode = D3D11_CULL_BACK;
			Device->CreateRasterizerState2(&raster_desc, &RasterizerState);

			D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
			Device->CreateDepthStencilState(&depth_stencil_desc, &DepthStencilState);
		}
	}

	Renderer::~Renderer()
	{

	}

	PUBLICAPI void Renderer::Render() {

		float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		Context->ClearRenderTargetView(RenderTextureTargetView, clear_color);
		Context->ClearDepthStencilView(RenderTextureDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		RECT window_rect{};
		GetClientRect(Window, &window_rect);
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f };
		Context->RSSetViewports(1, &viewport);
		Context->RSSetState(RasterizerState);

		Context->OMSetRenderTargets(1, &RenderTextureTargetView, RenderTextureDepthStencilView);
		Context->OMSetDepthStencilState(DepthStencilState, 0);

		auto& entities = Game::Get()->mEntities;
		auto& camera = entities.get<Camera>(entities.view<Camera>().front());

		auto view = entities.view<const Renderable, const Material, const Transform>();
		for (auto [entity, renderable, material, transform] : view.each())
		{
			D3D11_MAPPED_SUBRESOURCE cbuffer{};
			HRESULT result = Context->Map(material.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbuffer);
			assert(!FAILED(result));
			MaterialCBuffer* cb = (MaterialCBuffer*)cbuffer.pData;

			Matrix world =
				Matrix::Identity *
				Matrix::CreateScale(transform.Scale) *
				Matrix::CreateRotationX(transform.Rotation.x) *
				Matrix::CreateRotationY(transform.Rotation.y) *
				Matrix::CreateRotationZ(transform.Rotation.z) *
				Matrix::CreateTranslation(transform.Position);
			world.Transpose();
			cb->mvp = world * camera.GetView() * camera.Projection;
			Context->Unmap(material.ConstantBuffer, 0);
			Draw(renderable, material);
		}
	}

	void Renderer::Draw(const Renderable& renderable, const Material& material)
	{
		Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context->IASetInputLayout(material.VertexInputLayout);

		Context->VSSetShader(material.VertexShader, nullptr, 0);
		Context->PSSetShader(material.PixelShader, nullptr, 0);

		for (auto i = 0; i < material.Textures.size(); i++) {
			Context->PSSetShaderResources(i, 1, &material.Textures[i].TextureView);
			Context->PSSetSamplers(i, 1, &material.Textures[i].TextureSampler);
		}

		Context->VSSetConstantBuffers(0, 1, &material.ConstantBuffer);
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		Context->IASetVertexBuffers(0, 1, &renderable.VertexBuffer, &stride, &offset);
		Context->IASetIndexBuffer(renderable.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		Context->DrawIndexed(renderable.GetIndexCount(), 0, 0);
	}
	void* Renderer::GetFramebuffer()
	{
		return RenderTextureView;
	}
}