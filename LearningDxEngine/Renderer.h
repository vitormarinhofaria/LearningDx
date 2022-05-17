#pragma once
#include "LearningDxEngine.h"

namespace Engine {
	struct Renderer {
		Renderer();
		Renderer(HWND pWindow, uint32_t width, uint32_t height);
		~Renderer();
		PUBLICAPI void Render();
		void Draw(const Renderable& renderable, const Material& material);
		void* GetFramebuffer();

		ID3D11Device4* Device = nullptr;
		ID3D11DeviceContext* Context = nullptr;
		IDXGISwapChain1* Swapchain = nullptr;

		ID3D11Texture2D* RenderTexture = nullptr;
		ID3D11ShaderResourceView* RenderTextureView = nullptr;
		ID3D11RenderTargetView* RenderTextureTargetView = nullptr;
		ID3D11DepthStencilView* RenderTextureDepthStencilView = nullptr;

		ID3D11RenderTargetView* PresentTargetView = nullptr;
		ID3D11DepthStencilView* PresentDepthStencilView = nullptr;

		ID3D11DepthStencilState* DepthStencilState = nullptr;
		ID3D11RasterizerState2* RasterizerState = nullptr;

		HWND Window = nullptr;
		uint32_t Width = 0;
		uint32_t Height = 0;
	};
}