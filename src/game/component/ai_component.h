/*****************************************************************//**
 * @file   ai_component.h
 * @brief  AI组件
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.12.02
 *********************************************************************/

#pragma once
#include "../../engine/component/component.h"
#include "ai/ai_behavior.h"
#include <memory>
#include <string_view>

namespace game::component::ai { class AIBehavior; }
namespace engine::component {
	class TransformComponent;
	class PhysicsComponent;
	class SpriteComponent;
	class AnimationComponent;
}

namespace game::component {

/**
* @brief 负责管理游戏对象的AI对象
*
* 使用策略模式, 持有一个具体的AI行为对象实例来执行AI逻辑
* 提供对游戏对象其他关键组件的访问
*/
class AIComponent final : public engine::component::Component {
	friend class engine::object::GameObject;
public:
	AIComponent() = default;														///< @brief 
	~AIComponent() override = default;												///< @brief 

	// 禁用拷贝和移动语义
	AIComponent(const AIComponent&) = delete;										///< @brief 删除拷贝构造
	AIComponent& operator=(const AIComponent&) = delete;							///< @brief 删除拷贝赋值构造
	AIComponent(AIComponent&&) = delete;											///< @brief 删除移动构造
	AIComponent& operator=(AIComponent&&) = delete;									///< @brief 删除移动赋值构造

	void setBehavior(std::unique_ptr<ai::AIBehavior> behavior);						///< @brief 设置当前AI行为策略
	bool takeDamage(int damage);													///< @brief 处理伤害逻辑
	bool isAlive() const;															///< @brief 检测对象是否存活

	engine::component::TransformComponent* getTransformComponent() const;			///< @brief 获取变换组件
	engine::component::PhysicsComponent* getPhysicsComponent() const;				///< @brief 获取物理组件
	engine::component::SpriteComponent* getSpriteComponent() const;					///< @brief 获取精灵组件
	engine::component::AnimationComponent* getAnimationComponent() const;			///< @brief 获取动画组件

protected:
	// 核心循环方法
	void init() override;															///< @brief 初始化
	void update(float delta, engine::core::Context&) override;						///< @brief 更新
private:
	static constexpr std::string_view mLogTag = "AIComponent";						///< @brief 日志标识
	std::unique_ptr<ai::AIBehavior> mCurrentBehavior = nullptr;						///< @brief 当前AI行为策略
	// TODO: 未来可添加一些敌人的属性

	// 缓存组件指针
	engine::component::TransformComponent* mTransformComponent = nullptr;			///< @brief 变换组件
	engine::component::PhysicsComponent* mPhysicsComponent = nullptr;				///< @brief 物理组件
	engine::component::SpriteComponent* mSpriteComponent = nullptr;					///< @brief 精灵组件
	engine::component::AnimationComponent* mAnimationComponent = nullptr;			///< @brief 动画组件
};
}