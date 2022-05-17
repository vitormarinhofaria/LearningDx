#pragma once
struct Vec3 {
	float x;
	float y;
	float z;
};

struct Vec2 {
	float x;
	float y;
};

struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};