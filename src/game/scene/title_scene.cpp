#include "title_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/utils/math.h"
#include "../data/session_data.h"
#include "game_scene.h"
#include "helps_scene.h"
#include <spdlog/spdlog.h>

namespace game::scene {

// 构造函数：初始化场景名称和上下文，创建 UI 管理器
TitleScene::TitleScene(engine::core::Context& context,
	engine::scene::SceneManager& sceneManager,
	std::shared_ptr<game::data::SessionData> sessionData)
	: engine::scene::Scene("TitleScene", context, sceneManager),
	mSessionData(std::move(sessionData))
{
	if (!mSessionData) {
		spdlog::warn("TitleScene 接收到空的 SessionData，创建一个默认的 SessionData");
		mSessionData = std::make_shared<game::data::SessionData>();
	}
	spdlog::trace("TitleScene 创建.");
}

// 初始化场景
void TitleScene::init() {
	if (mIsInitialized) {
		return;
	}
	// 加载背景地图
	engine::scene::LevelLoader levelLoader;
	if (!levelLoader.loadLevel("assets/maps/level0.tmj", *this)) {
		spdlog::error("加载背景失败");
		return;
	}

	// 创建 UI 元素
	createUI();

	Scene::init();
	spdlog::trace("TitleScene 初始化完成.");
}

// 创建 UI 界面元素
void TitleScene::createUI() {
	spdlog::trace("创建 TitleScene UI...");
	auto windowSize = glm::vec2(640.0f, 360.0f);

	if (!mUIManager->init(windowSize)) {
		spdlog::error("初始化 UIManager 失败!");
		return;
	}

	// 设置音量
	mContext.getAudioPlayer().setMusicVolume(0.2f);  // 设置背景音乐音量为20%
	mContext.getAudioPlayer().setSoundVolume(0.5f);  // 设置音效音量为50%

	// 设置背景音乐
	// context_.getAudioPlayer().playMusic("assets/audio/platformer_level03_loop.ogg");

	// 创建标题图片 (假设不知道大小)
	auto titleImage = std::make_unique<engine::ui::UIImage>("assets/textures/UI/title-screen.png");
	auto size = mContext.getResourceManager().getTextureSize(titleImage->getTextureId());
	titleImage->setSize(size * 2.0f);      // 放大为2倍

	// 水平居中
	auto titlePosition = (windowSize - titleImage->getSize()) / 2.0f - glm::vec2(0.0f, 50.0f);
	titleImage->setPosition(titlePosition);
	mUIManager->addElement(std::move(titleImage));

	// --- 创建按钮面板并居中 --- (4个按钮，设定好大小、间距)
	float buttonWidth = 96.0f;
	float buttonHeight = 32.0f;
	float buttonSpacing = 20.0f;
	int numButtons = 4;

	// 计算面板总宽度
	float panelWidth = numButtons * buttonWidth + (numButtons - 1) * buttonSpacing;
	float panelHeight = buttonHeight;

	// 计算面板位置使其居中
	float panelX = (windowSize.x - panelWidth) / 2.0f;
	float panelY = windowSize.y * 0.65f;  // 垂直位置中间靠下

	auto buttonPanel = std::make_unique<engine::ui::UIPanel>(
		glm::vec2(panelX, panelY),
		glm::vec2(panelWidth, panelHeight)
	);

	// --- 创建按钮并添加到 UIPanel (位置是相对于 UIPanel 的 0,0) ---
	glm::vec2 currentButtonPosition = glm::vec2(0.0f, 0.0f);
	glm::vec2 buttonSize = glm::vec2(buttonWidth, buttonHeight);

	// Start Button
	auto startButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Start1.png",
		"assets/textures/UI/buttons/Start2.png",
		"assets/textures/UI/buttons/Start3.png",
		currentButtonPosition,
		buttonSize,
		[this]() { this->onStartGameClick(); });
	buttonPanel->addChild(std::move(startButton));

	// Load Button
	currentButtonPosition.x += buttonWidth + buttonSpacing;
	auto loadButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Load1.png",
		"assets/textures/UI/buttons/Load2.png",
		"assets/textures/UI/buttons/Load3.png",
		currentButtonPosition,
		buttonSize,
		[this]() { this->onLoadGameClick(); });
	buttonPanel->addChild(std::move(loadButton));

	// Helps Button
	currentButtonPosition.x += buttonWidth + buttonSpacing;
	auto helpsButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Helps1.png",
		"assets/textures/UI/buttons/Helps2.png",
		"assets/textures/UI/buttons/Helps3.png",
		currentButtonPosition,
		buttonSize,
		[this]() { this->onHelpsClick(); });
	buttonPanel->addChild(std::move(helpsButton));

	// Quit Button
	currentButtonPosition.x += buttonWidth + buttonSpacing;
	auto quitButton = std::make_unique<engine::ui::UIButton>(mContext,
		"assets/textures/UI/buttons/Quit1.png",
		"assets/textures/UI/buttons/Quit2.png",
		"assets/textures/UI/buttons/Quit3.png",
		currentButtonPosition,
		buttonSize,
		[this]() { this->onQuitClick(); });
	buttonPanel->addChild(std::move(quitButton));

	// 将 UIPanel 添加到UI管理器
	mUIManager->addElement(std::move(buttonPanel));

	// 创建 Credits 标签
	auto creditsLabel = std::make_unique<engine::ui::UILabel>(mContext.getTextRenderer(),
		"SunnyLand Credits: XXX - 2025",
		"assets/fonts/VonwaonBitmap-16px.ttf",
		16,
		engine::utils::FColor{ 0.8f, 0.8f, 0.8f, 1.0f });
	creditsLabel->setPosition(glm::vec2{ (windowSize.x - creditsLabel->getSize().x) / 2.0f,
											windowSize.y - creditsLabel->getSize().y - 10.0f });
	mUIManager->addElement(std::move(creditsLabel));

	spdlog::trace("TitleScene UI 创建完成.");
}

// 更新场景逻辑
void TitleScene::update(float delta_time) {
	Scene::update(delta_time);

	// 相机自动向右移动
	mContext.getCamera().move(glm::vec2(delta_time * 100.0f, 0.0f));
}

// --- 按钮回调实现 --- //

void TitleScene::onStartGameClick() {
	spdlog::debug("开始游戏按钮被点击。");
	if (mSessionData) {
		mSessionData->reset();
	}
	mSceneManager.requestReplaceScene(std::make_unique<GameScene>(mContext, mSceneManager, mSessionData));
}

void TitleScene::onLoadGameClick() {
	spdlog::debug("加载游戏按钮被点击。");
	if (!mSessionData) {
		spdlog::error("游戏状态为空，无法加载。");
		return;
	}

	if (mSessionData->loadFromFile("assets/save.json")) {
		spdlog::debug("保存文件加载成功。开始游戏...");
		mSceneManager.requestReplaceScene(std::make_unique<GameScene>(mContext, mSceneManager, mSessionData));
	}
	else {
		spdlog::warn("加载保存文件失败。");
	}
}

void TitleScene::onHelpsClick() {
	spdlog::debug("帮助按钮被点击。");
	mSceneManager.requestPushScene(std::make_unique<HelpsScene>(mContext, mSceneManager));
}

void TitleScene::onQuitClick() {
	spdlog::debug("退出按钮被点击。");
	mContext.getInputManager().setShouldQuit(true);
}

} // namespace game::scenes 