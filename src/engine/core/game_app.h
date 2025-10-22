#pragma once

#ifndef GAME_APP_H
#define GAME_APP_H

#include <memory>

// 前向声明, 减少头文件依赖, 增加编译速度
struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Renderer;
class Camera;
}

namespace engine::input {
class InputManager;
}

namespace engine::core {
class Time;
class Config;
/**
 * @brief 主游戏应用程序类, 初始化SDL, 管理游戏循环
 */
class GameApp final {	// final表示不能被继承
public:
	GameApp();
	~GameApp();

	/**
	 * @brief 运行游戏应用程序,其中会调用init(),然后进入主循环,离开循环后自动调用close().
	 */
	void run();

	// 禁止拷贝和移动构造
	GameApp(const GameApp&) = delete;
	GameApp& operator=(const GameApp&) = delete;
	GameApp(GameApp&&) = delete;
	GameApp& operator=(GameApp&&) = delete;

private:
	[[nodiscard]] bool init();	// nodiscord表示函数返回值不应该被忽略
	void handleEvents();
	void update(float delta);
	void render();
	void close();

	// 各模块的初始化/创建函数, 在init()中调用
	[[nodiscard]] bool initConfig();
	[[nodiscard]] bool initSDL();
	[[nodiscard]] bool initTime();
	[[nodiscard]] bool initResourceManager();
	[[nodiscard]] bool initRenderer();
	[[nodiscard]] bool initCamera();
	[[nodiscard]] bool initInputManager();

	// 测试函数
	void testResourceManager();
	void testRenderer();
	void testCamera();
	void testInputManager();
	void testGameObject();

private:
	static constexpr std::string_view mLogTag = "GameApp";

	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mSDLRenderer = nullptr;
	bool mIsRunning = false;

	// 引擎组件
	std::unique_ptr<engine::core::Time> mTime;
	std::unique_ptr<engine::core::Config> mConfig;
	std::unique_ptr<engine::resource::ResourceManager> mResourceManager;
	std::unique_ptr<engine::render::Renderer> mRenderer;
	std::unique_ptr<engine::render::Camera> mCamera;
	std::unique_ptr<engine::input::InputManager> mInputManager;
};

}

#endif // !GAME_APP_H
