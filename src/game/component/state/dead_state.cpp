#include "dead_state.h"
#include "../player_component.h"
#include "../../../engine/object/game_object.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/collider_component.h"
#include "../../../engine/component/audio_component.h"

namespace game::component::state {
DeadState::DeadState(PlayerComponent* playerComponent) 
	: PlayerState(playerComponent)
{
}

void DeadState::enter() {
	// 播放进入死亡状态
	spdlog::debug("{} : 玩家进入死亡状态", std::string(mLogTag));
	playAnimation("hurt");

	// 应用击退力, 向上的击退力
	auto physicsComponent = mPlayerComponent->getPhysicsComponent();
	physicsComponent->setVelocity(glm::vec2(0.f, -200.f));

	// 禁用碰撞(自动掉出屏幕)
	auto colliderComponent = mPlayerComponent->getOwner()->getComponent<engine::component::ColliderComponent>();
	if (colliderComponent) {
		colliderComponent->setActive(false);
	}

	if (auto* audioComponent = mPlayerComponent->getAudioComponent(); audioComponent) {
		audioComponent->playSound("dead");
	}
}

void DeadState::exit() {
}

std::unique_ptr<PlayerState> DeadState::handleInput(engine::core::Context&) {
	// 死亡状态下不处理输入
	return nullptr;
}

std::unique_ptr<PlayerState> DeadState::update(float, engine::core::Context&) {
	// 死亡状态下不更新状态
	return nullptr;
}
} // namespace game::component::state
