#include "Game.h"
#include "LearningDxEngine.h"

namespace Engine {

	Game* Game::Get() {
		return mGame;
	}

	PUBLICAPI Game::Game(void* window, uint32_t width, uint32_t height) {
		this->mRenderer = Renderer{ (HWND)window, 1920, 1080 };
		mGame = this;
	}
	PUBLICAPI Game::~Game()
	{
	}
}