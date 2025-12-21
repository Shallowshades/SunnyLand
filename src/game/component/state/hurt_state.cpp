#include "hurt_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/audio_component.h"
#include <glm/common.hpp>

namespace game::component::state {
HurtState::HurtState(PlayerComponent* playerComponent) 
	: PlayerState(playerComponent)
{
}

void HurtState::enter() {
	// 播放受伤动画
	playAnimation("hurt");
	// 造成击退效果, 默认击退方向-左上方
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	auto spriteComponent = mPlayerComponent->getSpriteComponent();
	auto knockbackVelocity = glm::vec2(-100.f, -150.f);
	// 根据当前精灵的朝向状态是否改为右上方
	if (spriteComponent->isFlipped()) {
		knockbackVelocity.x = -knockbackVelocity.x;
	}
	physicsComponent->setVelocity(knockbackVelocity);

	if (auto* audioComponent = mPlayerComponent->getAudioComponent(); audioComponent) {
		audioComponent->playSound("hurt");
	}
}

void HurtState::exit() {

}

std::unique_ptr<PlayerState> HurtState::handleInput(engine::core::Context&) {
	// 硬直期间不能进行任何操作
	return nullptr;
}

std::unique_ptr<PlayerState> HurtState::update(float delta, engine::core::Context&) {
	mStunnedTimer += delta;
	// 两种情况离开受伤硬直状态
	// 1. 落地
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	if (physicsComponent->hasCollidedBelow()) {
		if (glm::abs(physicsComponent->getVelocity().x) < 1.f) {
			return std::make_unique<IdleState>(mPlayerComponent);
		}
		else {
			return std::make_unique<WalkState>(mPlayerComponent);
		}
	}
	// 2. 硬直时间结束(能走到这里说明没有落地, 直接切换到FallState)
	if (mStunnedTimer > mPlayerComponent->getStunnedDuration()) {
		mStunnedTimer = 0.f;
		return std::make_unique<FallState>(mPlayerComponent);
	}

	return nullptr;
}
} // namespace game::component::state
