#pragma once
#include "pch.h"
using namespace DirectX::SimpleMath;
struct Transform
{
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Vector3 scale = { 1.0f, 1.0f, 1.0f };

	Transform() {};
	Transform(Vector3 p_pos, Vector3 p_rot, Vector3 p_scale) : position(p_pos), rotation(p_rot), scale(p_scale) {};
};

