#include "game_scene.h"
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
	// 加载关卡(LevelLoader加载后即可销毁)
	spdlog::info("{} 加载关卡", std::string(mLogTag));
	engine::scene::LevelLoader levelLoader;
	levelLoader.loadLevel("assets/maps/level1.tmj", *this);

	// 注册"main"层到物理引擎
	auto* mainLayer = findGameObjectByName("main");
	if (mainLayer) {
		auto* tileLayer = mainLayer->getComponent<engine::component::TileLayerComponent>();
		if (tileLayer) {
			mContext.getPhysicsEngine().registerCollisionLayer(tileLayer);
			spdlog::info("{} : 注册 'main' 层到物理引擎", std::string(mLogTag));
		}
	}

	// 测试创建游戏对象
	testCreateObject();

	Scene::init();
	spdlog::trace("{} 初始化完成", std::string(mLogTag));
}

void GameScene::update(float deltaTime){
	Scene::update(deltaTime);
	testCollisionPairs();
}

void GameScene::render(){
	Scene::render();
}

void GameScene::handleInput(){
	Scene::handleInput();
	// testCamera();
	testObject();
}

void GameScene::clean(){
	Scene::clean();
}

void GameScene::testCreateObject() {
	spdlog::trace("{} 创建测试对象", std::string(mLogTag));
	auto testObject = std::make_unique<engine::object::GameObject>("testObject");
	gTestObject = testObject.get();

	// 添加组件
	testObject->addComponent<engine::component::TransformComponent>(glm::vec2(100.f, 100.f));
	testObject->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", mContext.getResourceManager());
	testObject->addComponent<engine::component::PhysicsComponent>(&mContext.getPhysicsEngine());
	testObject->addComponent<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(glm::vec2(32.f)));

	// 添加到场景中
	addGameObject(std::move(testObject));

	// 添加组件2
	auto testObject2 = std::make_unique<engine::object::GameObject>("testObject2");
	testObject2->addComponent<engine::component::TransformComponent>(glm::vec2(50.f, 50.f));
	testObject2->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", mContext.getResourceManager());
	testObject2->addComponent<engine::component::PhysicsComponent>(&mContext.getPhysicsEngine(), false);
	testObject2->addComponent<engine::component::ColliderComponent>(std::make_unique<engine::physics::AABBCollider>(glm::vec2(16.f)));

	// 添加到场景中
	addGameObject(std::move(testObject2));

	spdlog::trace("{} testObject 创建并添加到GameScene中", std::string(mLogTag));
}

void GameScene::testCamera() {
	auto& camera = mContext.getCamera();
	auto& inputManager = mContext.getInputManager();
	if (inputManager.isActionDown("MoveUp")) {
		camera.move(glm::vec2(0.f, -1.f));
	}
	if (inputManager.isActionDown("MoveDown")) {
		camera.move(glm::vec2(0.f, 1.f));
	}
	if (inputManager.isActionDown("MoveLeft")) {
		camera.move(glm::vec2(-1.f, 0.f));
	}
	if (inputManager.isActionDown("MoveRight")) {
		camera.move(glm::vec2(1.f, 0.f));
	}
}

void GameScene::testObject() {
	if (!gTestObject) {
		return;
	}

	auto& inputManager = mContext.getInputManager();
	
	auto* pc = gTestObject->getComponent<engine::component::PhysicsComponent>();
	if (!pc) {
		return;
	}

	if (inputManager.isActionDown("MoveLeft")) {
		pc->setVelocity(glm::vec2(-100.f, pc->getVelocity().y));
	}
	else {
		pc->setVelocity(glm::vec2(pc->getVelocity().x * 0.9, pc->getVelocity().y));
	}
	if (inputManager.isActionDown("MoveRight")) {
		pc->setVelocity(glm::vec2(100.f, pc->getVelocity().y));
	}
	else {
		pc->setVelocity(glm::vec2(pc->getVelocity().x * 0.9, pc->getVelocity().y));
	}
	if (inputManager.isActionPressed("Jump")) {
		pc->setVelocity(glm::vec2(pc->getVelocity().x, -400.f));
	}
}

void GameScene::testCollisionPairs() {
	auto collisionPairs = mContext.getPhysicsEngine().getCollisionPairs();
	for (auto& pair : collisionPairs) {
		spdlog::info("{} : 碰撞对: {} 和 {}", std::string(mLogTag), pair.first->getName(), pair.second->getName());
	}
}
} // namespace game::scene
