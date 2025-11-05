/*****************************************************************//**
 * @file   sprite.h
 * @brief  精灵图类
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.11.05
 *********************************************************************/
#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <optional>
#include <string>
#include <SDL3/SDL_rect.h>

namespace engine::render {
/**
 * @brief 精灵类.
 */
class Sprite final {
public:
	/**
	 * @brief 构造一个精灵.
	 * 
	 * @param textureId	纹理资源的标识符. 不应为空
	 * @param sourceRect 可选的源矩阵, 定义要使用的纹理部分. 如果为std::nullopt, 则使用整个纹理
	 * @param isFlipped 是否水平翻转
	 */
	Sprite(const std::string& textureId, const std::optional<SDL_FRect>& sourceRect = std::nullopt, bool isFlipped = false);

	const std::string& getTextureId() const;
	const std::optional<SDL_FRect>& getSourceRect() const;
	bool isFlipped() const;

	void setTextureId(const std::string& textureId);
	void setSourceRect(const std::optional<SDL_FRect>& sourceRect);
	void setFlipped(bool flipped);

private:
	std::string mTextureId;
	std::optional<SDL_FRect> mSourceRect;
	bool mIsFlipped;
};
} // namespace engine::render

#endif // SPRITE_H
