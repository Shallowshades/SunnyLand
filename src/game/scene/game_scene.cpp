#include "game_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
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

	// 测试创建游戏对象
	testCreateObject();

	Scene::init();
	spdlog::trace("{} 初始化完成", std::string(mLogTag));
}

void GameScene::update(float deltaTime){
	Scene::update(deltaTime);
}

void GameScene::render(){
	Scene::render();
}

void GameScene::handleInput(){
	Scene::handleInput();
	testCamera();
}

void GameScene::clean(){
	Scene::clean();
}

void GameScene::testCreateObject() {
	spdlog::trace("{} 创建测试对象", std::string(mLogTag));
	auto testObject = std::make_unique<engine::object::GameObject>("testObject");

	// 添加组件
	testObject->addComponent<engine::component::TransformComponent>(glm::vec2(100.f, 100.f));
	testObject->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", mContext.getResourceManager());
	
	// 添加到场景中
	addGameObject(std::move(testObject));
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
} // namespace game::scene
