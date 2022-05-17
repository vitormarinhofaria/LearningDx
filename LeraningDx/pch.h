#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#endif

#include <memory>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <assert.h>

#include <fmt/core.h>

#include <entt/entt.hpp>

#include <DirectXTK/GamePad.h>
#include <DirectXTK/Mouse.h>
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/GeometricPrimitive.h>
#include <DirectXTK/Model.h>
#include <DirectXMath.h>
#include <DirectXTK/SimpleMath.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>