#include "game_scene.h"
#include "../component/player_component.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>

namespace game::scene {
game::scene::GameScene::GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& sceneManager) 
	: Scene(name, context, sceneManager)
{
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

	Scene::init();
	spdlog::trace("{} : 初始化完成", std::string(mLogTag));
}

void GameScene::update(float deltaTime){
	Scene::update(deltaTime);
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
	if (!levelLoader.loadLevel("assets/maps/level1.tmj", *this)) {
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
} // namespace game::scene
