#pragma once

#ifdef _DEBUG
#ifdef _EXPORT
#define PUBLICAPI __declspec(dllexport)
#else
#define PUBLICAPI __declspec(dllimport)
#endif
#endif

namespace Engine {
	class Game;

	struct MaterialCBuffer;

	class Material;

	struct Vertex;

	class Renderable;

	struct Renderer;

	class Texture;
}

#include <Windows.h>
#include <d3d11_4.h>
#include <cstdint>
#include <string>
#include <vector>
#include <assert.h>

#include "entt/entt.hpp"
#include "fmt/core.h"
#include "imgui.h"

#include "DirectXTK/Audio.h"
#include "DirectXTK/SimpleMath.h"
#include "DirectXTK/GamePad.h"
#include "DirectXTK/Keyboard.h"
#include "DirectXTK/Mouse.h"
