/*****************************************************************//**
 * @file   patrol_behavior.h
 * @brief  巡逻行为
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
* @brief AI行为: 在指定范围内左右巡逻.
*
* 撞墙或者到达边界时改变方向.
*/

class PatrolBehavior final : public AIBehavior {
	friend class game::component::AIComponent;
public:
	/**
	* @brief 构造函数.
	*
	* @param minX 巡逻范围的最小X坐标
	* @param maxX 巡逻范围的最大X坐标
	* @param speed 移动速度
	*/
	PatrolBehavior(float minX, float maxX, float speed = 50.f);
	~PatrolBehavior() override = default;												///< @brief 析构函数

	// 禁用拷贝和移动语义
	PatrolBehavior(const PatrolBehavior&) = delete;										///< @brief 删除拷贝构造
	PatrolBehavior& operator=(const PatrolBehavior&) = delete;							///< @brief 删除拷贝赋值构造
	PatrolBehavior(PatrolBehavior&&) = delete;											///< @brief 删除移动构造
	PatrolBehavior& operator=(PatrolBehavior&&) = delete;								///< @brief 删除移动赋值构造

private:
	void enter(AIComponent& aiComponent) override;										///< @brief 更新
	void update(float delta, AIComponent& aiComponent) override;						///< @brief 进入
private:
	static constexpr std::string_view mLogTag = "PatrolBehavior";						///< @brief 日志标识
	float mPatrolMinX = 0.f;															///< @brief 巡逻最小X坐标
	float mPatrolMaxX = 0.f;															///< @brief 巡逻最大X坐标
	float mMoveSpeed = 50.f;															///< @brief 移动速度
	bool mMoveRight = false;															///< @brief 是否向右移动
};
}
