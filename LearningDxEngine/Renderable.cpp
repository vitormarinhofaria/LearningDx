#include "Renderable.h"
#include "LearningDxEngine.h"
#include "Game.h"
#include "Renderer.h"

namespace Engine {

	PUBLICAPI Renderable::Renderable(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		Renderer& renderer = Game::Get()->mRenderer;
		HRESULT result;
		{
			size_t stride = sizeof(Vertex);
			this->IndexCount = (uint32_t)indices.size();
			auto offset = 0;

			D3D11_BUFFER_DESC vBufferDesc{
				.ByteWidth = (UINT)vertices.size() * (UINT)sizeof(Vertex),
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_VERTEX_BUFFER
			};
			D3D11_SUBRESOURCE_DATA vSubres{ .pSysMem = vertices.data() };
			result = renderer.Device->CreateBuffer(&vBufferDesc, &vSubres, &this->VertexBuffer);
			assert(!FAILED(result));

			D3D11_BUFFER_DESC iBufferDesc{
				.ByteWidth = (UINT)indices.size() * (UINT)sizeof(uint32_t),
				.Usage = D3D11_USAGE_IMMUTABLE,
				.BindFlags = D3D11_BIND_INDEX_BUFFER
			};
			D3D11_SUBRESOURCE_DATA iSubres{ .pSysMem = indices.data() };
			result = renderer.Device->CreateBuffer(&iBufferDesc, &iSubres, &this->IndexBuffer);
		}
	}

	uint32_t Renderable::GetIndexCount() const
	{
		return IndexCount;
	}

}