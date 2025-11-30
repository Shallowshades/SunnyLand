#include "animation_component.h"
#include "sprite_component.h"
#include "../object/game_object.h"
#include "../render/animation.h"
#include <spdlog/spdlog.h>

namespace engine::component {
void AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> animation) {
	if (!animation) {
		return;
	}

	std::string name = animation->getName();
	mAnimations[name] = std::move(animation);
	spdlog::debug("{} : 已将动画 '{}' 添加到 GameObject '{}'", std::string(mLogTag), name, (mOwner ? mOwner->getName() : "unknown"));
}

void AnimationComponent::playAnimation(const std::string& name) {
	auto iter = mAnimations.find(name);
	if (iter == mAnimations.end() || !iter->second) {
		spdlog::warn("{} : 未找到游戏对象 '{}' 的动画 '{}'", std::string(mLogTag), (mOwner ? mOwner->getName() : "unknown"), name);
		return;
	}

	// 如果已经在播放相同的动画, 不重新开始 (注释这一段则重新开始播放)
	if (mCurrentAnimation == iter->second.get() && mIsPlaying) {
		return;
	}

	mCurrentAnimation = iter->second.get();
	mAnimationTimer = 0.f;
	mIsPlaying = true;

	// 立即将精灵更新到第一帧
	if (mSpriteComponent && !mCurrentAnimation->getIsEmpty()) {
		const auto& firstFrame = mCurrentAnimation->getFrame(0.f);
		mSpriteComponent->setSourceRect(firstFrame.mSourceRect);
		spdlog::debug("{} : 游戏对象 '{}' 播放动画 '{}'", std::string(mLogTag), (mOwner ? mOwner->getName() : "unknown"), name);
	}
}

void AnimationComponent::stopAnimation() {
	mIsPlaying = false;
}

std::string AnimationComponent::getCurrentAnimationName() const {
	if (mCurrentAnimation) {
		return mCurrentAnimation->getName();
	}
	return "";
}

bool AnimationComponent::getIsPlaying() const {
	return mIsPlaying;
}

bool AnimationComponent::getIsAnimationFinished() const {
	// 如果没有当前动画 (说明从未调用过playAnimation), 或者当前动画是循环的, 则返回 false
	if (!mCurrentAnimation || mCurrentAnimation->getIsLooping()) {
		return false;
	}
	return mAnimationTimer >= mCurrentAnimation->getTotalDuration();
}

bool AnimationComponent::getIsOneShotRemoval() const {
	return mIsOneShotRemoval;
}

void AnimationComponent::setOneShotRemoval(bool isOneShotRemoval) {
	mIsOneShotRemoval = isOneShotRemoval;
}

void AnimationComponent::init() {
	if (!mOwner) {
		spdlog::error("{} : 没有所有者GameObject", std::string(mLogTag));
		return;
	}

	mSpriteComponent = mOwner->getComponent<SpriteComponent>();
	if (!mSpriteComponent) {
		spdlog::error("{} : 游戏对象 '{}' 需要精灵组件, 但未找到", std::string(mLogTag), mOwner->getName());
		return;
	}
}

void AnimationComponent::update(float delta, engine::core::Context& context) {
	// 如果没有正在播放的动画, 或者没有当前动画, 或者没有精灵组件, 或者当前动画没有帧, 则直接返回
	if (!mIsPlaying || !mCurrentAnimation || !mSpriteComponent || mCurrentAnimation->getIsEmpty()) {
		spdlog::trace("{} : 游戏对象动画组件更新时没有正在播放的动画或者精灵组件为空", std::string(mLogTag));
		return;
	}

	// 推进计时器
	mAnimationTimer += delta;
	// 根据时间获取当前帧
	const auto& currentFrame = mCurrentAnimation->getFrame(mAnimationTimer);
	// 更新精灵组件的源矩阵 (使用 SpriteComponent 的新方法)
	mSpriteComponent->setSourceRect(currentFrame.mSourceRect);

	// 检查非循环动画是否已结束
	if (!mCurrentAnimation->getIsLooping() && mAnimationTimer >= mCurrentAnimation->getTotalDuration()) {
		mIsPlaying = false;
		// 将时间限制在结束点
		mAnimationTimer = mCurrentAnimation->getTotalDuration();
		if (mIsOneShotRemoval) {
			mOwner->setNeedRemove(true);
		}
	}
}
} // namespace engine::component
