/*****************************************************************//**
 * @file   dead_state.h
 * @brief  玩家状态-死亡
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.12.01
 *********************************************************************/

#pragma once
#ifndef DEAD_STATE_H
#define DEAD_STATE_H

#include "player_state.h"

namespace game::component::state {
/**
* @brief 玩家受伤状态类.
*/
class DeadState final : public PlayerState {
	friend class game::component::PlayerComponent;
public:
	DeadState(PlayerComponent* playerComponent);
	~DeadState() override = default;
private:
	// 核心状态方法
	virtual void enter() override;															///< @brief 进入
	virtual void exit() override;															///< @brief 离开
	virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;		///< @brief 处理输入
	virtual std::unique_ptr<PlayerState> update(float, engine::core::Context&) override;	///< @brief 更新																///< @brief 僵直时间
};
} // namespace game::component::state

#endif // DEAD_STATE_H
