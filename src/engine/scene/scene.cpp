#include "scene.h"
#include "../object/game_object.h"
#include "scene_manager.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace engine::scene {
engine::scene::Scene::Scene(std::string name, engine::core::Context& context, engine::scene::SceneManager& sceneManager)
	: mSceneName(std::move(name)), mContext(context), mSceneManager(sceneManager), mIsInitialized(false)
{
	spdlog::trace("{} : {} 构造完成", std::string(mLogTag), mSceneName);
}
Scene::~Scene() = default;

void Scene::init() {
	mIsInitialized = true;
	spdlog::trace("{} : {} 初始化完成", std::string(mLogTag), mSceneName);
}

void Scene::update(float deltaTime) {
	if (!mIsInitialized) {
		return;
	}

	// 更新所有游戏对象, 并删除需要移除的对象
	for (auto iter = mGameObjects.begin(); iter != mGameObjects.end();) {
		// 安全更新游戏对象
		if (*iter && !(*iter)->isNeedRemove()) {
			(*iter)->update(deltaTime, mContext);
			++iter;
		}
		else { 
			// 安全删除需要移除的对象
			if (*iter) {
				(*iter)->clean();
			}
			iter = mGameObjects.erase(iter);
		}
	}

	processPendingAdditions();
}

void Scene::render() {
	if (!mIsInitialized) {
		return;
	}

	// 渲染所有游戏对象
	for (const auto& obj : mGameObjects) {
		if (obj) {
			obj->render(mContext);
		}
	}
}

void Scene::handleInput() {
	if (!mIsInitialized) {
		return;
	}

	// 遍历所有游戏对象, 并删除需要移除的对象
	for (auto iter = mGameObjects.begin(); iter != mGameObjects.end();) {
		if (*iter && !(*iter)->isNeedRemove()) {
			(*iter)->handleInput(mContext);
			++iter;
		}
		else {
			if (*iter) {
				(*iter)->clean();
			}
			iter = mGameObjects.erase(iter);
		}
	}
}

void Scene::clean() {
	if (!mIsInitialized) {
		return;
	}

	for (const auto& obj : mGameObjects) {
		if (obj) {
			obj->clean();
		}
	}
	mGameObjects.clear();

	mIsInitialized = false;
	spdlog::trace("{} : {} 清理完成.", std::string(mLogTag), mSceneName);
}

void Scene::addGameObject(std::unique_ptr<engine::object::GameObject>&& gameObject) {
	if (gameObject) {
		mGameObjects.push_back(std::move(gameObject));
	}
	else {
		spdlog::warn("{} : {} 尝试添加空对象", std::string(mLogTag), mSceneName);
	}
}

void Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& gameObject) {
	if (gameObject) {
		mPendingAdditions.push_back(std::move(gameObject));
	}
	else {
		spdlog::warn("{} : {} 尝试添加空对象", std::string(mLogTag), mSceneName);
	}
}

void Scene::removeGameObject(engine::object::GameObject* gameObjectPtr) {
	if (!gameObjectPtr) {
		spdlog::warn("{} : {} 尝试移除空对象", std::string(mLogTag), mSceneName);
		return;
	}

	// erase-remove 移除法不可用, 因为智能指针与裸指针无法比较
	// 需要使用std::remove_if和lambda表达式自定义比较的方式
	auto iter = std::remove_if(mGameObjects.begin(), mGameObjects.end(), [gameObjectPtr](const std::unique_ptr<engine::object::GameObject>& p) {
		return p.get() == gameObjectPtr;
		});
	if (iter != mGameObjects.end()) {
		(*iter)->clean();
		mGameObjects.erase(iter, mGameObjects.end());
		spdlog::trace("{} : {} 移除游戏对象.", std::string(mLogTag), mSceneName);
	}
	else {
		spdlog::warn("{} : {} 中不存在应删除的游戏对象", std::string(mLogTag), mSceneName);
	}
}

void Scene::safeRemoveGameObject(engine::object::GameObject* gameObjectPtr) {
	gameObjectPtr->setNeedRemove(true);
}

const std::vector<std::unique_ptr<engine::object::GameObject>>& Scene::getGameObjects() const {
	return mGameObjects;
}

engine::object::GameObject* Scene::findGameObjectByName(const std::string& name) const {
	// 找到第一个符合条件的游戏对象就返回
	for (const auto& obj : mGameObjects) {
		if (obj && obj->getName() == name) {
			return obj.get();
		}
	}
	return nullptr;
}

void Scene::setName(const std::string& name) {
	mSceneName = name;
}

const std::string& Scene::getName() const {
	return mSceneName;
}

void Scene::setIsInitialized(bool initialized) {
	mIsInitialized = initialized;
}

bool Scene::getIsInitialized() const {
	return mIsInitialized;
}

engine::core::Context& Scene::getContext() const {
	return mContext;
}

engine::scene::SceneManager& Scene::getSceneManager() const {
	return mSceneManager;
}

std::vector<std::unique_ptr<engine::object::GameObject>>& Scene::getGameObjects() {
	return mGameObjects;
}

void Scene::processPendingAdditions() {
	// 处理待添加的游戏对象
	for (auto& gameObject : mPendingAdditions) {
		addGameObject(std::move(gameObject));
	}
	mPendingAdditions.clear();
}
}
