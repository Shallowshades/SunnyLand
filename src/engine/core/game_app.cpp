#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "game_app.h"
#include "time.h"
#include "config.h"
#include "context.h"
#include "../resource/resource_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../input/input_manager.h"
#include "../physics/physics_engine.h"
#include "../scene/scene_manager.h"
#include "../../game/scene/game_scene.h"

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


	while (mIsRunning) {
		mTime->update();
		float delta = mTime->getDeltaTime(); // 每帧的时间间隔
		mInputManager->update();
		handleEvents();
		update(delta);
		render();

		// spdlog::info("Delta Time: {}", delta);
	}

	close();
}

bool engine::core::GameApp::init() {
	spdlog::trace("{} 初始化...", std::string(mLogTag));

	if (!initConfig()) return false;
	if (!initSDL()) return false;
	if (!initTime()) return false;
	if (!initResourceManager()) return false;
	if (!initRenderer()) return false;
	if (!initCamera()) return false;
	if (!initInputManager()) return false;
	if (!initPhysicsEngine()) return false;
	if (!initContext()) return false;
	if (!initSceneManager()) return false;

	// 创建第一个场景并压入栈
	auto scene = std::make_unique<game::scene::GameScene>("GameScene", *mContext, *mSceneManager);
	mSceneManager->requestPushScene(std::move(scene));

	mIsRunning = true;
	spdlog::trace("{} 初始化成功", std::string(mLogTag));
	return true;
}

void engine::core::GameApp::handleEvents() {
	if (mInputManager->shouldQuit()) {
		spdlog::trace("{} 收到来自 InputManager 的退出请求.", std::string(mLogTag));
		mIsRunning = false;
		return;
	}

	mSceneManager->handleInput();
}

void engine::core::GameApp::update(float delta) {
	// 游戏逻辑更新
	mSceneManager->update(delta);
}

void engine::core::GameApp::render() {
	//1. 清除屏幕
	mRenderer->clearScreen();
	//2. 具体渲染代码
	mSceneManager->render();
	//3. 更新屏幕显示
	mRenderer->present();
}

void engine::core::GameApp::close() {
	spdlog::trace("{} 关闭...", std::string(mLogTag));

	// 先关闭场景管理器, 确保所有场景都被清理
	mSceneManager->clean();

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

bool engine::core::GameApp::initConfig() {
	try {
		mConfig = std::make_unique<engine::core::Config>("assets/config.json");
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化配置失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 配置初始化成功", std::string(mLogTag));
	return true;
}

bool engine::core::GameApp::initSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		spdlog::error("{} 无法创建窗口! SDL错误: {}", std::string(mLogTag), SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(mConfig->mWindowTitle.c_str(), mConfig->mWindowWidth, mConfig->mWindowHeight, SDL_WINDOW_RESIZABLE);
	if (mWindow == nullptr) {
		spdlog::error("{} 无法创建窗口! SDL错误: {}", std::string(mLogTag), SDL_GetError());
		return false;
	}

	mSDLRenderer = SDL_CreateRenderer(mWindow, nullptr);
	if (mSDLRenderer == nullptr) {
		spdlog::error("{} 无法创建渲染器! SDL错误: {}", std::string(mLogTag), SDL_GetError());
		return false;
	}

	// 设置VSync(注意:VSync开启时, 驱动程序会尝试将帧率限制到显示器刷新率, 有可能会覆盖手动设置的mTargetFps)
	int vsyncMode = mConfig->mVsyncEnabled ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
	SDL_SetRenderVSync(mSDLRenderer, vsyncMode);
	spdlog::trace("{} Vsync设置为: {}", std::string(mLogTag), mConfig->mVsyncEnabled ? "Enable" : "Disable");

	// 设置逻辑分辨率为窗口大小的一半 (针对像素游戏)
	SDL_SetRenderLogicalPresentation(mSDLRenderer, mConfig->mWindowWidth / 2, mConfig->mWindowHeight / 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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
	mTime->setTargetFps(mConfig->mTargetFps);
	spdlog::trace("{} 时间管理初始化成功", std::string(mLogTag));
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
		mCamera = std::make_unique<engine::render::Camera>(glm::vec2(mConfig->mWindowWidth / 2, mConfig->mWindowHeight / 2));
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化相机失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 相机初始化成功", std::string(mLogTag));
	return true;
}

bool engine::core::GameApp::initInputManager() {
	try {
		mInputManager = std::make_unique<engine::input::InputManager>(mSDLRenderer, mConfig.get());
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化输入管理器失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 输入管理器初始化成功", std::string(mLogTag));
	return true;
}

bool engine::core::GameApp::initPhysicsEngine() {
	try {
		mPhysicsEngine = std::make_unique<engine::physics::PhysicsEngine>();
	}
	catch (const std::exception& e) {
		spdlog::error("{} : 初始化物理引擎失败 : {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} : 物理引擎初始化成功.", std::string(mLogTag));
	return true;
}

bool engine::core::GameApp::initContext() {
	try {
		mContext = std::make_unique<engine::core::Context>(*mInputManager, *mRenderer, *mCamera, *mResourceManager, *mPhysicsEngine);
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化上下文失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	return true;
}

bool engine::core::GameApp::initSceneManager() {
	try {
		mSceneManager = std::make_unique<engine::scene::SceneManager>(*mContext);
	}
	catch (const std::exception& e) {
		spdlog::error("{} 初始化场景管理器失败: {}", std::string(mLogTag), e.what());
		return false;
	}
	spdlog::trace("{} 场景管理器初始化成功.", std::string(mLogTag));
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

void engine::core::GameApp::testInputManager() {
	std::vector<std::string> actions = {
		"MoveUp",
		"MoveDown",
		"MoveLeft",
		"MoveRight",
		"Jump",
		"Attack",
		"Pause",
		"MouseLeftClick",
		"MouseRightClick"
	};

	for (const auto& action : actions) {
		if (mInputManager->isActionPressed(action)) {
			spdlog::info("{} 按下 {}", std::string(mLogTag), action);
		}
		if (mInputManager->isActionReleased(action)) {
			spdlog::info("{} 抬起 {}", std::string(mLogTag), action);
		}
		if (mInputManager->isActionDown(action)) {
			spdlog::info("{} 按下中 {}", std::string(mLogTag), action);
		}
	}
}

void engine::core::GameApp::testGameObject() {
	// gameObject.addComponent<engine::component::TransformComponent>(glm::vec2(100, 100));
	// gameObject.addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", *mResourceManager, engine::utils::Alignment::CENTER);
	// gameObject.getComponent<engine::component::TransformComponent>()->setScale(glm::vec2(2.f, 2.f));
	// gameObject.getComponent<engine::component::TransformComponent>()->setRotation(30.f);*/
}
