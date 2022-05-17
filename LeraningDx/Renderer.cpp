#include "pch.h"
#include "Renderer.h"
#include "Messages.h"
#include "ApplicationState.h"
#include "Camera.h"
#include "BasicComponents.h"

void D3D11Renderer::Init(void* window) {

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

		if (FAILED(result))
		{
			PostMessage(nullptr, AM_FAILED_RENDERER, 0, 0);
			return;
		}

		result = base_device->QueryInterface(__uuidof(ID3D11Device4), (void**)&device);
		assert(SUCCEEDED(result));
		base_device->Release();

		result = base_context->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)&context);
		assert(SUCCEEDED(result));
		base_context->Release();
	}
#ifdef _DEBUG
	{
		ID3D11Debug* debug = nullptr;
		device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
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
		HRESULT result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory4), (void**)&dxgi_factory);
		assert(!FAILED(result));
		/*{
			IDXGIDevice4* dxgi_device = nullptr;
			HRESULT result = device->QueryInterface(__uuidof(IDXGIDevice4), (void**)&dxgi_device);
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
		}*/

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

		 result = dxgi_factory->CreateSwapChainForHwnd(device, (HWND)window, &scd_desc, nullptr, nullptr, &swapchain);
		assert(!FAILED(result));
		dxgi_factory->Release();
	}

	{

		D3D11_TEXTURE2D_DESC fb_desc{};
		fb_desc.Height = 1080;
		fb_desc.Width = 1920;
		fb_desc.ArraySize = 1;
		fb_desc.MipLevels = 1;
		fb_desc.SampleDesc.Count = 1;
		fb_desc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
		fb_desc.Usage = D3D11_USAGE_DEFAULT;
		fb_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		HRESULT result = device->CreateTexture2D(&fb_desc, nullptr, &framebuffer);
		assert(!FAILED(result));

		result = device->CreateShaderResourceView(framebuffer, nullptr, &framebuffer_tex_resource);
		assert(!FAILED(result));

		result = device->CreateRenderTargetView(framebuffer, 0, &framebuffer_view);
		assert(!FAILED(result));

		D3D11_TEXTURE2D_DESC depth_buffer_desc{};
		framebuffer->GetDesc(&depth_buffer_desc);

		depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ID3D11Texture2D* depth_buffer = nullptr;

		result = device->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_buffer);
		assert(!FAILED(result));

		result = device->CreateDepthStencilView(depth_buffer, nullptr, &depthstencil_view);
		assert(!FAILED(result));


	}
	{
		HRESULT result;
		ID3D11Texture2D* frame_buffer = nullptr;
		result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frame_buffer);
		assert(!FAILED(result));
		result = device->CreateRenderTargetView(frame_buffer, 0, &present_buffer_view);
		assert(!FAILED(result));

		D3D11_TEXTURE2D_DESC present_depth_desc{};
		frame_buffer->GetDesc(&present_depth_desc);

		present_depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		present_depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		ID3D11Texture2D* present_depth = nullptr;

		result = device->CreateTexture2D(&present_depth_desc, nullptr, &present_depth);
		assert(!FAILED(result));

		result = device->CreateDepthStencilView(present_depth, nullptr, &present_depth_view);
		assert(!FAILED(result));

		frame_buffer->Release();
	}
	{
		D3D11_RASTERIZER_DESC2 raster_desc{};
		raster_desc.FillMode = D3D11_FILL_SOLID;
		raster_desc.CullMode = D3D11_CULL_BACK;
		device->CreateRasterizerState2(&raster_desc, &rasterizer_state);
	}

	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
	}
	{
		D3D11_BUFFER_DESC lightBufferDesc{};
		lightBufferDesc.ByteWidth = sizeof(LightBuffer);
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT result = device->CreateBuffer(&lightBufferDesc, nullptr, &lightBuffer);
		assert(!FAILED(result));
	}
}

void D3D11Renderer::Render(entt::registry& entities)
{
	using namespace DirectX;

	float clear_color[4] = { 0.5f, 0.75f, 0.88f, 1.0f };
	context->ClearRenderTargetView(framebuffer_view, clear_color);
	context->ClearDepthStencilView(depthstencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

	RECT window_rect{};
	GetClientRect(ApplicationState::Get().window, &window_rect);
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f };
	context->RSSetViewports(1, &viewport);
	context->RSSetState(rasterizer_state);

	context->OMSetRenderTargets(1, &framebuffer_view, depthstencil_view);
	context->OMSetDepthStencilState(depth_stencil_state, 0);
	//context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	auto& camera = entities.get<Camera>(entities.view<Camera>().front());

	auto primitives = entities.view<std::unique_ptr<DirectX::GeometricPrimitive>, const Transform>();
	for (auto [entity, primitive, transform] : primitives.each())
	{
		Matrix world = Matrix::Identity *
			Matrix::CreateScale(transform.scale) *
			Matrix::CreateRotationX(transform.rotation.x) *
			Matrix::CreateRotationY(transform.rotation.y) *
			Matrix::CreateRotationZ(transform.rotation.z) *
			Matrix::CreateTranslation(transform.position);

		primitive->Draw(world, camera.getView(), camera.projection);
	}

	auto light = entities.view<Light, Transform>().front();
	auto& lightTransform = entities.get<Transform>(light);
	/// Light Constant Buffer
	D3D11_MAPPED_SUBRESOURCE lightBufferSr{};
	HRESULT result = context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightBufferSr);
	assert(!FAILED(result));
	LightBuffer* p_lightBuffer = (LightBuffer*)lightBufferSr.pData;
	p_lightBuffer->lightPosition = lightTransform.position;
	p_lightBuffer->lightColor = XMVectorSet(1.0f, 0.8f, 0.6f, 1.0f);
	p_lightBuffer->ambientLightColor = XMVectorSet(0.6f, 0.8f, 1.0f, 1.0f);
	context->Unmap(lightBuffer, 0);
	context->PSSetConstantBuffers(0, 1, &lightBuffer);

	auto view = entities.view<const Renderable, const Transform>();
	for (auto [entity, renderable, transform] : view.each())
	{
		D3D11_MAPPED_SUBRESOURCE cbuffer{};
		HRESULT result = context->Map(renderable.cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbuffer);
		assert(!FAILED(result));
		ConstantBuffer* cb = (ConstantBuffer*)cbuffer.pData;

		Matrix world =
			Matrix::Identity *
			Matrix::CreateScale(transform.scale) *
			Matrix::CreateRotationX(transform.rotation.x) *
			Matrix::CreateRotationY(transform.rotation.y) *
			Matrix::CreateRotationZ(transform.rotation.z) *
			Matrix::CreateTranslation(transform.position);
		
		cb->mvp = world * camera.getView() * camera.projection;
		cb->light = lightTransform.position;
		cb->model = world;
		cb->modelInvert = world.Invert();
		context->Unmap(renderable.cbuffer, 0);

		Render(renderable);
	}
}

void D3D11Renderer::Render(const Renderable& renderable)
{
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(renderable.input_layout);

	context->VSSetShader(renderable.vertex_shader, nullptr, 0);
	context->PSSetShader(renderable.pixel_shader, nullptr, 0);

	context->PSSetShaderResources(0, 1, &renderable.texture_view);
	context->PSSetSamplers(0, 1, &renderable.sampler_state);

	context->VSSetConstantBuffers(1, 1, &renderable.cbuffer);
	context->PSSetConstantBuffers(1, 1, &renderable.cbuffer);
	context->IASetVertexBuffers(0, 1, &renderable.vertex_buffer, &renderable.stride, &renderable.offset);
	context->IASetIndexBuffer(renderable.index_buffer, DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(renderable.num_inds, 0, 0);
}
void D3D11Renderer::Present()
{
	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(present_buffer_view, clear_color);
	context->ClearDepthStencilView(present_depth_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

	RECT window_rect{};
	GetClientRect(ApplicationState::Get().window, &window_rect);
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)(window_rect.right - window_rect.left), (float)(window_rect.bottom - window_rect.top), 0.0f, 1.0f };
	context->RSSetViewports(1, &viewport);
	context->RSSetState(rasterizer_state);

	context->OMSetRenderTargets(1, &present_buffer_view, present_depth_view);
	context->OMSetDepthStencilState(depth_stencil_state, 0);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	swapchain->Present(1, 0);
}
void* D3D11Renderer::GetFramebuffer()
{
	return framebuffer_tex_resource;
}
void D3D11Renderer::Dispose()
{
	framebuffer_view->Release();
	swapchain->Release();
	context->Release();
	device->Release();
}