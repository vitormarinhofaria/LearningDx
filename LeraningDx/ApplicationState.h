#pragma once
#include "pch.h"
#include "Renderer.h"

struct ApplicationState {
	bool running = false;
	HWND window = nullptr;
	IRenderer* renderer = nullptr;
	entt::registry entities{};

	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;

	static ApplicationState& Get() {
		using namespace DirectX::SimpleMath;

		static ApplicationState* g_app_state = nullptr;
		if (!g_app_state) {
			g_app_state = new ApplicationState;
		}
		return *g_app_state;
	}
private:
	ApplicationState(){}
};
