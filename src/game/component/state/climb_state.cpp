#include "climb_state.h"
#include "walk_state.h"
#include "idle_state.h"
#include "jump_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/animation_component.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state {
ClimbState::ClimbState(PlayerComponent* playerComponent) 
	: PlayerState(playerComponent)
{	
}

void ClimbState::enter() {
	spdlog::debug("{} : 进入攀爬状态", std::string(mLogTag));
	playAnimation("climb");
	if (auto pc = mPlayerComponent->getPhysicsComponent(); pc) {
		pc->setUseGravity(false);
	}
}

void ClimbState::exit() {
	spdlog::debug("{} : 退出攀爬状态", std::string(mLogTag));
	if (auto pc = mPlayerComponent->getPhysicsComponent(); pc) {
		pc->setUseGravity(true);
	}
}

std::unique_ptr<PlayerState> ClimbState::handleInput(engine::core::Context& context) {
	auto inputManager = context.getInputManager();
	auto pc = mPlayerComponent->getPhysicsComponent();
	auto ac = mPlayerComponent->getAnimationComponent();

	// 攀爬状态下, 按键则移动, 不按键则静止
	auto isUp = inputManager.isActionDown("MoveUp");
	auto isDown = inputManager.isActionDown("MoveDown");
	auto isLeft = inputManager.isActionDown("MoveLeft");
	auto isRight = inputManager.isActionDown("MoveRight");
	auto speed = mPlayerComponent->getClimbSpeed();

	auto velocity = pc->getVelocity();
	velocity.y = isUp ? -speed : (isDown ? speed : 0.f);
	velocity.x = isLeft ? -speed : (isRight ? speed : 0.f);
	pc->setVelocity(velocity);

	// 根据是否有按键决定动画播放情况
	if (isUp || isDown || isLeft || isRight) {
		ac->resumeAnimation();
	}
	else {
		ac->stopAnimation();
	}

	// 按键跳跃主动离开攀爬状态
	if (inputManager.isActionPressed("jump")) {
		return std::make_unique<JumpState>(mPlayerComponent);
	}
	return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context&) {
	auto pc = mPlayerComponent->getPhysicsComponent();
	// 如果着地, 则切换到空闲状态
	if (pc->hasCollidedBelow()) {
		return std::make_unique<IdleState>(mPlayerComponent);
	}

	// 如果离开梯子区域, 则切换到下落状态
	if (!pc->hasCollidedLadder()) {
		return std::make_unique<FallState>(mPlayerComponent);
	}
	return nullptr;
}
} // namespace game::component::state
