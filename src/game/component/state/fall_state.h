/*****************************************************************//**
 * @file   fall_state.h
 * @brief  玩家状态-下落
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.11.29
 *********************************************************************/

#pragma once
#ifndef FALL_STATE_H
#define FALL_STATE_H

#include "player_state.h"

namespace game::component::state {
/**
* @brief 玩家跳跃状态类.
*/
class FallState final : public PlayerState {
	friend class game::component::PlayerComponent;
public:
	FallState(PlayerComponent* playerComponent);
	~FallState() override = default;
private:
	// 核心状态方法
	virtual void enter() override;															///< @brief 进入
	virtual void exit() override;															///< @brief 离开
	virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;		///< @brief 处理输入
	virtual std::unique_ptr<PlayerState> update(float, engine::core::Context&) override;	///< @brief 更新

private:
	static constexpr std::string_view mLogTag = "FallState";
};
} // namespace game::component::state

#endif // FALL_STATE_H
