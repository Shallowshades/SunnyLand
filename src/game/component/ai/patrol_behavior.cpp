#include "patrol_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/object/game_object.h"
#include <spdlog/spdlog.h>

namespace game::component::ai {
PatrolBehavior::PatrolBehavior(float minX, float maxX, float speed) 
	: mPatrolMinX(minX)
	, mPatrolMaxX(maxX)
	, mMoveSpeed(speed)
{
	if (mPatrolMinX >= mPatrolMaxX) {
		spdlog::error("{} : minX {} 应小于 maxX {}. 行为可能不正确.", std::string(mLogTag), mPatrolMinX, mPatrolMaxX);
		std::swap(mPatrolMinX, mPatrolMaxX);
	}
}

void PatrolBehavior::enter(AIComponent& aiComponent) {
	// 播放动画
	if (auto* ac = aiComponent.getAnimationComponent(); ac) {
		ac->playAnimation("walk");
	}
}

void PatrolBehavior::update(float, AIComponent& aiComponent) {
	// 获取必要组件
	auto pc = aiComponent.getPhysicsComponent();
	auto tc = aiComponent.getTransformComponent();
	auto sc = aiComponent.getSpriteComponent();
	if (!pc || !tc || !sc) {
		spdlog::error("{} : 缺少必要组件, 无法执行巡逻行为.", std::string(mLogTag));
		return;
	}

	// 检查碰撞和边界
	auto currentX = tc->getPosition().x;
	// 撞右墙或到达设定目标则转向左
	if (pc->hasCollidedRight() || currentX >= mPatrolMaxX) {
		pc->setVelocity({ -mMoveSpeed, pc->getVelocity().y });
		mMoveRight = false;
	}
	// 撞左墙或到达设定目标则转向右
	else if (pc->hasCollidedLeft() || currentX <= mPatrolMinX) {
		pc->setVelocity({ mMoveSpeed, pc->getVelocity().y });
		mMoveRight = true;
	}

	// 更新精灵翻转
	sc->setFlipped(mMoveRight);
}
} //namespace game::component::ai 
