#include <utility>
#include <typeinfo>
#include <spdlog/spdlog.h>
#include "player_component.h"
#include "state/idle_state.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/animation_component.h"
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

void PlayerComponent::setJumpForce(float jumpForce) {
	mJumpForce = jumpForce;
}

float PlayerComponent::getJumpForce() const {
	return mJumpForce;
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
	if (!mTransformComponent || !mPhysicsComponent || !mSpriteComponent || !mAnimationComponent) {
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
