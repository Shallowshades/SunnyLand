/*****************************************************************//**
 * @file   title_scene.h
 * @brief  主界面
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2026.01.20
 *********************************************************************/

#pragma once

#ifndef TITLE_SCENE_H
#define TITLE_SCENE_H

#include "../../engine/scene/scene.h"
#include <memory>

namespace engine::object {
    class GameObject;
}

namespace engine::scene {
    class SceneManager;
}

namespace game::data {
    class SessionData;
}

namespace game::scene {

/**
* @brief 标题场景类，提供4个按钮：开始游戏、加载游戏、帮助、退出
*/
class TitleScene final : public engine::scene::Scene {
public:
    /**
	* @brief 构造函数
	* @param context 引擎上下文
	* @param sceneManager 场景管理器
	* @param gamePlayState 指向游戏玩法状态的共享指针
	*/
    TitleScene(engine::core::Context& context,
        engine::scene::SceneManager& sceneManager,
        std::shared_ptr<game::data::SessionData> sessionData = nullptr);

    ~TitleScene() override = default;

    // --- 核心方法 --- //
    void init() override;
    void update(float delta_time) override;

    // 禁止拷贝和移动
    TitleScene(const TitleScene&) = delete;
    TitleScene& operator=(const TitleScene&) = delete;
    TitleScene(TitleScene&&) = delete;
    TitleScene& operator=(TitleScene&&) = delete;

private:
    // 初始化 UI 元素
    void createUI();

    // 按钮回调函数
    void onStartGameClick();
    void onLoadGameClick();
    void onHelpsClick();
    void onQuitClick();
private:
	std::shared_ptr<game::data::SessionData> mSessionData;
};
} // namespace game::scene

#endif // !TITLE_SCENE_H
