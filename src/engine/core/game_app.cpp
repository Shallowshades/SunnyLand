#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "game_app.h"

engine::core::GameApp::GameApp() = default;

engine::core::GameApp::~GameApp() {
	if (mIsRunning) {
		spdlog::warn("GameApp被销毁时没有显式关闭, 正在关闭...");
		close();
	}
}

void engine::core::GameApp::run() {
	if (!init()) {
		spdlog::error("初始化失败, 无法运行游戏.");
		return;
	}

	while (mIsRunning) {
		float delta = 0.01f; // 每帧的时间间隔
		handleEvents();
		update(delta);
		render();
	}

	close();
}

bool engine::core::GameApp::init() {
	spdlog::trace("初始化GameApp...");
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		spdlog::error("SDL 初始化失败! SDL错误: {}", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
	if (mWindow == nullptr) {
		spdlog::error("无法创建窗口! SDL错误: {}", SDL_GetError());
		return false;
	}

	mSDLRenderer = SDL_CreateRenderer(mWindow, nullptr);
	if (mSDLRenderer == nullptr) {
		spdlog::error("无法创建渲染器! SDL错误: {}", SDL_GetError());
		return false;
	}

	mIsRunning = true;
	return true;
}

void engine::core::GameApp::handleEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			mIsRunning = false;
		}
	}
}

void engine::core::GameApp::update([[maybe_unused]] float delta) {

}

void engine::core::GameApp::render() {

}

void engine::core::GameApp::close() {
	spdlog::trace("关闭GameApp...");
	if (mSDLRenderer != nullptr) {
		SDL_DestroyRenderer(mSDLRenderer);
		mSDLRenderer = nullptr;
	}
	if (mWindow != nullptr) {
		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}
	SDL_Quit();
	mIsRunning = false;
}
