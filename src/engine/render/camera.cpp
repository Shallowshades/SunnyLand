#include "camera.h"
#include "../utils/math.h"
#include <spdlog/spdlog.h>

namespace engine::render{
Camera::Camera(const glm::vec2& viewPortSize, const glm::vec2& position, const std::optional<engine::utils::Rect> limitBounds) 
	: mViewPortSize(viewPortSize), mPosition(position), mLimitBounds(limitBounds) 
{
	spdlog::trace("{} 初始化成功, 位置: ({}, {})", std::string(mLogTag), std::to_string(mPosition.x), std::to_string(mPosition.y));
}

void Camera::update(float deltaTime) {
	 // TODO 自动跟随目标
}

void Camera::move(const glm::vec2& offset) {
	mPosition += offset;
	clampPosition();
}

glm::vec2 Camera::worldToScreen(const glm::vec2& worldPosition) const {
	return worldPosition - mPosition;
}

glm::vec2 Camera::screenToWorld(const glm::vec2& screenPosition) const {
	return screenPosition + mPosition;
}

glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2& worldPosition, const glm::vec2& scrollFactor) const {
	// 相机位置应用滚动因子
	return worldPosition - mPosition * scrollFactor;
}

void Camera::setPosition(const glm::vec2& position) {
	mPosition = position;
	clampPosition();
}

void Camera::setLimitBounds(const engine::utils::Rect& bounds) {
	mLimitBounds = bounds;
	clampPosition(); // 设置边界后, 立即应用限制
}
const glm::vec2& Camera::getPosition() const {
	return mPosition;
}
std::optional<engine::utils::Rect> Camera::getLimitBounds() const {
	return mLimitBounds;
}
glm::vec2 Camera::getViewPortSize() const
{
	return mViewPortSize;
}
void Camera::clampPosition() {
	// 边界检查需要确保相机视图, position到position+viewPortSize要在limitBounds
	if (mLimitBounds.has_value() && mLimitBounds->size.x > 0 && mLimitBounds->size.y > 0) {
		// 计算允许的相机位置范围
		glm::vec2 minCameraPosition = mLimitBounds->position;
		glm::vec2 maxCameraPosition = mLimitBounds->position + mLimitBounds->size - mViewPortSize;

		// 确保maxCameraPosition不小于minCameraPosition (视口可能比世界还大)
		maxCameraPosition.x = std::max(minCameraPosition.x, maxCameraPosition.x);
		maxCameraPosition.y = std::max(minCameraPosition.y, maxCameraPosition.y);

		mPosition = glm::clamp(mPosition, minCameraPosition, maxCameraPosition);
	}
	// 如果mLimitBounds无效则不进行限制
}
}
