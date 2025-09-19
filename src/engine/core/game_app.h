#pragma once

#ifndef GAME_APP_H
#define GAME_APP_H

#include <memory>

// 前向声明, 减少头文件依赖, 增加编译速度
struct SDL_Window;
struct SDL_Renderer;

namespace engine::core {
class Time;
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

private:
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mSDLRenderer = nullptr;
	bool mIsRunning = false;

	// 引擎组件
	std::unique_ptr<engine::core::Time> mTime;
};

}

#endif // !GAME_APP_H
