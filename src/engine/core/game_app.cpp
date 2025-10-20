#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "game_app.h"
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"

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
	if (!initRenderer()) return false;
	if (!initCamera()) return false;

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
	// 游戏逻辑更新
	testCamera();
}

void engine::core::GameApp::render() {
	//1. 清除屏幕
	mRenderer->clearScreen();
	//2. 具体渲染代码
	testRenderer();
	//3. 更新屏幕显示
	mRenderer->present();
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

	// 设置逻辑分辨率
	SDL_SetRenderLogicalPresentation(mSDLRenderer, 640, 360, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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

bool engine::core::GameApp::initRenderer() {
	try {
		mRenderer = std::make_unique<engine::render::Renderer>(mSDLRenderer, mResourceManager.get());
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化渲染器失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 渲染器初始化成功", std::string(mLogTag));
	return true;
}

bool engine::core::GameApp::initCamera() {
	try {
		mCamera = std::make_unique<engine::render::Camera>(glm::vec2(640, 360));
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化相机失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 相机初始化成功", std::string(mLogTag));
	return true;
}

//////////////////////////////////////////////////////////////////////////
/// 测试用函数
//////////////////////////////////////////////////////////////////////////
void engine::core::GameApp::testResourceManager() {
	mResourceManager->getTexture("assets/textures/Actors/eagle-attack.png");
	mResourceManager->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
	mResourceManager->getSound("assets/audio/button_click.wav");
	mResourceManager->unloadTexture("assets/textures/Actors/eagle-attack.png");
	mResourceManager->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
	mResourceManager->unloadSound("assets/audio/button_click.wav");
}

void engine::core::GameApp::testRenderer() {
	engine::render::Sprite spriteWorld("assets/textures/Actors/frog.png");
	engine::render::Sprite spriteUi("assets/textures/UI/buttons/Start1.png");
	engine::render::Sprite spriteParallax("assets/textures/Layers/back.png");

	static float rotation = 0.0f;
	rotation += 0.1f;

	// 注意渲染顺序
	mRenderer->drawParallax(*mCamera, spriteParallax, glm::vec2(100, 100), glm::vec2(0.5f, 0.5f), glm::bvec2(true, false));
	mRenderer->drawSprite(*mCamera, spriteWorld, glm::vec2(200, 200), glm::vec2(1.0f, 1.0f), rotation);
	mRenderer->drawUISprite(spriteUi, glm::vec2(100, 100));

}

void engine::core::GameApp::testCamera() {
	auto key_state = SDL_GetKeyboardState(nullptr);
	if (key_state[SDL_SCANCODE_UP]) mCamera->move(glm::vec2(0, -1));
	if (key_state[SDL_SCANCODE_DOWN]) mCamera->move(glm::vec2(0, 1));
	if (key_state[SDL_SCANCODE_LEFT]) mCamera->move(glm::vec2(-1, 0));
	if (key_state[SDL_SCANCODE_RIGHT]) mCamera->move(glm::vec2(1, 0));
}
