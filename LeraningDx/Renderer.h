#pragma once
#include "pch.h"
#include "Renderable.h"

struct IRenderer {
	virtual void Init(void* window) = 0;
	virtual void Dispose() = 0;
	virtual void Render(entt::registry& entitites) = 0;
	virtual void Render(const Renderable& renderable) = 0;
	virtual void Present() = 0;
	virtual void* GetFramebuffer() = 0;
};

struct D3D11Renderer : IRenderer {
	ID3D11Device4* device;
	ID3D11DeviceContext4* context;
	IDXGISwapChain1* swapchain;

	ID3D11Texture2D* framebuffer;
	ID3D11ShaderResourceView* framebuffer_tex_resource;
	ID3D11RenderTargetView* framebuffer_view;
	ID3D11DepthStencilView* depthstencil_view;
	
	ID3D11RenderTargetView* present_buffer_view;
	ID3D11DepthStencilView* present_depth_view;
	
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11RasterizerState2* rasterizer_state;

	ID3D11Buffer* lightBuffer;

	void Init(void* window);
	void Dispose();
	void Render(entt::registry& entitites);
	void Render(const Renderable& renderable);
	void Present();
	void* GetFramebuffer();
};