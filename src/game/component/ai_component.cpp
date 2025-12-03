#include "ai_component.h"
#include "ai/ai_behavior.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include <spdlog/spdlog.h>

namespace game::component {
void AIComponent::setBehavior(std::unique_ptr<ai::AIBehavior> behavior) {
	mCurrentBehavior = std::move(behavior);
	spdlog::debug("{} : 游戏对象 '{}' 上的AI组件设置了新的行为.", std::string(mLogTag), (mOwner ? mOwner->getName() : "unknown"));
	if (mCurrentBehavior) {
		mCurrentBehavior->enter(*this);
	}
}

bool AIComponent::takeDamage(int damage) {
	bool success = false;
	if (auto hc = mOwner->getComponent<engine::component::HealthComponent>(); hc) {
		success = hc->takeDamage(damage);
		// TODO: 设置受伤/死亡后的行为
	}
	return success;
}

bool AIComponent::isAlive() const {
	if (auto* hc = mOwner->getComponent<engine::component::HealthComponent>(); hc) {
		return hc->isAlive();
	}
	return true;
}

engine::component::TransformComponent* AIComponent::getTransformComponent() const {
	return mTransformComponent;
}

engine::component::PhysicsComponent* AIComponent::getPhysicsComponent() const {
	return mPhysicsComponent;
}

engine::component::SpriteComponent* AIComponent::getSpriteComponent() const {
	return mSpriteComponent;
}

engine::component::AnimationComponent* AIComponent::getAnimationComponent() const {
	return mAnimationComponent;
}

void AIComponent::init() {
	if (!mOwner) {
		spdlog::error("{} : 玩家组件没有所属游戏对象", std::string(mLogTag));
		return;
	}

	// 获取并缓存必要的组件指针
	mTransformComponent = mOwner->getComponent<engine::component::TransformComponent>();
	mPhysicsComponent = mOwner->getComponent<engine::component::PhysicsComponent>();
	mSpriteComponent = mOwner->getComponent<engine::component::SpriteComponent>();
	mAnimationComponent = mOwner->getComponent<engine::component::AnimationComponent>();
	
	// 检查必要组件
	if (!mTransformComponent || !mPhysicsComponent || !mSpriteComponent || !mAnimationComponent) {
		spdlog::error("{} : 游戏对象 '{}' 缺少必要组件", std::string(mLogTag), mOwner->getName());
	}
}

void AIComponent::update(float delta, engine::core::Context&) {
	// 将更新委托给当前的行为策略
	if (mCurrentBehavior) {
		mCurrentBehavior->update(delta, *this);
	}
	else {
		spdlog::warn("{} : 游戏对象 '{}' 没有设置AI组件.", std::string(mLogTag), (mOwner ? mOwner->getName() : "unknown"));
	}
}
} // namespace game::component
