/*****************************************************************//**
 * @file   climb_state.h
 * @brief  攀爬状态
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.12.03
 *********************************************************************/

#pragma once
#ifndef CLIMB_STATE_H
#define CLIMB_STATE_H

#include "player_state.h"
#include <string_view>

namespace game::component::state {
class ClimbState : public PlayerState {
	friend class game::component::PlayerComponent;
public:
	ClimbState(PlayerComponent* playerComponent);
	~ClimbState() override = default;
private:
	void enter() override;
	void exit() override;
	std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
	std::unique_ptr<PlayerState> update(float, engine::core::Context&) override;
private:
	static constexpr std::string_view mLogTag = "ClimbState";
};
} // namespace game::component::state

#endif // !CLIMB_STATE_H

