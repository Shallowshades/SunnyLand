#include "texture_manager.h"
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource {
	TextureManager::TextureManager(SDL_Renderer* renderer) : mRenderer(renderer) {
		if (!mRenderer) {
			throw std::runtime_error(std::string(mLogTag) + std::string(" 构造失败: 渲染器指针为空"));
		}
		// SDL3中无需手动调用IMG_Init/IMG_Quit
		spdlog::trace("{} 构造成功", mLogTag);
	}

	SDL_Texture* TextureManager::loadTexture(const std::string& filePath) {
		// 检查是否已经被加载
		auto iter = mTextures.find(filePath);
		if (iter != mTextures.end()) {
			return iter->second.get();
		}

		// 如果没有加载则尝试加载纹理
		SDL_Texture* rawTexture = IMG_LoadTexture(mRenderer, filePath.c_str());
		if (!rawTexture) {
			spdlog::error("{} 加载纹理失败: '{}' : {}", mLogTag.data(), filePath, SDL_GetError());
			return nullptr;
		}

		// 使带有自定义删除器的unique_ptr存储加载的纹理
		mTextures.emplace(filePath, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(rawTexture));
		spdlog::debug("{} 成功加载并缓存纹理: {}", mLogTag.data(), filePath);
		return rawTexture;
	}

	SDL_Texture* TextureManager::getTexture(const std::string& filePath) {
		// 查找现有纹理
		auto iter = mTextures.find(filePath);
		if (iter != mTextures.end()) {
			return iter->second.get();
		}

		// 如果未找到, 尝试加载它
		spdlog::warn("{} 纹理 '{}' 未找到缓存, 尝试加载", mLogTag.data(), filePath);
		return loadTexture(filePath);
	}

	glm::vec2 TextureManager::getTextureSize(const std::string& filePath) {
		// 获取纹理
		SDL_Texture* texture = getTexture(filePath);
		if (!texture) {
			spdlog::error("{} 无法获取纹理: {}", mLogTag.data(), filePath);
			return glm::vec2(0);
		}

		// 获取纹理尺寸
		glm::vec2 size;
		if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
			spdlog::error("{} 无法查询纹理尺寸: {}", mLogTag.data(), filePath);
			return glm::vec2(0);
		}
		return size;
	}

	void TextureManager::unloadTexture(const std::string& filePath) {
		auto iter = mTextures.find(filePath);
		if (iter != mTextures.end()) {
			spdlog::debug("{} 卸载纹理: {}", mLogTag.data(), filePath);
			mTextures.erase(iter);
		}
		else {
			spdlog::warn("{} 尝试卸载不存在的纹理: {}", mLogTag.data(), filePath);
		}
	}

	void TextureManager::clearTextures() {
		if (!mTextures.empty()) {
			spdlog::debug("{} 正在清除所有{}个纹理", mLogTag.data(), mTextures.size());
			mTextures.clear();
		}
	}

} // namespace engine::resource
