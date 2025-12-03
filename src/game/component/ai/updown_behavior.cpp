#include "updown_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/object/game_object.h"
#include <spdlog/spdlog.h>

namespace game::component::ai {
UpdownBehavior::UpdownBehavior(float minY, float maxY, float speed) 
	: mPatrolMinY(minY)
	, mPatrolMaxY(maxY)
	, mMoveSpeed(speed)
{
	if (mPatrolMinY >= mPatrolMaxY) {
		spdlog::error("{} : minY {} 应小于 maxY {}. 行为可能不正确.", std::string(mLogTag), mPatrolMinY, mPatrolMaxY);
		std::swap(mPatrolMinY, mPatrolMaxY);
	}
}

void UpdownBehavior::enter(AIComponent & aiComponent) {
	// 播放动画
	if (auto ac = aiComponent.getAnimationComponent(); ac) {
		ac->playAnimation("fly");
	}

	// 禁用重力
	if (auto pc = aiComponent.getPhysicsComponent(); pc) {
		pc->setUseGravity(false);
	}
}

void UpdownBehavior::update(float, AIComponent & aiComponent) {
	// 获取必要组件
	auto pc = aiComponent.getPhysicsComponent();
	auto tc = aiComponent.getTransformComponent();
	if (!pc || !tc) {
		spdlog::error("{} : 缺少必要的组件, 无法执行巡逻行为", std::string(mLogTag));
		return;
	}

	// 检查碰撞和边界
	auto currentY = tc->getPosition().y;
	// 到达上边界或碰撞上方障碍, 向下移动
	if (pc->hasCollidedAbove() || currentY <= mPatrolMinY) {
		pc->setVelocity({ pc->getVelocity().x, mMoveSpeed });
		mMoveDown = true;
	}
	// 到达下边界或碰撞下方障碍, 向下移动
	else if (pc->hasCollidedBelow() || currentY >= mPatrolMaxY) {
		pc->setVelocity({ pc->getVelocity().x, -mMoveSpeed });
		mMoveDown = false;
	}
}
} // namespace game::component::ai
