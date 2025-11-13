/*****************************************************************//**
 * @file   physics_engine.h
 * @brief  物理引擎
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.11.13
 *********************************************************************/

#pragma once
#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <string>
#include <vector>
#include <glm/vec2.hpp>

namespace engine::component { class PhysicsComponent; }

namespace engine::physics {
/**
* @brief 负责管理和模拟物理行为及碰撞检测.
*/
class PhysicsEngine {
public:
	PhysicsEngine() = default;													///< @brief 默认构造

	// 禁止拷贝和移动
	PhysicsEngine(const PhysicsEngine&) = delete;								///< @brief 删除拷贝构造
	PhysicsEngine& operator=(const PhysicsEngine&) = delete;					///< @brief 删除拷贝赋值构造
	PhysicsEngine(PhysicsEngine&&) = delete;									///< @brief 删除移动构造
	PhysicsEngine& operator=(PhysicsEngine&&) = delete;							///< @brief 删除移动赋值构造

	void setGravity(const glm::vec2& gravity);									///< @brief 设置重力
	const glm::vec2& getGravity() const;										///< @brief 获取重力
	void setMaxSpeed(float maxSpeed);											///< @brief 设置最大速度
	float getMaxSpeed() const;													///< @brief 获取最大速度

	void registerComponent(engine::component::PhysicsComponent* component);		///< @brief 注册组件
	void unregisterComponent(engine::component::PhysicsComponent* component);	///< @brief 移除组件

	void update(float delta);													///< @brief 更新

private:
	static constexpr std::string_view mLogTag = "PhysicsEngine";				///< @brief 日志标识
																				
	std::vector<engine::component::PhysicsComponent*> mComponents;				///< @brief 在物理系统中注册的组件数组
	glm::vec2 mGravity = { 0.f, 980.f };										///< @brief 重力加速度
	float mMaxSpeed = 500.f;													///< @brief 最大速度限值
};
}

#endif // PHYSICS_ENGINE_H
