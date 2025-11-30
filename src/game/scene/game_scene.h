/*****************************************************************//**
 * @file   game_scene.h
 * @brief  游戏场景
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.11.04
 *********************************************************************/

#pragma once
#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <memory>
#include "../../engine/scene/scene.h"

namespace engine::object { class GameObject; }

namespace game::scene {

/**
 * @brief 主要的游戏场景, 包含玩家, 敌人, 关卡元素等.
 */
class GameScene final : public engine::scene::Scene {
public:
	/**
	 * @brief 构造函数.
	 * 
	 * @param name 场景名称
	 * @param context 上下文引用
	 * @param sceneManager 场景管理器引用
	 */
	GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& sceneManager);

	// 重写核心方法
	void init() override;											///< @brief 初始化
	void update(float deltaTime) override;							///< @brief 更新
	void render() override;											///< @brief 渲染
	void handleInput() override;									///< @brief 处理输入
	void clean() override;											///< @brief 清理

private:
	[[nodiscard]] bool initLevel();									///< @brief 初始化关卡
	[[nodiscard]] bool initPlayer();								///< @brief 初始化玩家
	[[nodiscard]] bool initEnemyAndItem();							///< @brief 初始化敌人和道具

private:
	constexpr static std::string_view mLogTag = "GameScene";		///< @brief 日志标识

	engine::object::GameObject* mPlayer = nullptr;					///< @brief 玩家对象指针
};
} // namespace game::scene

#endif // GAME_SCENE_H
