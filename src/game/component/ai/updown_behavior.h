/*****************************************************************//**
 * @file   updown_behavior.h
 * @brief  上下飞行策略
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.12.03
 *********************************************************************/

#pragma once
#include "ai_behavior.h"
#include <string_view>

namespace game::component::ai {

/**
* @brief AI行为: 在指定范围内垂直移动.
*
* 到达边界或障碍物时改变方向.
*/
class UpdownBehavior final : public AIBehavior {
	friend class game::component::AIComponent;
public:
	/**
	* @brief 构造函数.
	*
	* @param minX 巡逻范围的最小X坐标
	* @param maxX 巡逻范围的最大X坐标
	* @param speed 移动速度
	*/
	UpdownBehavior(float minY, float maxY, float speed = 50.f);
	~UpdownBehavior() override = default;												///< @brief 析构函数

	// 禁用拷贝和移动语义
	UpdownBehavior(const UpdownBehavior&) = delete;										///< @brief 删除拷贝构造
	UpdownBehavior& operator=(const UpdownBehavior&) = delete;							///< @brief 删除拷贝赋值构造
	UpdownBehavior(UpdownBehavior&&) = delete;											///< @brief 删除移动构造
	UpdownBehavior& operator=(UpdownBehavior&&) = delete;								///< @brief 删除移动赋值构造

private:
	void enter(AIComponent& aiComponent) override;										///< @brief 更新
	void update(float delta, AIComponent& aiComponent) override;						///< @brief 进入
private:
	static constexpr std::string_view mLogTag = "UpdownBehavior";						///< @brief 日志标识
	float mPatrolMinY = 0.f;															///< @brief 巡逻最小X坐标
	float mPatrolMaxY = 0.f;															///< @brief 巡逻最大X坐标
	float mMoveSpeed = 50.f;															///< @brief 移动速度
	bool mMoveDown = false;															///< @brief 是否向右移动
};
}
