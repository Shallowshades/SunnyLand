#include "physics_engine.h"
#include "collision.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../component/tilelayer_component.h"
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

void PhysicsEngine::registerCollisionLayer(engine::component::TileLayerComponent* layer) {
	layer->setPhysicsEngine(this);
	mCollisionTileLayers.push_back(layer);
	spdlog::trace("{} : 碰撞瓦片图层注册完成", std::string(mLogTag));
}

void PhysicsEngine::unregisterCollisionLayer(engine::component::TileLayerComponent* layer) {
	auto iter = std::remove(mCollisionTileLayers.begin(), mCollisionTileLayers.end(), layer);
	mCollisionTileLayers.erase(iter, mCollisionTileLayers.end());
	spdlog::trace("{} : 碰撞瓦片图层注销完成", std::string(mLogTag));
}

void PhysicsEngine::update(float delta) {
	// 每帧开始时, 清空碰撞对列表
	mCollisionPairs.clear();

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
		physicsComponent->setVelocity(physicsComponent->getVelocity() + (physicsComponent->getForce() / physicsComponent->getMass()) * delta);
		physicsComponent->clearForce();

		//// 更新位置 :  s += v * delta
		//auto* transform = physicsComponent->getTransform();
		//if (transform) {
		//	transform->translate(physicsComponent->getVelocity() * delta);
		//}

		//// 限制最大速度
		//physicsComponent->setVelocity(glm::clamp(physicsComponent->getVelocity(), -mMaxSpeed, mMaxSpeed));

		// 处理对象间的碰撞
		resolveTileCollisions(physicsComponent, delta);
	}

	// 处理对象间碰撞
	checkObjectCollisions();
}

void PhysicsEngine::checkObjectCollisions() {
	// 两层循环遍历所有包含物理组件的游戏对象
	for (size_t i = 0; i < mComponents.size(); ++i) {
		auto* aPC = mComponents[i];
		if (!aPC || !aPC->isEnabled()) {
			continue;
		}

		auto* aObject = aPC->getOwner();
		if (!aObject) {
			continue;
		}

		auto* aCC = aObject->getComponent<engine::component::ColliderComponent>();
		if (!aCC || !aCC->getIsActive()) {
			continue;
		}

		for (size_t j = i + 1; j < mComponents.size(); ++j) {
			auto* bPC = mComponents[j];
			if (!bPC || !bPC->isEnabled()) {
				continue;
			}

			auto* bObject = bPC->getOwner();
			if (!bObject) {
				continue;
			}

			auto* bCC = bObject->getComponent<engine::component::ColliderComponent>();
			if (!bCC || !bCC->getIsActive()) {
				continue;
			}


			// 保护逻辑结束, 开始执行碰撞检测逻辑
			if (collision::checkCollision(*aCC, *bCC)) {
				// TODO: 碰撞对的条件

				mCollisionPairs.emplace_back(aObject, bObject);
			}
		}
	}
}

void PhysicsEngine::resolveTileCollisions(engine::component::PhysicsComponent* pc, float delta) {
	// 检查组件是否有效
	auto* object = pc->getOwner();
	if (!object) {
		return;
	}

	auto* tc = object->getComponent<engine::component::TransformComponent>();
	auto* cc = object->getComponent<engine::component::ColliderComponent>();
	if (!tc || !cc || !cc->getIsActive() || cc->getIsTrigger()) {
		return;
	}
	// 使用最小包围盒进行碰撞检测
	auto worldAABB = cc->getWorldAABB();
	auto objectPosition = worldAABB.position;
	auto objectSize = worldAABB.size;
	if (worldAABB.size.x <= 0.f || worldAABB.size.y <= 0.f) {
		return;
	}

	auto tolerance = 1.f;
	auto ds = pc->getVelocity() * delta;
	auto newObjectPosition = objectPosition + ds;

	// 遍历所有注册的碰撞瓦片层
	for (auto* layer : mCollisionTileLayers) {
		if (!layer) {
			continue;
		}

		auto tileSize = layer->getTileSize();
		// 轴分离碰撞检测: 
		// 先检查X方向是否有碰撞(Y方向使用初始值ObjectPosition.Y)
		// 轴分离碰撞检测：再检查Y方向是否有碰撞 (x方向使用初始值ObjectPosition.X)

		// 向右移动
		if (ds.x > 0.f) {
			// 检测右侧碰撞, 需要分别测试右上和右下角
			auto rightTopX = newObjectPosition.x + objectSize.x;
			auto tileX = static_cast<int>(floor(rightTopX / tileSize.x));
			// y 方向坐标有两个, 右上和右下
			auto tileY = static_cast<int>(floor(objectPosition.y / tileSize.y));
			auto tileTypeTop = layer->getTileTypeAt(glm::vec2(tileX, tileY));
			auto tileYBottom = static_cast<int>(floor((objectPosition.y + objectSize.y - tolerance) / tileSize.y));
			auto tileTypeBottom = layer->getTileTypeAt(glm::vec2(tileX, tileYBottom));

			if (tileTypeTop == engine::component::TileType::SOLID || tileTypeBottom == engine::component::TileType::SOLID) {
				// 撞墙: X 速度归零, X 方向移动到贴着墙的位置
				newObjectPosition.x = tileX * tileSize.x - objectSize.x;
				pc->setVelocity(glm::vec2(0.f, pc->getVelocity().y));
			}
		}

		// 向左移动
		else if (ds.x < 0.f) {
			// 检测左侧碰撞, 需要分别测试左上角和左下角
			auto leftTopX = newObjectPosition.x;
			auto tileX = static_cast<int>(floor(leftTopX / tileSize.x));
			// y 方向坐标有两个, 左上和左下
			auto tileY = static_cast<int>(floor(objectPosition.y / tileSize.y));
			auto tileTypeTop = layer->getTileTypeAt(glm::vec2(tileX, tileY));
			auto tileYBottom = static_cast<int>(floor((objectPosition.y + objectSize.y - tolerance) / tileSize.y));
			auto tileTypeBottom = layer->getTileTypeAt(glm::vec2(tileX, tileYBottom));

			if (tileTypeTop == engine::component::TileType::SOLID || tileTypeBottom == engine::component::TileType::SOLID) {
				// 撞墙: X 速度归零, X 方向移动到贴着墙的位置
				newObjectPosition.x = static_cast<float>((tileX + 1) * tileSize.x);
				pc->setVelocity(glm::vec2(0.f, pc->getVelocity().y));
			}
		}

		// 向下移动
		if (ds.y > 0.f) {
			// 检测底部碰撞, 需要分别测试左下和右下角
			auto bottomLeftY = newObjectPosition.y + objectSize.y;
			auto tileY = static_cast<int>(floor(bottomLeftY / tileSize.y));
			// X 方向坐标有两个, 左下和右下
			auto tileX = static_cast<int>(floor(objectPosition.x / tileSize.x));
			auto tileTypeLeft = layer->getTileTypeAt(glm::vec2(tileX, tileY));
			auto tileXRight = static_cast<int>(floor((objectPosition.x + objectSize.x - tolerance) / tileSize.x));
			auto tileTypeRight = layer->getTileTypeAt(glm::vec2(tileXRight, tileY));

			if (tileTypeLeft == engine::component::TileType::SOLID || tileTypeRight == engine::component::TileType::SOLID) {
				// 触底: Y 速度归零, Y 方向移动到贴着墙的位置
				newObjectPosition.y = tileY * tileSize.y - objectSize.y;
				pc->setVelocity(glm::vec2(pc->getVelocity().x, 0.f));
			}
		}

		// 向上移动
		else if (ds.y < 0.f) {
			// 检测顶部碰撞, 需要分别测试左上角和右上角
			auto topLeftY = newObjectPosition.y;
			auto tileY = static_cast<int>(floor(topLeftY / tileSize.y));
			// X 方向坐标有两个, 左上和右上
			auto tileX = static_cast<int>(floor(objectPosition.x / tileSize.x));
			auto tileTypeLeft = layer->getTileTypeAt(glm::vec2(tileX, tileY));
			auto tileXRight = static_cast<int>(floor((objectPosition.x + objectSize.x - tolerance) / tileSize.x));
			auto tileTypeRight = layer->getTileTypeAt(glm::vec2(tileXRight, tileY));

			if (tileTypeLeft == engine::component::TileType::SOLID || tileTypeRight == engine::component::TileType::SOLID) {
				// 撞顶: Y 速度归零, Y 方向移动到贴着墙的位置
				newObjectPosition.y = static_cast<float>((tileY + 1) * tileSize.y);
				pc->setVelocity(glm::vec2(pc->getVelocity().x, 0.f));
			}
		}

		// 更新物体位置, 并限制最大速度
		tc->setPosition(newObjectPosition);
		pc->setVelocity(glm::clamp(pc->getVelocity(), -mMaxSpeed, mMaxSpeed));
	}
}
}
