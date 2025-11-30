#include "player_state.h"
#include "../player_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/object/game_object.h"
#include <spdlog/spdlog.h>

namespace game::component::state {
void PlayerState::playAnimation(const std::string& animationName) {
	if (!mPlayerComponent) {
		spdlog::error("{} : 玩家状态没有关联玩家组件, 无法播放动画 '{}'", std::string(mLogTag), animationName);
		return;
	}

	auto ac = mPlayerComponent->getAnimationComponent();
	if (!ac) {
		spdlog::error("{} : 玩家组件 '{}' 没有 动画组件, 无法播放动画 '{}'", std::string(mLogTag), mPlayerComponent->getOwner()->getName(), animationName);
		return;
	}
	ac->playAnimation(animationName);
}
} // namespace game::component::state
