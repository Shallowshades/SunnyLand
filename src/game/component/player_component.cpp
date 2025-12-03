#include <utility>
#include <typeinfo>
#include <spdlog/spdlog.h>
#include "player_component.h"
#include "state/idle_state.h"
#include "state/hurt_state.h"
#include "state/dead_state.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/object/game_object.h"
#include "../../engine/input/input_manager.h"

namespace game::component {
engine::component::TransformComponent* PlayerComponent::getTransformComponent() const {
	return mTransformComponent;
}

engine::component::SpriteComponent* PlayerComponent::getSpriteComponent() const {
	return mSpriteComponent;
}

engine::component::PhysicsComponent* PlayerComponent::getPhysicsComponent() const {
	return mPhysicsComponent;
}

engine::component::AnimationComponent* PlayerComponent::getAnimationComponent() const {
	return mAnimationComponent;
}

engine::component::HealthComponent* PlayerComponent::getHealthComponent() const {
	return mHealthComponent;
}

void PlayerComponent::setIsDead(bool isDead) {
	mIsDead = isDead;
}

bool PlayerComponent::getIsDead() const {
	return mIsDead;
}

void PlayerComponent::setMoveForce(float moveForce) {
	mMoveForce = moveForce;
}

float PlayerComponent::getMoveForce() const {
	return mMoveForce;
}

void PlayerComponent::setMaxSpeed(float maxSpeed) {
	mMaxSpeed = maxSpeed;
}

float PlayerComponent::getMaxSpeed() const {
	return mMaxSpeed;
}

void PlayerComponent::setFrictionFactor(float frictionFactor) {
	mFrictionFactor = frictionFactor;
}

float PlayerComponent::getFrictionFactor() const {
	return mFrictionFactor;
}

void PlayerComponent::setJumpVelocity(float jumpForce) {
	mJumpVelocity = jumpForce;
}

float PlayerComponent::getJumpVelocity() const {
	return mJumpVelocity;
}

void PlayerComponent::setStunnedDuration(float duration) {
	mStunnedDuration = duration;
}

float PlayerComponent::getStunnedDuration() const {
	return mStunnedDuration;
}

void PlayerComponent::setState(std::unique_ptr<state::PlayerState> newState) {
	if (!newState) {
		spdlog::warn("{} : 尝试设置空的玩家状态", std::string(mLogTag));
		return;
	}

	if (mCurrentState) {
		mCurrentState->exit();
	}

	mCurrentState = std::move(newState);
	spdlog::trace("{} : 玩家组件正在切换到状态: {}", std::string(mLogTag), typeid(mCurrentState).name());
	mCurrentState->enter();
}

bool PlayerComponent::takeDamage(int damageAmount) {
	if (mIsDead || !mHealthComponent || damageAmount <= 0) {
		spdlog::warn("{} : 玩家已死亡或缺少必要组件, 并未造成伤害.", std::string(mLogTag));
		return false;
	}

	bool success = mHealthComponent->takeDamage(damageAmount);
	if (!success) {
		return false;
	}

	// 成功造成伤害, 根据是否存活决定状态切换
	if (mHealthComponent->isAlive()) {
		spdlog::debug("{} : 玩家收到了 {} 点伤害, 当前生命值: {}/{}.", std::string(mLogTag), damageAmount, mHealthComponent->getCurrentHealth(), mHealthComponent->getMaxHealth());
		setState(std::make_unique<state::HurtState>(this));
	}
	else {
		spdlog::debug("{} : 玩家死亡.", std::string(mLogTag));
		mIsDead = true;
		setState(std::make_unique<state::DeadState>(this));
	}

	return true;
}

void PlayerComponent::init() {
	if (!mOwner) {
		spdlog::error("{} : 没有所属游戏对象", std::string(mLogTag));
		return;
	}

	// 获取必要组件
	mTransformComponent = mOwner->getComponent<engine::component::TransformComponent>();
	mPhysicsComponent = mOwner->getComponent<engine::component::PhysicsComponent>();
	mSpriteComponent = mOwner->getComponent<engine::component::SpriteComponent>();
	mAnimationComponent = mOwner->getComponent<engine::component::AnimationComponent>();
	mHealthComponent = mOwner->getComponent<engine::component::HealthComponent>();
	if (!mTransformComponent || !mPhysicsComponent || !mSpriteComponent || !mAnimationComponent || !mHealthComponent) {
		spdlog::error("{} : 对象缺少必要组件", std::string(mLogTag));
	}

	mCurrentState = std::make_unique<state::IdleState>(this);
	if (mCurrentState) {
		setState(std::move(mCurrentState));
	}
	else {
		spdlog::error("{} : 初始化玩家状态失败", std::string(mLogTag));
	}
	spdlog::trace("{} : 初始化完成", std::string(mLogTag));
}

void PlayerComponent::handleInput(engine::core::Context& context) {
	if (!mCurrentState) {
		return;
	}

	auto nextState = mCurrentState->handleInput(context);
	if (nextState) {
		setState(std::move(nextState));
	}
}

void PlayerComponent::update(float delta, engine::core::Context& context) {
	if (!mCurrentState) {
		return;
	}

	auto nextState = mCurrentState->update(delta, context);
	if (nextState) {
		setState(std::move(nextState));
	}
}

}
