#pragma once
#include "LearningDxEngine.h"

namespace Engine {
	using namespace DirectX::SimpleMath;

	struct Vertex {
		Vector3 Pos;
		Vector3 Normal;
		Vector2 UV;
		Vector3 Color;
	};

	class Renderable {
	public:
		PUBLICAPI Renderable(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;
		uint32_t GetIndexCount() const;

	private:
		uint32_t IndexCount;
	};
}