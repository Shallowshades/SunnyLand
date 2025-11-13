#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../object/game_object.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace engine::physics {
void PhysicsEngine::setGravity(const glm::vec2& gravity) {
	mGravity = gravity;
}

const glm::vec2& PhysicsEngine::getGravity() const {
	return mGravity;
}

void PhysicsEngine::setMaxSpeed(float maxSpeed) {
	mMaxSpeed = maxSpeed;
}

float PhysicsEngine::getMaxSpeed() const {
	return mMaxSpeed;
}

void PhysicsEngine::registerComponent(engine::component::PhysicsComponent* component) {
	mComponents.push_back(component);
	spdlog::trace("{} : 物理组件注册完成", std::string(mLogTag));
}

void PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent* component) {
	auto iter = std::remove(mComponents.begin(), mComponents.end(), component);
	mComponents.erase(iter, mComponents.end());
	spdlog::trace("{} : 物理组件注册完成", std::string(mLogTag));
}

void PhysicsEngine::update(float delta) {
	// 遍历所有注册的物理组件
	for (auto* physicsComponent : mComponents) {
		// 检测组件是否有效且组件是否启用
		if (!physicsComponent || !physicsComponent->isEnabled()) {
			continue;
		}

		// 应用重力 (如果组件受重力影响) : F = m * g
		if (physicsComponent->isUseGravity()) {
			physicsComponent->addForce(physicsComponent->getMass() * mGravity);
		}
		// TODO: 摩擦力, 风力等

		// 更新速度 : v += a * dt, 其中 a = F / m
		physicsComponent->setVelocity(physicsComponent->getVelocity() + (physicsComponent->getForce() / physicsComponent->getMass())* delta);
		physicsComponent->clearForce();

		// 更新位置 :  s += v * delta
		auto* transform = physicsComponent->getTransform();
		if (transform) {
			transform->translate(physicsComponent->getVelocity() * delta);
		}

		// 限制最大速度
		physicsComponent->setVelocity(glm::clamp(physicsComponent->getVelocity(), -mMaxSpeed, mMaxSpeed));
	}
}
}
