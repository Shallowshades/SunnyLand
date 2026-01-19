#include "game_scene.h"
#include "../component/player_component.h"
#include "../component/ai_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../component/ai/jump_behavior.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/render/animation.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/utils/math.h"
#include "../data/session_data.h"
#include <spdlog/spdlog.h>

namespace game::scene {
game::scene::GameScene::GameScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager, std::shared_ptr<game::data::SessionData> data)
	: Scene("GameScene", context, sceneManager)
	, mGameSessionData(std::move(data))
{
	if (!mGameSessionData) {
		mGameSessionData = std::make_unique<game::data::SessionData>();
		spdlog::info("{} : 未提供 SessionData, 使用默认值", std::string(mLogTag));
	}
	spdlog::trace("{} 构造完成", std::string(mLogTag));
}

void GameScene::init() {
	if (mIsInitialized) {
		spdlog::warn("{} : 已经初始化过了, 重复调用 init", std::string(mLogTag));
		return;
	}
	spdlog::trace("{} : 开始初始化", std::string(mLogTag));

	if (!initLevel()) {
		spdlog::error("{} : 关卡初始化失败, 无法继续.", std::string(mLogTag));
		mContext.getInputManager().setShouldQuit(true);
		return;
	}

	if (!initPlayer()) {
		spdlog::error("{} : 玩家初始化失败, 无法继续", std::string(mLogTag));
		mContext.getInputManager().setShouldQuit(true);
		return;
	}

	if (!initEnemyAndItem()) {
		spdlog::error("{} : 敌人和道具初始化失败, 无法继续.", std::string(mLogTag));
		mContext.getInputManager().setShouldQuit(true);
		return;
	}

	if (!initUI()) {
		spdlog::error("{} : UI初始化失败.", std::string(mLogTag));
		mContext.getInputManager().setShouldQuit(true);
		return;
	}

	// 设置音量
	mContext.getAudioPlayer().setMusicVolume(0.2f);
	mContext.getAudioPlayer().setSoundVolume(0.5f);
	mContext.getAudioPlayer().playMusic("assets/audio/hurry_up_and_run.ogg", true, 1000);

	Scene::init();
	spdlog::trace("{} : 初始化完成", std::string(mLogTag));
}

void GameScene::update(float deltaTime){
	Scene::update(deltaTime);
	handleObjectCollisions();
	handleTileTriggers();
}

void GameScene::render(){
	Scene::render();
}

void GameScene::handleInput(){
	Scene::handleInput();
}

void GameScene::clean(){
	Scene::clean();
}

bool GameScene::initLevel() {
	// 加载关卡(LevelLoader加载后即可销毁)
	spdlog::info("{} 加载关卡", std::string(mLogTag));
	engine::scene::LevelLoader levelLoader;
	auto levelPath = mGameSessionData->getMapPath();
	if (!levelLoader.loadLevel(levelPath, *this)) {
		spdlog::error("{} : 关卡加载失败", std::string(mLogTag));
		return false;
	}

	// 注册"main"层到物理引擎
	auto* mainLayer = findGameObjectByName("main");
	if (!mainLayer) {
		spdlog::error("{} : 未找到\"main\"层", std::string(mLogTag));
		return false;
	}
	auto* tileLayer = mainLayer->getComponent<engine::component::TileLayerComponent>();
	if (!tileLayer) {
		spdlog::error("{} : \"main\"层没有 TileLayerComponent 组件", std::string(mLogTag));
		return false;
	}
	mContext.getPhysicsEngine().registerCollisionLayer(tileLayer);
	spdlog::info("{} : 注册 'main' 层到物理引擎", std::string(mLogTag));

	// 设置相机边界
	auto worldSize = mainLayer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
	mContext.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0.f), worldSize));
	// 设置世界边界
	mContext.getPhysicsEngine().setWorldBound(engine::utils::Rect(glm::vec2(0.f), worldSize));
	
	spdlog::trace("{} : 关卡初始化完成", std::string(mLogTag));
	return true;
}

bool GameScene::initPlayer() {
	// 获取玩家对象
	mPlayer = findGameObjectByName("player");
	if (!mPlayer) {
		spdlog::error("{} : 未找到玩家对象", std::string(mLogTag));
		return false;
	}

	// 添加PlayerComponent到玩家对象
	auto* playerComponent = mPlayer->addComponent<game::component::PlayerComponent>();
	if (!playerComponent) {
		spdlog::error("{} : 无法添加PlayerComponent到玩家对象", std::string(mLogTag));
		return false;
	}

	// 相机跟随玩家
	auto* playerTransform = mPlayer->getComponent<engine::component::TransformComponent>();
	if (!playerTransform) {
		spdlog::error("{} : 玩家对象没有变换组件, 无法设置相机目标", std::string(mLogTag));
		return false;
	}
	mContext.getCamera().setTarget(playerTransform);
	spdlog::trace("{} : Player 初始化完成", std::string(mLogTag));
	return true;
}

bool GameScene::initEnemyAndItem() {
	bool success = true;
	for (auto& gameObject : mGameObjects) {
		if (gameObject->getName() == "eagle") {
			if (auto aic = gameObject->addComponent<game::component::AIComponent>(); aic) {
				auto maxY = gameObject->getComponent<engine::component::TransformComponent>()->getPosition().y;
				auto minY = maxY - 80.f;
				aic->setBehavior(std::make_unique<game::component::ai::UpdownBehavior>(minY, maxY));
			}
		}

		if (gameObject->getName() == "frog") {
			if (auto aic = gameObject->addComponent<game::component::AIComponent>(); aic) {
				auto maxX = gameObject->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.f;
				auto minX = maxX - 90.f;
				aic->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(minX, maxX));
			}
		}

		if (gameObject->getName() == "opossum") {
			if (auto aic = gameObject->addComponent<game::component::AIComponent>(); aic) {
				auto maxX = gameObject->getComponent<engine::component::TransformComponent>()->getPosition().x;
				auto minX = maxX - 200.f;
				aic->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(minX, maxX));
			}
		}

		if (gameObject->getTag() == "item") {
			if (auto* ac = gameObject->getComponent<engine::component::AnimationComponent>(); ac) {
				ac->playAnimation("idle");
			}
			else {
				spdlog::error("{} : item 对象缺少动画组件, 无法播放动画", std::string(mLogTag));
				success = false;
			}
		}
	}
	return success;
}

bool GameScene::initUI() {
	if (!mUIManager->init(glm::vec2(640.f, 360.f))) {
		return false;
	}

	createScoreUI();
	createHealthUI();

	return true;
}

void GameScene::handleObjectCollisions() {
	// 从物理引擎中获取碰撞对
	auto collisionPairs = mContext.getPhysicsEngine().getCollisionPairs();
	for (const auto& pair : collisionPairs) {
		auto obj1 = pair.first;
		auto obj2 = pair.second;

		// 处理玩家与敌人的碰撞
		if (obj1->getName() == "player" && obj2->getTag() == "enemy") { 
			playerVSEnemyCollision(obj1, obj2);
		}
		else if (obj2->getName() == "player" && obj1->getTag() == "enemy") {
			playerVSEnemyCollision(obj2, obj1);
		}

		// 处理玩家与道具的碰撞
		else if (obj1->getName() == "player" && obj2->getTag() == "item") {
			playerVSItemCollision(obj1, obj2);
		}
		else if (obj2->getName() == "player" && obj1->getTag() == "item") {
			playerVSItemCollision(obj2, obj1);
		}

		// 处理玩家与"hazard"碰撞
		else if (obj1->getName() == "player" && obj2->getTag() == "hazard") {
			obj1->getComponent<game::component::PlayerComponent>()->takeDamage(1);
			handlePlayerDamage(1);
			spdlog::debug("{} : 玩家 {} 受到了 HAZARD 对象伤害", std::string(mLogTag), obj1->getName());
		}
		else if (obj2->getName() == "player" && obj1->getTag() == "hazard") {
			obj2->getComponent<game::component::PlayerComponent>()->takeDamage(1);
			handlePlayerDamage(1);
			spdlog::debug("{} : 玩家 {} 受到了 HAZARD 对象伤害", std::string(mLogTag), obj2->getName());
		}

		// 处理玩家与关底触发器碰撞
		else if (obj1->getName() == "player" && obj2->getTag() == "next_level") {
			toNextLevel(obj2);
		} else if (obj2->getName() == "player" && obj1->getTag() == "next_level") {
			toNextLevel(obj1);
		}
	}
}

void GameScene::handleTileTriggers() {
	const auto& tileTriggerEvents = mContext.getPhysicsEngine().getTileTriggerEvents();
	for (const auto& event : tileTriggerEvents) {
		auto obj = event.first;
		auto tileType = event.second;
		if (tileType == engine::component::TileType::HAZARD) {
			// 碰撞危险瓦片, 受伤
			if (obj->getName() == "player") {
				handlePlayerDamage(1);
				spdlog::debug("{} : 玩家 {} 受到了 HAZARD 瓦片伤害", std::string(mLogTag), obj->getName());
			}
			// TODO: 其他对象类型的处理, 目前让敌人无视瓦片伤害
		}
	}
}

void GameScene::handlePlayerDamage(int damage) {
	auto playerComponent = mPlayer->getComponent<game::component::PlayerComponent>();
	if (!playerComponent->takeDamage(1)) {
		return;
	}

	if (playerComponent->getIsDead()) {
		spdlog::info("{} : 玩家 {} 死亡", std::string(mLogTag), mPlayer->getName());
		// TODO: 可能的死亡逻辑处理
	}

	// 更新生命值和生命值UI
	updateHealthWithUI();
}

void GameScene::playerVSEnemyCollision(engine::object::GameObject* player, engine::object::GameObject* enemy) {
	// 踩踏逻辑
	// 1. 玩家中心点在敌人上方
	// 2. 重叠区域: overlap.x > overlap.y
	auto playerAABB = player->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
	auto enemyAABB = enemy->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
	auto playerCenter = playerAABB.position + playerAABB.size / 2.f;
	auto enemyCenter = enemyAABB.position + enemyAABB.size / 2.f;
	auto overlap = glm::vec2(playerAABB.size / 2.f + enemyAABB.size / 2.f) - glm::abs(playerCenter - enemyCenter);

	// 踩踏判断成功, 敌人受伤
	if (overlap.x > overlap.y && playerCenter.y < enemyCenter.y) {
		spdlog::info("{} : 玩家 {} 踩踏了敌人 {}", std::string(mLogTag), player->getName(), enemy->getName());
		auto enemyHealth = enemy->getComponent<engine::component::HealthComponent>();
		if (!enemyHealth) {
			spdlog::error("{} : 敌人 {} 没有生命组件, 无法处理踩踏伤害", std::string(mLogTag), enemy->getName());
			return;
		}
		enemyHealth->takeDamage(1);
		if (!enemyHealth->isAlive()) {
			spdlog::info("{} : 敌人 {} 被踩踏后死亡", std::string(mLogTag), enemy->getName());
			enemy->setNeedRemove(true);
			createEffect(enemyCenter, enemy->getTag());
		}
		// 玩家跳起效果
		auto velocity = player->getComponent<engine::component::PhysicsComponent>()->getVelocity();
		velocity.y = -300.f;
		player->getComponent<engine::component::PhysicsComponent>()->setVelocity(velocity);
		// 播放音效
		mContext.getAudioPlayer().playSound("assets/audio/punch2a.mp3");
		// 加分
		addScoreWithUI(10);
	}
	else {
		spdlog::info("{} : 敌人 {} 对玩家 {} 造成伤害", std::string(mLogTag), enemy->getName(), player->getName());
		handlePlayerDamage(1);
	}
}

void GameScene::playerVSItemCollision(engine::object::GameObject* player, engine::object::GameObject* item) {
	if (item->getName() == "fruit") {
		player->getComponent<engine::component::HealthComponent>()->heal(1);
		healWithUI(1);
	}
	else if (item->getName() == "gem") {
		mGameSessionData->addScore(5);
		addScoreWithUI(5);
	}
	auto itemAABB = item->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
	createEffect(itemAABB.position + itemAABB.size / 2.f, item->getTag());
	// 播放吃到道具音效
	mContext.getAudioPlayer().playSound("assets/audio/poka01.mp3");
}

void GameScene::toNextLevel(engine::object::GameObject* trigger) {
	auto sceneName = trigger->getName();
	auto mapPath = levelNameToPath(sceneName);
	mGameSessionData->setNextLevel(mapPath);
	auto nextScene = std::make_unique<game::scene::GameScene>(mContext, mSceneManager, mGameSessionData);
	mSceneManager.requestReplaceScene(std::move(nextScene));
}

std::string GameScene::levelNameToPath(const std::string& levelName) const {
	return "assets/maps/" + levelName + ".tmj";
}

void GameScene::createEffect(const glm::vec2& centerPosition, const std::string& tag) {
	// 创建游戏对象和变换组件
	auto effectObject = std::make_unique<engine::object::GameObject>("effect_" + tag);
	effectObject->addComponent<engine::component::TransformComponent>(centerPosition);

	// 根据标签创建不同的精灵组件和动画
	auto animation = std::make_unique<engine::render::Animation>("effect", false);
	if (tag == "enemy") {
		effectObject->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png", mContext.getResourceManager(), engine::utils::Alignment::CENTER);
		for (auto i = 0; i < 6; ++i) {
			animation->addFrame(SDL_FRect { static_cast<float>(i * 40), 0.f, 40.f, 40.f }, 0.1f);
		}
	}
	else {
		spdlog::warn("{} : 未知特效类型: {}", std::string(mLogTag), tag);
		return;
	}

	// 根据创建的动画, 添加动画组件, 并设置为单次播放
	auto ac = effectObject->addComponent<engine::component::AnimationComponent>();
	ac->addAnimation(std::move(animation));
	ac->setOneShotRemoval(true);
	ac->playAnimation("effect");
	safeAddGameObject(std::move(effectObject));
	spdlog::debug("{} : 创建特效: {}", std::string(mLogTag), tag);
}

void GameScene::createScoreUI() {
	// 创建得分标签
	auto scoreText = "Score: " + std::to_string(mGameSessionData->getCurrentScore());
	auto score_label = std::make_unique<engine::ui::UILabel>(mContext.getTextRenderer(),
		scoreText,
		"assets/fonts/VonwaonBitmap-16px.ttf",
		16);
	mScoreLabel = score_label.get();									// 成员变量赋值（获取裸指针）
	auto screen_size = mUIManager->getRootElement()->getSize();			// 获取屏幕尺寸
	mScoreLabel->setPosition(glm::vec2(screen_size.x - 100.0f, 10.0f));
	mUIManager->addElement(std::move(score_label));
}

void GameScene::createHealthUI() {
	int maxHealth = mGameSessionData->getMaxHealth();
	int current_health = mGameSessionData->getCurrentHealth();
	float startX = 10.0f;
	float startY = 10.0f;
	float iconWidth = 20.0f;
	float iconHeight = 18.0f;
	float spacing = 5.0f;
	std::string fullHeartTexture = "assets/textures/UI/Heart.png";
	std::string emptyHeartTexture = "assets/textures/UI/Heart-bg.png";

	// 创建一个默认的UIPanel (不需要背景色，因此大小无所谓，只用于定位)
	auto healthPanel = std::make_unique<engine::ui::UIPanel>();
	mHealthPanel = healthPanel.get();				// 成员变量赋值（获取裸指针）

	// --- 根据最大生命值，循环创建生命值图标(添加到UIPanel中) ---
	for (int i = 0; i < maxHealth; ++i) {			// 创建背景图标
		glm::vec2 iconPosition = { startX + i * (iconWidth + spacing), startY };
		glm::vec2 iconSize = { iconWidth, iconHeight };

		auto bgIcon = std::make_unique<engine::ui::UIImage>(emptyHeartTexture, iconPosition, iconSize);
		mHealthPanel->addChild(std::move(bgIcon));
	}
	for (int i = 0; i < current_health; ++i) {		// 创建前景图标
		glm::vec2 iconPosition = { startX + i * (iconWidth + spacing), startY };
		glm::vec2 iconSize = { iconWidth, iconHeight };

		auto fgIcon = std::make_unique<engine::ui::UIImage>(fullHeartTexture, iconPosition, iconSize);
		mHealthPanel->addChild(std::move(fgIcon));
	}
	// 将UIPanel添加到UI管理器中
	mUIManager->addElement(std::move(healthPanel));
}

void GameScene::addScoreWithUI(int score) {
	mGameSessionData->addScore(score);
	auto scoreText = "Score: " + std::to_string(mGameSessionData->getCurrentScore());
	spdlog::info("得分: {}", scoreText);
	mScoreLabel->setText(scoreText);
}

void GameScene::healWithUI(int amount) {
	mPlayer->getComponent<engine::component::HealthComponent>()->heal(amount);
	updateHealthWithUI();
}

void GameScene::updateHealthWithUI() {
	if (!mPlayer || !mHealthPanel) {
		spdlog::error("玩家对象或 HealthPanel 不存在，无法更新生命值UI");
		return;
	}

	// 获取当前生命值并更新游戏数据
	auto currentHealth = mPlayer->getComponent<engine::component::HealthComponent>()->getCurrentHealth();
	mGameSessionData->setCurrentHealth(currentHealth);
	auto maxHealth = mGameSessionData->getMaxHealth();

	// 前景图标是后添加的，因此设置后半段的可见性即可
	for (auto i = maxHealth; i < maxHealth * 2; ++i) {
		mHealthPanel->getChildren()[i]->setVisible(i - maxHealth < currentHealth);
	}
}
} // namespace game::scene
