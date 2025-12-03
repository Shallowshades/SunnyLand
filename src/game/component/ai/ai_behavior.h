/*****************************************************************//**
 * @file   ai_behavior.h
 * @brief  AI行为策略的抽象基类
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.12.02
 *********************************************************************/

#pragma once
#ifndef AI_BEHAVIOR_H
#define AI_BEHAVIOR_H

namespace game::component { class AIComponent; }

namespace game::component::ai {
/**
 * @brief AI 行为策略的抽象基类.
 */
class AIBehavior {
	friend class game::component::AIComponent;
public:
	AIBehavior() = default;														///< @brief 构造函数
	virtual ~AIBehavior() = default;											///< @brief 析构函数

	// 禁用拷贝和移动语义
	AIBehavior(const AIBehavior&) = delete;										///< @brief 删除拷贝构造
	AIBehavior& operator=(const AIBehavior&) = delete;							///< @brief 删除拷贝赋值构造
	AIBehavior(AIBehavior&&) = delete;											///< @brief 删除移动构造
	AIBehavior& operator=(AIBehavior&&) = delete;								///< @brief 删除移动赋值构造
protected:
	// 没有保存拥有者指针, 因此需要传入AI组件引用
	virtual void enter(AIComponent&) {};										///< @brief 可选是否实现, 默认为空
	virtual void update(float, AIComponent&) = 0;								///< @brief 更新AI行为逻辑(具体策略), 必须实现
};
} // namespace game::component::ai

#endif // !AI_BEHAVIOR_H
