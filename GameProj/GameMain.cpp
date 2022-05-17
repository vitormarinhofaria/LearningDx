#include "LearningDxEngine.h"
#include "Game.h"
#include "Renderer.h"
#include <Windows.h>
#include "fmt/core.h"
#include "entt/entt.hpp"
#include "Components.h"
#include "Renderable.h"
#include "Material.h"
#include "string"

class MyGame : public Engine::Game {
public:
	MyGame(HWND window, uint32_t width, uint32_t height) :Engine::Game(window, width, height) {

	}
	// Inherited via Game
	virtual void Update(double deltaTime) override
	{
		fmt::print("tá rodando cara, vai dormir\n");
		fmt::print("Uhuulll\n");
	}
	virtual void Init() override
	{
		mEntities = entt::registry();
		auto camera = mEntities.create();
		mEntities.emplace<Engine::Camera>(camera);

		std::vector<Engine::Vertex> vertices = {
			Engine::Vertex{{-0.5, 0.5, 0.4}, {0.2, 0.2, 0.2}, {0.2, 0.2 }, {1.0, 1.0, 1.0 }},
			Engine::Vertex{{-0.5, 0.5, 0.4}, {0.2, 0.2, 0.2}, {0.2, 0.2 }, {1.0, 1.0, 1.0 }},
			Engine::Vertex{{-0.5, 0.5, 0.4}, {0.2, 0.2, 0.2}, {0.2, 0.2 }, {1.0, 1.0, 1.0 }},
			Engine::Vertex{{-0.5, 0.5, 0.4}, {0.2, 0.2, 0.2}, {0.2, 0.2 }, {1.0, 1.0, 1.0 }},
		};
		std::vector<uint32_t> indices = { 0, 1, 2, 3 };
		auto e1 = mEntities.create();
		mEntities.emplace<Engine::Renderable>(e1, vertices, indices);
		auto material = Engine::Material(L"assets/shaders/NewVertexShader.cso", L"assets/shaders/NewPixelShader.cso");
		material.Textures.push_back(Engine::Texture("assets/momoko.png"));
		mEntities.emplace<Engine::Material>(e1, material);

	}
};

extern "C" _declspec(dllexport) Engine::Game * RegisterGameClass(void* window) {
	return new MyGame((HWND)window, 1920, 1080);
}