/*****************************************************************//**
 * @file   menu_scene.h
 * @brief  菜单界面
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2026.01.21
 *********************************************************************/

#pragma once
#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "../../engine/scene/scene.h"
#include <memory>

 // 前向声明
namespace engine::core {
    class Context;
}
namespace engine::scene {
    class SceneManager;
}
namespace game::data {
    class SessionData;
}

namespace game::scene {

/**
* @brief 游戏暂停时显示的菜单场景，提供继续、保存、返回、退出等选项。
* 该场景通常被推送到 GameScene 之上。
*/
class MenuScene final : public engine::scene::Scene {
public:
    /**
	* @brief MenuScene 的构造函数
	* @param context 引擎上下文的引用
	* @param scene_manager 场景管理器的引用
	* @param session_data_ 场景间传递的游戏数据
	*/
    MenuScene(engine::core::Context& context,
        engine::scene::SceneManager& sceneManager,
        std::shared_ptr<game::data::SessionData> sessionData);

    ~MenuScene() override = default;

    // --- 核心循环方法 ---
    void init() override;
    void handleInput() override;

private:
    // 用于创建UI和按钮回调的私有辅助方法
    void createUI();
    void onResumeClicked();
    void onSaveClicked();
    void onBackClicked();
    void onQuitClicked();
private:
	std::shared_ptr<game::data::SessionData> mSessionData;
};

} // namespace game::scene

#endif // !MENU_SCENE_H
