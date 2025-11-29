/*****************************************************************//**
 * @file   player_component.h
 * @brief  玩家类组件
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.11.28
 *********************************************************************/

#pragma once
#ifndef PLAYER_COMPONENT_H
#define PLAYER_COMPONENT_H

#include <memory>
#include <string_view>

#include "../../engine/component/component.h"
#include "state/player_state.h"

namespace engine::input { class InputManager;  }
namespace engine::component {
	class TransformComponent;
	class PhysicsComponent;
	class SpriteComponent;
}
namespace game::component::state {
	class PlayerState;
}
namespace game::component {
/**
 * @brief 处理玩家输入, 状态和控制GameObject移动的组件.
 * 
 *		  使用状态模式管理Idle, Walk, Jump, Fall 等状态
 */
class PlayerComponent final : public engine::component::Component {
	friend class engine::object::GameObject;
public:
	PlayerComponent() = default;															///< @brief 默认构造函数
	~PlayerComponent() override = default;													///< @brief 重写析构函数

	// 禁用拷贝和移动语义
	PlayerComponent(const PlayerComponent&) = delete;										///< @brief 删除拷贝构造
	PlayerComponent& operator=(const PlayerComponent&) = delete;							///< @brief 删除拷贝赋值构造
	PlayerComponent(PlayerComponent&&) = delete;											///< @brief 删除移动构造
	PlayerComponent& operator=(PlayerComponent&&) = delete;									///< @brief 删除移动赋值构造
	
	engine::component::TransformComponent* getTransformComponent() const;					///< @brief 获取变换组件指针
	engine::component::SpriteComponent* getSpriteComponent() const;							///< @brief 获取精灵组件指针
	engine::component::PhysicsComponent* getPhysicsComponent() const;						///< @brief 获取物理组件指针

	void setIsDead(bool isDead);															///< @brief 设置玩家是否死亡
	bool getIsDead() const;																	///< @brief 获取玩家是否死亡
	void setMoveForce(float moveForce);														///< @brief 设置水平移动力
	float getMoveForce() const;																///< @brief 获取水平移动力
	void setMaxSpeed(float maxSpeed);														///< @brief 设置最大移动速度
	float getMaxSpeed() const;																///< @brief 获取最大移动速度
	void setFrictionFactor(float frictionFactor);											///< @brief 设置摩擦系数
	float getFrictionFactor() const;														///< @brief 获取摩擦系数
	void setJumpForce(float jumpForce);														///< @brief 设置弹跳力
	float getJumpForce() const;																///< @brief 获取弹跳力

	void setState(std::unique_ptr<state::PlayerState> newState);							///< @brief 设置玩家当前状态

private:
	void init() override;																	///< @brief 初始化
	void handleInput(engine::core::Context& context) override;								///< @brief 处理输入
	void update(float delta, engine::core::Context& context) override;						///< @brief 更新

private:
	static constexpr std::string_view mLogTag = "PlayerComponent";							///< @brief 日志标识
	engine::component::TransformComponent* mTransformComponent = nullptr;					///< @brief 变换组件指针
	engine::component::SpriteComponent* mSpriteComponent = nullptr;							///< @brief 精灵组件指针
	engine::component::PhysicsComponent* mPhysicsComponent = nullptr;						///< @brief 物理组件指针

	std::unique_ptr<state::PlayerState> mCurrentState;										///< @brief 玩家当前状态
	bool mIsDead = false;																	///< @brief 玩家是否死亡

	// 移动相关参数	
	float mMoveForce = 200.f;																///< @brief 水平移动力
	float mMaxSpeed = 120.f;																///< @brief 最大移动速度 (像素/秒)
	float mFrictionFactor = 0.85f;															///< @brief 摩擦系数 (Idle 时缓冲效果, 每帧乘以此系数)
	float mJumpForce = 350.f;																///< @brief 跳跃力 (按下Jump时给的瞬间的向上的力)
};
} // game::component

#endif PLAYER_COMPONENT_H
