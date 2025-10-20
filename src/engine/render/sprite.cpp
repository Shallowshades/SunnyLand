#include "sprite.h"

namespace engine::render {
Sprite::Sprite(const std::string& textureId, const std::optional<SDL_FRect>& sourceRect, bool isFlipped)
	: mTextureId(textureId), mSourceRect(sourceRect), mIsFlipped(isFlipped) {}

const std::string& Sprite::getTextureId() const {
	return mTextureId;
}
const std::optional<SDL_FRect>& Sprite::getSourceRect() const {
	return mSourceRect;
}
bool Sprite::isFlipped() const {
	return mIsFlipped;
}
void Sprite::setTextureId(const std::string& textureId) {
	mTextureId = textureId;
}
void Sprite::setSourceRect(const std::optional<SDL_FRect>& sourceRect) {
	mSourceRect = sourceRect;
}
void Sprite::setFlipped(bool flipped) {
	mIsFlipped = flipped;
}
} // namespace engine::render
