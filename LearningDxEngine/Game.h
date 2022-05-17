#pragma once
#include "LearningDxEngine.h"
#include "Renderer.h"

namespace Engine {
	class Game {
	public:
		static Game* Get();

		PUBLICAPI Game(void* window, uint32_t width, uint32_t height);
		PUBLICAPI ~Game();

		Renderer mRenderer;
		entt::registry mEntities;

		virtual void Update(double deltaTime) = 0;
		virtual void Init() = 0;
	};
}
namespace {
	static Engine::Game* mGame;
}