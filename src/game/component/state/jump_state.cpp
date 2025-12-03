#include "jump_state.h"
#include "walk_state.h"
#include "idle_state.h"
#include "fall_state.h"
#include "climb_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state {
JumpState::JumpState(PlayerComponent* playerComponent)
	: PlayerState(playerComponent)
{
}

void JumpState::enter() {
	playAnimation("jump");		// 播放跳跃动画
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	auto velocity = physicsComponent->getVelocity();
	velocity.y = -mPlayerComponent->getJumpVelocity();
	physicsComponent->setVelocity(velocity);

	spdlog::debug("{} : 玩家组件进入JumpState, 设置初始垂直速度为: {}", std::string(mLogTag), velocity.y);
}

void JumpState::exit() {

}

std::unique_ptr<PlayerState> JumpState::handleInput(engine::core::Context& context) {
	auto inputManager = context.getInputManager();
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	auto spriteComponent = mPlayerComponent->getSpriteComponent();

	// 如果按下上下键, 且与梯子重合, 则切换到ClimbState
	if (physicsComponent->hasCollidedLadder() && (inputManager.isActionDown("MoveDown") || inputManager.isActionDown("MoveUp"))) {
		return std::make_unique<ClimbState>(mPlayerComponent);
	}

	// 跳跃状态下可以左右移动
	auto velocity = physicsComponent->getVelocity();
	if (inputManager.isActionDown("MoveLeft")) {
		if (velocity.x > 0.f) {
			// 突然反向移动, 先减速到0, 增强手感
			physicsComponent->setVelocity(glm::vec2(0.f, velocity.y));
		}
		// 添加向左的水平力
		physicsComponent->addForce(glm::vec2(-mPlayerComponent->getMoveForce(), 0.f));
		spriteComponent->setFlipped(true);
	}
	else if (inputManager.isActionDown("MoveRight")) {
		if (velocity.x < 0.f) {
			// 突然反向移动, 先减速到0, 增强手感
			physicsComponent->setVelocity(glm::vec2(0.f, velocity.y));
		}
		// 添加向右的水平力
		physicsComponent->addForce(glm::vec2(mPlayerComponent->getMoveForce(), 0.f));
		spriteComponent->setFlipped(false);
	}
	return nullptr;
}

std::unique_ptr<PlayerState> JumpState::update(float, engine::core::Context&) {
	// 限制最大速度
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	auto maxSpeed = mPlayerComponent->getMaxSpeed();
	auto velocity = physicsComponent->getVelocity();
	velocity.x = glm::clamp(velocity.x, -maxSpeed, maxSpeed);
	physicsComponent->setVelocity(velocity);

	// 如果速度为正, 则切换到 FallState
	if (velocity.y >= 0.f) {
		return std::make_unique<FallState>(mPlayerComponent);
	}
	return nullptr;
}
} // namespace game::component::state
