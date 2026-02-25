#include "end_scene.h"
#include "title_scene.h"
#include "game_scene.h"
#include "../data/session_data.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/utils/math.h"
#include <spdlog/spdlog.h>

namespace game::scene {
EndScene::EndScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager, std::shared_ptr<game::data::SessionData> sessionData) 
	: engine::scene::Scene("EndScene", context, sceneManager)
	, mSessionData(std::move(sessionData))
{
	if (!mSessionData) {
		spdlog::error("error : 结束场景收到了空的游戏数据");
	}
	spdlog::trace("EndScene : {} 创建.", mSessionData->getIsWin() ? "Win" : "Lose");
}

void EndScene::init() {
	if (mIsInitialized) {
		return;
	}

	mContext.getGameState().setState(engine::core::State::GameOver);

	createUI();

	Scene::init();

	spdlog::info("EndScene 初始化完成");
}

void EndScene::createUI() {
	auto windowSize = mContext.getGameState().getLogicalSize();
	if (!mUIManager->init(windowSize)) {
		spdlog::error("错误 : 结束场景的UI管理器初始化失败!");
		return;
	}

	auto isWin = mSessionData->getIsWin();

	// 主文字标签
	std::string mainMessage = isWin ? "YOU WIN! CONGRATS!" : "YOU DIED! TRY AGAIN!";
	// 颜色
	engine::utils::FColor messageColor = isWin ? engine::utils::FColor{ 0.f, 1.f, 0.f, 1.f } : engine::utils::FColor{ 1.f, 0.f, 0.f, 1.f };
	// label
	auto mainLabel = std::make_unique<engine::ui::UILabel>(mContext.getTextRenderer(), mainMessage, "assets/fonts/VonwaonBitmap-16px.ttf", 48, messageColor);
	// 标签居中
	glm::vec2 labelSize = mainLabel->getSize();
	glm::vec2 mainLabelPosition = { (windowSize.x - labelSize.x) / 2.f, windowSize.y * 0.3f };
	mainLabel->setPosition(mainLabelPosition);
	mUIManager->addElement(std::move(mainLabel));

	// 得分标签
	int currentScore = mSessionData->getCurrentScore();
	int highScore = mSessionData->getHighScore();
	engine::utils::FColor scoreColor = { 1.f, 1.f, 1.f, 1.f };
	int scoreFontSize = 24;

	// 当前得分
	std::string scoreText = "Score : " + std::to_string(currentScore);
	auto scoreLabel = std::make_unique<engine::ui::UILabel>(mContext.getTextRenderer(), scoreText, "assets/fonts/VonwaonBitmap-16px.ttf", scoreFontSize, scoreColor);
	glm::vec2 scoreLabelSize = scoreLabel->getSize();
	glm::vec2 scoreLabelPosition = { (windowSize.x - scoreLabelSize.x) / 2.f, mainLabelPosition.y + labelSize.y + 20.f };
	scoreLabel->setPosition(scoreLabelPosition);
	mUIManager->addElement(std::move(scoreLabel));

	// 最高分
	std::string highScoreText = "High Score : " + std::to_string(highScore);
	auto highScoreLabel = std::make_unique<engine::ui::UILabel>(mContext.getTextRenderer(), highScoreText, "assets/fonts/VonwaonBitmap-16px.ttf", scoreFontSize, scoreColor);
	glm::vec2 highScoreLabelSize = highScoreLabel->getSize();
	glm::vec2 highScoreLabelPosition = { (windowSize.x - highScoreLabelSize.x) / 2.f, scoreLabelPosition.y + scoreLabelSize.y + 10.f };
	highScoreLabel->setPosition(highScoreLabelPosition);
	mUIManager->addElement(std::move(highScoreLabel));

	// UI 按钮
	glm::vec2 buttonSize = { 120.f, 40.f };
	float buttonSpacing = 20.f;
	float totalButtonWidth = buttonSize.x * 2.f + buttonSpacing;

	// 按钮放在右下角, 与边缘间隔30像素
	float buttonX = windowSize.x - totalButtonWidth - 30.f;
	float buttonY = windowSize.y - buttonSize.y - 30.f;

	// Back Button
	auto backButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Back1.png",
		"assets/textures/UI/buttons/Back2.png",
		"assets/textures/UI/buttons/Back3.png",
		glm::vec2{ buttonX, buttonY },
		buttonSize,
		[this]() { this->onBackClick(); }
	);
	mUIManager->addElement(std::move(backButton));

	// Restart Button
	buttonX += buttonSize.x + buttonSpacing;
	auto restartButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Restart1.png",
		"assets/textures/UI/buttons/Restart2.png",
		"assets/textures/UI/buttons/Restart3.png",
		glm::vec2{ buttonX, buttonY },
		buttonSize,
		[this]() { this->onRestartClick(); }
	);
	mUIManager->addElement(std::move(restartButton));
}

void EndScene::onBackClick() {
	spdlog::info("结束场景 : 返回按钮被点击");
	mSceneManager.requestReplaceScene(std::make_unique<TitleScene>(mContext, mSceneManager, mSessionData));
}

void EndScene::onRestartClick() {
	spdlog::info("结束场景 : 重新开始按钮被点击");
	mSessionData->reset();
	mSceneManager.requestReplaceScene(std::make_unique<GameScene>(mContext, mSceneManager, mSessionData));
}
} // namespace game::scene
