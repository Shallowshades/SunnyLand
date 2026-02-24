/*****************************************************************//**
 * @file   end_scene.h
 * @brief  结束界面
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2026.02.24
 *********************************************************************/

#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
#include <string>

namespace engine::core { class Context; }
namespace engine::scene { class SceneManager; }
namespace game::data { class SessionData; }
namespace engine::ui {
	class UIManager;
	class Label;
	class Button;
}
namespace engine::input { class InputManager; }
namespace engine::render {
	class Renderer;
	class Camera;
}

namespace game::scene {

/**
 * @class EndScene
 * @brief 显示游戏结束(胜利或失败)信息的叠加场景
 * 
 * 提供重新开始或者返回主菜单的选项
 */
class EndScene final : public engine::scene::Scene {
public:
	EndScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager, std::shared_ptr<game::data::SessionData> sessionData);
	~EndScene() override = default;

	void init() override;

	// 禁止拷贝和移动
	EndScene(const EndScene&) = delete;
	EndScene& operator=(const EndScene&) = delete;
	EndScene(EndScene&&) = delete;
	EndScene& operator=(EndScene&&) = delete;

private:
	void createUI();
	void onBackClick();
	void onRestartClick();

private:
	std::shared_ptr<game::data::SessionData> mSessionData;
};
} // namespace game::scene