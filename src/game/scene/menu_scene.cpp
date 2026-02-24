#include "menu_scene.h"
#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/core/game_state.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/utils/math.h"
#include "../data/session_data.h"
#include <spdlog/spdlog.h>

namespace game::scene {
MenuScene::MenuScene(engine::core::Context& context,
	engine::scene::SceneManager& sceneManager,
	std::shared_ptr<game::data::SessionData> sessionData)
	: Scene("MenuScene", context, sceneManager), // 调用基类构造函数
	mSessionData(std::move(sessionData)) {
	if (!mSessionData) {
		spdlog::error("菜单场景构造时 SessionData 为空。");
	}
	spdlog::trace("MenuScene 构造完成.");
}

void MenuScene::init() {
	mContext.getGameState().setState(engine::core::State::Paused);
	createUI();

	Scene::init();
	spdlog::trace("menuScene 初始化完成");
}

void MenuScene::createUI() {

	auto windowSize = mContext.getGameState().getLogicalSize();
	if (!mUIManager->init(windowSize)) {
		spdlog::error("MenuScene 中初始化 UIManager 失败!");
		return;
	}

	// "PAUSE"标签
	auto pause_label = std::make_unique<engine::ui::UILabel>(mContext.getTextRenderer(),
		"PAUSE",
		"assets/fonts/VonwaonBitmap-16px.ttf",
		32);
	// 放在中间靠上的位置 
	auto size = pause_label->getSize();
	auto labelY = windowSize.y * 0.2;
	pause_label->setPosition(glm::vec2((windowSize.x - size.x) / 2.0f, labelY));
	mUIManager->addElement(std::move(pause_label));

	// --- 创建按钮 --- (4个按钮，设定好大小、间距)
	float buttonWidth = 96.0f; // 按钮稍微小一点
	float buttonHeight = 32.0f;
	float buttonSpacing = 10.0f;
	float startY = labelY + 80.0f; // 从标签下方开始，增加间距
	float buttonX = (windowSize.x - buttonWidth) / 2.0f; // 水平居中

	// Resume Button
	auto resumeButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Resume1.png",
		"assets/textures/UI/buttons/Resume2.png",
		"assets/textures/UI/buttons/Resume3.png",
		glm::vec2{ buttonX, startY },
		glm::vec2{ buttonWidth, buttonHeight },
		[this]() { this->onResumeClicked(); });
	mUIManager->addElement(std::move(resumeButton));

	// Save Button
	startY += buttonHeight + buttonSpacing;
	auto saveButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Save1.png",
		"assets/textures/UI/buttons/Save2.png",
		"assets/textures/UI/buttons/Save3.png",
		glm::vec2{ buttonX, startY },
		glm::vec2{ buttonWidth, buttonHeight },
		[this]() { this->onSaveClicked(); });
	mUIManager->addElement(std::move(saveButton));

	// Back Button
	startY += buttonHeight + buttonSpacing;
	auto backButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Back1.png",
		"assets/textures/UI/buttons/Back2.png",
		"assets/textures/UI/buttons/Back3.png",
		glm::vec2{ buttonX, startY },
		glm::vec2{ buttonWidth, buttonHeight },
		[this]() { this->onBackClicked(); });
	mUIManager->addElement(std::move(backButton));

	// Quit Button
	startY += buttonHeight + buttonSpacing;
	auto quitButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Quit1.png",
		"assets/textures/UI/buttons/Quit2.png",
		"assets/textures/UI/buttons/Quit3.png",
		glm::vec2{ buttonX, startY },
		glm::vec2{ buttonWidth, buttonHeight },
		[this]() { this->onQuitClicked(); });
	mUIManager->addElement(std::move(quitButton));
}

void MenuScene::handleInput() {
	// 先让 UIManager 处理交互
	Scene::handleInput();

	// 检查暂停键，允许按暂停键恢复游戏
	if (mContext.getInputManager().isActionPressed("pause")) {
		spdlog::debug("在菜单场景中按下暂停键，正在恢复游戏...");
		mSceneManager.requestPopScene();       // 弹出自身以恢复底层的GameScene
		mContext.getGameState().setState(engine::core::State::Playing);
	}
}

// --- 按钮回调函数实现 ---

void MenuScene::onResumeClicked() {
	spdlog::debug("继续游戏按钮被点击。");
	mSceneManager.requestPopScene();           // 弹出当前场景
	mContext.getGameState().setState(engine::core::State::Playing);
}

void MenuScene::onSaveClicked() {
	spdlog::debug("保存游戏按钮被点击。");
	if (mSessionData && mSessionData->saveToFile("assets/save.json")) {
		spdlog::debug("菜单场景中成功保存游戏数据。");
	}
	else {
		spdlog::error("菜单场景中保存游戏数据失败。");
	}
}

void MenuScene::onBackClicked() {
	spdlog::debug("返回按钮被点击。弹出菜单场景和游戏场景，返回标题界面。");
	// 直接替换为TitleScene
	mSceneManager.requestReplaceScene(std::make_unique<TitleScene>(mContext, mSceneManager, mSessionData));
}

void MenuScene::onQuitClicked() {
	spdlog::debug("退出按钮被点击。请求应用程序退出。");
	mSessionData->syncHighScore("assets/save.json");
	mContext.getInputManager().setShouldQuit(true);     // 输入管理器设置退出标志
}

} // namespace game::scene