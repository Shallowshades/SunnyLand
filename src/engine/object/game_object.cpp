#include "game_object.h"
#include "../render/renderer.h"
#include "../input/input_manager.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>

namespace engine::object {
GameObject::GameObject(const std::string& name, const std::string& tag)
	: mName(name), mTag(tag)
{
	spdlog::trace("{} created: {} {}", std::string(mLogTag), mName, mTag);
}

void GameObject::setName(const std::string& name){
	mName = name;
}

const std::string& GameObject::getName() const{
	return mName;
}

void GameObject::setTag(const std::string& tag){
	mTag = tag;
}

const std::string& GameObject::getTag() const{
	return mTag;
}

void GameObject::setNeedRemove(bool needRemove) {
	mNeedRemove = needRemove;
}

bool GameObject::isNeedRemove() const{
	return mNeedRemove;
}

void GameObject::update(float deltaTime, engine::core::Context& context){
	// 遍历所有组件并调用他们的update方法
	for (auto& pair : mComponents) {
		pair.second->update(deltaTime, context);
	}
}

void GameObject::render(engine::core::Context& context){
	// 遍历所有组件并调用他们的render方法
	for (auto& pair : mComponents) {
		pair.second->render(context);
	}
}

void GameObject::clean(){
	spdlog::trace("{} cleaning GameObject...", std::string(mLogTag));
	// 遍历所有组件并调用他们的clean方法
	for (auto& pair : mComponents) {
		pair.second->clean();
	}
	mComponents.clear();
}

void GameObject::handleInput(engine::core::Context& context){
	// 遍历所有组件并调用他们的handleInput方法
	for (auto& pair : mComponents) {
		pair.second->handleInput(context);
	}
}
} // namespace engine::object
