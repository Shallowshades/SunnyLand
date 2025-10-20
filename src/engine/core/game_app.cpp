#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "game_app.h"
#include "time.h"
#include "../resource/resource_manager.h"

engine::core::GameApp::GameApp() = default;

engine::core::GameApp::~GameApp() {
	if (mIsRunning) {
		spdlog::warn("{} 被销毁时没有显式关闭, 正在关闭...", std::string(mLogTag));
		close();
	}
}

void engine::core::GameApp::run() {
	if (!init()) {
		spdlog::error("{} 初始化失败, 无法运行游戏.", std::string(mLogTag));
		return;
	}

	mTime->setTargetFps(144);	// 设置目标帧率,未来从配置文件读取
	while (mIsRunning) {
		mTime->update();
		float delta = mTime->getDeltaTime(); // 每帧的时间间隔
		handleEvents();
		update(delta);
		render();

		// spdlog::info("Delta Time: {}", delta);
	}

	close();
}

bool engine::core::GameApp::init() {
	spdlog::trace("{} 初始化...", std::string(mLogTag));
	
	if (!initSDL()) return false;
	if (!initTime()) return false;
	if (!initResourceManager()) return false;

	// 测试资源管理器
	testResourceManager();

	mIsRunning = true;
	spdlog::trace("{} 初始化成功", std::string(mLogTag));
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
	spdlog::trace("{} 关闭...", std::string(mLogTag));

	// 为了确保正确的销毁顺序, 有些智能指针对象需要手动管理
	mResourceManager.reset();

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

bool engine::core::GameApp::initSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		spdlog::error("{} 无法创建窗口! SDL错误: {}", std::string(mLogTag), SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
	if (mWindow == nullptr) {
		spdlog::error("{} 无法创建窗口! SDL错误: {}", std::string(mLogTag), SDL_GetError());
		return false;
	}

	mSDLRenderer = SDL_CreateRenderer(mWindow, nullptr);
	if (mSDLRenderer == nullptr) {
		spdlog::error("{} 无法创建渲染器! SDL错误: {}", std::string(mLogTag), SDL_GetError());
		return false;
	}
	spdlog::trace("{} 初始化SDL成功", std::string(mLogTag));
	return true;
}

bool engine::core::GameApp::initTime() {
	try {
		mTime = std::make_unique<Time>();
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化时间管理器失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	return true;
}

bool engine::core::GameApp::initResourceManager() {
	try {
		mResourceManager = std::make_unique<engine::resource::ResourceManager>(mSDLRenderer);
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化资源管理器失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 资源管理器成功", std::string(mLogTag));
	return true;
}

void engine::core::GameApp::testResourceManager() {
	mResourceManager->getTexture("assets/textures/Actors/eagle-attack.png");
	mResourceManager->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
	mResourceManager->getSound("assets/audio/button_click.wav");
	mResourceManager->unloadTexture("assets/textures/Actors/eagle-attack.png");
	mResourceManager->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
	mResourceManager->unloadSound("assets/audio/button_click.wav");
}
