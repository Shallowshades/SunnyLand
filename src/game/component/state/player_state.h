/*****************************************************************//**
 * @file   player_state.h
 * @brief  玩家状态机抽象类
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.11.29
 *********************************************************************/

#pragma once
#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#include <memory>
#include <string_view>

namespace engine::core { class Context; }
namespace game::component { class PlayerComponent; }

namespace game::component::state {
/**
 * @brief 玩家状态机的抽象基类.
 */
class PlayerState {
	friend class game::component::PlayerComponent;
public:
	PlayerState(PlayerComponent* playerComponent) : mPlayerComponent(playerComponent) {}
	virtual ~PlayerState() = default;

	// 禁用拷贝和移动语义
	PlayerState(const PlayerState&) = delete;										///< @brief 删除拷贝构造
	PlayerState& operator=(const PlayerState&) = delete;							///< @brief 删除拷贝赋值构造
	PlayerState(PlayerState&&) = delete;											///< @brief 删除移动构造
	PlayerState& operator=(PlayerState&&) = delete;									///< @brief 删除移动赋值构造

	void playAnimation(const std::string& animationName);							///< @brief 播放指定名称的动画, 使用动画组件的方法

protected:
	// 核心状态方法
	virtual void enter() = 0;														///< @brief 进入
	virtual void exit() = 0;														///< @brief 离开
	virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context&) = 0;	///< @brief 处理输入
	virtual std::unique_ptr<PlayerState> update(float, engine::core::Context&) = 0;	///< @brief 更新
	// handleInput 和 update 返回值为下一个状态, 如果不需要切换状态, 则返回nullptr
protected:
	static constexpr std::string_view mLogTag = "PlayerState";						///< @brief 日志标识
	PlayerComponent* mPlayerComponent = nullptr;									///< @brief 指向拥有此状态的玩家组件
};
}

#endif // !PLAYER_STATE_H

