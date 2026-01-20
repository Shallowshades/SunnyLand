#include "helps_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include <spdlog/spdlog.h>

namespace game::scene {

HelpsScene::HelpsScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager)
	: engine::scene::Scene("HelpsScene", context, sceneManager) {
	spdlog::trace("HelpsScene 创建.");
}

void HelpsScene::init() {
	if (mIsInitialized) {
		return;
	}
	auto windowSize = glm::vec2(640.0f, 360.0f);

	// 创建帮助图片 UIImage （让它覆盖整个屏幕）
	auto help_image = std::make_unique<engine::ui::UIImage>(
		"assets/textures/UI/instructions.png",
		glm::vec2(0.0f, 0.0f),
		windowSize
	);

	mUIManager->addElement(std::move(help_image));

	Scene::init();
	spdlog::trace("HelpsScene 初始化完成.");
}

void HelpsScene::handleInput() {
	if (!mIsInitialized) return;

	// 检测是否按下鼠标左键
	if (mContext.getInputManager().isActionPressed("MouseLeftClick")) {
		spdlog::debug("鼠标左键被按下, 退出 HelpsScene.");
		mSceneManager.requestPopScene();
	}
}

} // namespace game::scene