/*****************************************************************//**
 * @file   jump_state.h
 * @brief  玩家状态-跳跃
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.11.29
 *********************************************************************/

#pragma once
#ifndef JUMP_STATE_H
#define JUMP_STATE_H

#include "player_state.h"

namespace game::component::state {
/**
* @brief 玩家跳跃状态类.
*/
class JumpState final : public PlayerState {
	friend class game::component::PlayerComponent;
public:
	JumpState(PlayerComponent* playerComponent);
	~JumpState() override = default;
private:
	// 核心状态方法
	virtual void enter() override;															///< @brief 进入
	virtual void exit() override;															///< @brief 离开
	virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;		///< @brief 处理输入
	virtual std::unique_ptr<PlayerState> update(float, engine::core::Context&) override;	///< @brief 更新

private:
	static constexpr std::string_view mLogTag = "JumpState";
};
} // namespace game::component::state

#endif // JUMP_STATE_H
