/*****************************************************************//**
 * @file   jump_behavior.h
 * @brief  跳跃行为
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.12.02
 *********************************************************************/

#pragma once
#include "ai_behavior.h"
#include <glm/vec2.hpp>
#include <string_view>

namespace game::component::ai {

/**
* @brief AI行为: 在指定范围内周期性地跳跃.
* 
* 在地面时等待, 然后向当前方向跳跃.
* 撞墙或者到达边界时改变下次跳跃方向.
*/

class JumpBehavior final : public AIBehavior {
	friend class game::component::AIComponent;
public:
	/**
	 * @brief 构造函数.
	 * 
	 * @param minX 巡逻范围的最小X坐标
	 * @param maxX 巡逻范围的最大X坐标
	 * @param jumpVelocity 跳跃速度向量 (水平, 垂直)
	 * @param jumpInterval 两次跳跃之间的间隔时间
	 */
	JumpBehavior(float minX, float maxX, glm::vec2 jumpVelocity = glm::vec2(100.f, -300.f), float jumpInterval = 2.f);
	~JumpBehavior() override = default;

	// 禁用拷贝和移动语义
	JumpBehavior(const JumpBehavior&) = delete;										///< @brief 删除拷贝构造
	JumpBehavior& operator=(const JumpBehavior&) = delete;							///< @brief 删除拷贝赋值构造
	JumpBehavior(JumpBehavior&&) = delete;											///< @brief 删除移动构造
	JumpBehavior& operator=(JumpBehavior&&) = delete;								///< @brief 删除移动赋值构造

private:
	void update(float delta, AIComponent& aiComponent) override;

private:
	static constexpr std::string_view mLogTag = "JumpBehavior";
	float mPatrolMinX = 0.f;
	float mPatrolMaxX = 0.f;
	glm::vec2 mJumpVelocity = glm::vec2(100.f, -300.f);
	float mJumpInterval = 2.f;
	float mJumpTimer = 0.f;
	bool mJumpRight = false;
};
}
