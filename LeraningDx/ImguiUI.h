#pragma once
#include "pch.h"
struct ImguiUI
{
	ImGuiIO* io = nullptr;
	ImguiUI();
	~ImguiUI();
	void Draw(entt::registry& entities, ImTextureID framebuffer);
};

