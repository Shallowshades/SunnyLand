#include "jump_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/audio_component.h"
#include "../../../engine/object/game_object.h"
#include <spdlog/spdlog.h>

namespace game::component::ai {
JumpBehavior::JumpBehavior(float minX, float maxX, glm::vec2 jumpVelocity, float jumpInterval) 
	: mPatrolMinX(minX)
	, mPatrolMaxX(maxX)
	, mJumpVelocity(jumpVelocity)
	, mJumpInterval(jumpInterval)
{
	// 确保巡逻范围是有效的
	if (mPatrolMinX >= mPatrolMaxX) {
		spdlog::error("{} : JumpBehavior: minX({})应小于maxX({}). 行为可能不正确", std::string(mLogTag), mPatrolMinX, mPatrolMaxX);
		std::swap(mPatrolMinX, mPatrolMaxX);
	}

	// 确保跳跃间隔是正数
	if (mJumpInterval <= 0.f) {
		spdlog::error("{} : JumpBehavior: JumpInterval = '{}'应为正数. 已设置为2.0s", std::string(mLogTag), mJumpInterval);
		mJumpInterval = 2.f;
	}

	// 确保垂直跳跃速度是负数(向上)
	if (mJumpVelocity.y > 0.f) {
		spdlog::error("{} : JumpBehavior: 垂直跳跃速度({})应为负数(向上). 已取相反数", std::string(mLogTag), mJumpVelocity.y);
		mJumpVelocity.y = -mJumpVelocity.y;
	}
}

void JumpBehavior::update(float delta, AIComponent& aiComponent) {
	// 获取必要的组件
	auto transformComponent = aiComponent.getTransformComponent();
	auto physicsComponent = aiComponent.getPhysicsComponent();
	auto spriteComponent = aiComponent.getSpriteComponent();
	auto animationComponent = aiComponent.getAnimationComponent();
	auto audioComponent = aiComponent.getAudioComponent();

	if (!physicsComponent || !transformComponent || !spriteComponent || !animationComponent) {
		spdlog::error("{} : 缺少必要组件, 无法执行跳跃行为", std::string(mLogTag));
		return;
	}

	// 着地标志
	auto isOnGround = physicsComponent->hasCollidedBelow();
	if (isOnGround) {
		// 刚刚落地, 进入idle状态, 如果有音频组件, 播放音效
		if (audioComponent && mJumpTimer < 0.001f) {
			audioComponent->playSound("cry", -1, true);
		}

		mJumpTimer += delta;
		physicsComponent->setVelocity({ 0.f, physicsComponent->getVelocity().y });
		
		if (mJumpTimer >= mJumpInterval) {
			// 重置计时器
			mJumpTimer = 0.f;
			// 检查是否需要更新跳跃方向
			auto currentX = transformComponent->getPosition().x;
			// 如果右边超限或者撞墙, 向左跳
			if (mJumpRight && (physicsComponent->hasCollidedRight() || currentX >= mPatrolMaxX)) {
				mJumpRight = false;
			}
			// 如果左边超限或者撞墙, 向右跳
			else if (!mJumpRight && (physicsComponent->hasCollidedLeft() || currentX <= mPatrolMinX)) {
				mJumpRight = true;
			}
			auto jumpVelocityX = mJumpRight ? mJumpVelocity.x : -mJumpVelocity.x;
			physicsComponent->setVelocity({ jumpVelocityX, mJumpVelocity.y });
			animationComponent->playAnimation("jump");
			spriteComponent->setFlipped(mJumpRight);
		}
		else {
			animationComponent->playAnimation("idle");
		}
	}
	else {
		if (physicsComponent->getVelocity().y < 0) {
			animationComponent->playAnimation("jump");
		}
		else {
			animationComponent->playAnimation("fall");
		}
	}
}
} // namespace game::component::ai
