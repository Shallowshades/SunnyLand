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
	engine::object::GameObject* gTestObject = nullptr;
public:
	GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& sceneManager);

	// 重写核心方法
	void init() override;
	void update(float deltaTime) override;
	void render() override;
	void handleInput() override;
	void clean() override;

private:
	// 测试函数
	void testCreateObject();
	void testCamera();
	void testObject();

private:
	constexpr static std::string_view mLogTag = "GameScene";
};
} // namespace game::scene

#endif // GAME_SCENE_H
