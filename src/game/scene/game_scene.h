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
#include <glm/vec2.hpp>

namespace engine::object { class GameObject; }
namespace game::data { class SessionData; }

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
	GameScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager, std::shared_ptr<game::data::SessionData> data = nullptr);

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
	[[nodiscard]] bool initUI();									///< @brief 初始化UI

	///< @brief 处理游戏对象间的碰撞逻辑(从物理引擎获取信息).
	void handleObjectCollisions();
	///< @brief 处理瓦片触发事件(从物理引擎获取信息).
	void handleTileTriggers();
	///< @brief 处理玩家受伤 (更新得分, UI)
	void handlePlayerDamage(int damage);
	///< @brief 处理玩家与敌人碰撞处理
	void playerVSEnemyCollision(engine::object::GameObject* player, engine::object::GameObject* enemy);
	///< @brief 处理玩家与物品碰撞处理
	void playerVSItemCollision(engine::object::GameObject* player, engine::object::GameObject* item);
	///< @brief 进入下一个关卡
	void toNextLevel(engine::object::GameObject* trigger);
	///< @brief 根据关卡名称获取对应的地图文件
	std::string levelNameToPath(const std::string& levelName) const;

	/**
	 * @brief 创建一个特效对象(一次性).
	 * 
	 * @param centerPosition 特效中心位置
	 * @param tag 特效标签(决定特效类型, 例如"enemy", "item")
	 */
	void createEffect(const glm::vec2& centerPosition, const std::string& tag);

private:
	constexpr static std::string_view mLogTag = "GameScene";		///< @brief 日志标识

	engine::object::GameObject* mPlayer = nullptr;					///< @brief 玩家对象指针
	std::shared_ptr<game::data::SessionData> mGameSessionData;		///< @brief 场景间共享数据, 因此使用shared_ptr
};
} // namespace game::scene

#endif // GAME_SCENE_H
