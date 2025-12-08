#include "idle_state.h"
#include "walk_state.h"
#include "jump_state.h"
#include "fall_state.h"
#include "climb_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include <spdlog/spdlog.h>

namespace game::component::state {
IdleState::IdleState(PlayerComponent* playerComponent) 
	: PlayerState(playerComponent)
{
}

void IdleState::enter() {
	playAnimation("idle");		// 播放待机动画
}

void IdleState::exit() {

}

std::unique_ptr<PlayerState> IdleState::handleInput(engine::core::Context& context) {
	auto inputManager = context.getInputManager();
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();

	// 如果按下上键, 且与梯子重合, 则切换到ClimbState
	if (physicsComponent->hasCollidedLadder() && inputManager.isActionDown("MoveUp")) {
		return std::make_unique<ClimbState>(mPlayerComponent);
	}

	// 如果按下下键, 则切换到攀爬状态
	if (physicsComponent->isOnTopLadder() && inputManager.isActionDown("MoveDown")) {
		// 需要向下移动一点, 确保下一帧能与梯子碰撞 (否则会切换回下落状态)
		physicsComponent->getTransform()->translate(glm::vec2(0.f, 2.f));
		return std::make_unique<ClimbState>(mPlayerComponent);
	}

	// 如果按下了左右移动键, 则切换到WalkState
	if (inputManager.isActionDown("MoveLeft") || inputManager.isActionDown("MoveRight")) {
		return std::make_unique<WalkState>(mPlayerComponent);
	}
	// 如果按下"Jump"则切换到JumpState
	if (inputManager.isActionPressed("Jump")) {
		return std::make_unique<JumpState>(mPlayerComponent);
	}
	return nullptr;
}

std::unique_ptr<PlayerState> IdleState::update(float, engine::core::Context&) {
	// 应用摩擦力(水平方向)
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	auto frictionFactor = mPlayerComponent->getFrictionFactor();
	auto velocity = physicsComponent->getVelocity();
	velocity.x *= frictionFactor;
	physicsComponent->setVelocity(velocity);

	// 如果离地, 则切换到FallState
	if (!mPlayerComponent->isOnGround()) {
		return std::make_unique<FallState>(mPlayerComponent);
	}
	return nullptr;
}
} // namespace game::component::state
