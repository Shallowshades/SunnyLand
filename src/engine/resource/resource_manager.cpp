#include "resource_manager.h"
#include "texture_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource {
ResourceManager::ResourceManager(SDL_Renderer* renderer) {
	// 初始化各个子系统 (如果出现错误会抛出异常,由上层捕获)
	mTextureManager = std::make_unique<TextureManager>(renderer);
	mAudioManager = std::make_unique<AudioManager>();
	mFontManager = std::make_unique<FontManager>();

	spdlog::trace("{} 构造成功", mLogTag);
	// RAII : 构造成功即代表资源管理器可以正常工作, 无需再初始化, 无需检查指针是否为空
}

ResourceManager::~ResourceManager() = default;

void ResourceManager::clear() {
	mFontManager->clearFonts();
	mAudioManager->clearSounds();
	mTextureManager->clearTextures();
	spdlog::trace("{} 清空资源", mLogTag);
}

SDL_Texture* ResourceManager::loadTexture(const std::string filePath) {
	// 构造函数确保了mTextManager不为空, 因此不需要判空, 避免性能浪费
	return mTextureManager->loadTexture(filePath);
}
SDL_Texture* ResourceManager::getTexture(const std::string filePath) {
	return mTextureManager->getTexture(filePath);
}
void ResourceManager::unloadTexture(const std::string& filePath) {
	mTextureManager->unloadTexture(filePath);
}
glm::vec2 ResourceManager::getTextureSize(const std::string& filePath) {
	return mTextureManager->getTextureSize(filePath);
}
void ResourceManager::clearTextures() {
	mTextureManager->clearTextures();
}
Mix_Chunk* ResourceManager::loadSound(const std::string& filePath) {
	return mAudioManager->loadSound(filePath);
}
Mix_Chunk* ResourceManager::getSound(const std::string& filePath) {
	return mAudioManager->getSound(filePath);
}
void ResourceManager::unloadSound(const std::string& filePath) {
	mAudioManager->unloadSound(filePath);
}
void ResourceManager::clearSounds() {
	mAudioManager->clearSounds();
}
Mix_Music* ResourceManager::loadMusic(const std::string& filePath) {
	return mAudioManager->loadMusic(filePath);
}
Mix_Music* ResourceManager::getMusic(const std::string& filePath) {
	return mAudioManager->getMusic(filePath);
}
void ResourceManager::unloadMusic(const std::string& filePath) {
	mAudioManager->unloadMusic(filePath);
}
void ResourceManager::clearMusic() {
	mAudioManager->clearMusic();
}
TTF_Font* ResourceManager::loadFont(const std::string& filePath, int pointSize) {
	return mFontManager->loadFont(filePath, pointSize);
}
TTF_Font* ResourceManager::getFont(const std::string& filePath, int pointSize) {
	return mFontManager->getFont(filePath, pointSize);
}
void ResourceManager::unloadFont(const std::string& filePath, int pointSize) {
	mFontManager->unloadFont(filePath, pointSize);
}
void ResourceManager::clearFonts() {
	mFontManager->clearFonts();
}
}
