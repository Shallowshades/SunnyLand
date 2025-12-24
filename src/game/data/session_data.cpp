#include "session_data.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace game::data {
void SessionData::setCurrentHealth(int health) {
	// 将生命值限制在 0 和 max_health_ 之间
	mCurrentHealth = glm::clamp(health, 0, mMaxHealth);
}

void SessionData::setMaxHealth(int maxHealth) {
	if (maxHealth > 0) {
		mMaxHealth = maxHealth;
		// 确保当前生命值不超过新的最大生命值
		setCurrentHealth(mCurrentHealth);
	}
	else {
		spdlog::warn("尝试将最大生命值设置为非正数: {}", maxHealth);
	}
}

void SessionData::addScore(int scoreToAdd) {
	mCurrentScore += scoreToAdd;
	setHighScore(glm::max(mHighScore, mCurrentScore)); // 如果当前分数超过最高分，则更新最高分
}

void SessionData::setHighScore(int highScore) {
	mHighScore = highScore;
}

void SessionData::setLevelHealth(int levelHealth) {
	mLevelHealth = levelHealth;
}

void SessionData::setLevelScore(int levelScore) {
	mLevelScore = levelScore;
}

void SessionData::setMapPath(const std::string& mapPath) {
	mMapPath = mapPath;
}

void SessionData::reset() {
	mCurrentHealth = mMaxHealth;
	mCurrentScore = 0;
	mLevelHealth = 3;
	mLevelScore = 0;
	mMapPath = "assets/maps/level1.tmj";
	spdlog::info("SessionData reset.");
}

void SessionData::setNextLevel(const std::string& mapPath) {
	mMapPath = mapPath;
	mLevelHealth = mCurrentHealth;
	mLevelScore = mCurrentScore;
}

bool SessionData::saveToFile(const std::string& filename) const {
	nlohmann::json data;
	try {
		// 将成员变量序列化到 JSON 对象中
		data["level_score"] = mLevelScore;
		data["level_health"] = mLevelHealth;
		data["max_health"] = mMaxHealth;
		data["high_score"] = mHighScore;
		data["map_path"] = mMapPath;

		// 打开文件进行写入
		std::ofstream ofs(filename);
		if (!ofs.is_open()) {
			spdlog::error("无法打开存档文件进行写入: {}", filename);
			return false;
		}

		// 将 JSON 对象写入文件（使用4个空格进行美化输出）
		ofs << data.dump(4);
		ofs.close(); // 确保文件关闭

		spdlog::info("游戏数据成功存储到: {}", filename);
		return true;
	}
	catch (const std::exception& e) {
		spdlog::error("存档时出现错误 {}: {}", filename, e.what());
		return false;
	}
}

bool SessionData::loadFromFile(const std::string& filename) {
	try {
		// 打开文件进行读取
		std::ifstream ifs(filename);
		if (!ifs.is_open()) {
			spdlog::warn("读档时找不到文件: {}", filename);
			// 如果存档文件不存在，这不一定是错误
			return false;
		}

		// 从文件解析 JSON 数据
		nlohmann::json j;
		ifs >> j;
		ifs.close(); // 读取完成后关闭文件

		mCurrentScore = mLevelScore = j.value("level_score", 0);
		mCurrentHealth = mLevelHealth = j.value("level_health", 3);
		mMaxHealth = j.value("max_health", 3); // 使用合理的默认值
		mHighScore = j.value("high_score", 0);
		mMapPath = j.value("map_path", "assets/maps/level1.tmj"); // 默认起始地图

		spdlog::info("游戏数据成功加载: {}", filename);
		return true;
	}
	catch (const std::exception& e) {
		spdlog::error("读档时出现错误 {}: {}", filename, e.what());
		reset();
		return false;
	}
}

int SessionData::getCurrentHealth() const {
	return mCurrentHealth;
}

int SessionData::getMaxHealth() const {
	return mMaxHealth;
}

int SessionData::getCurrentScore() const {
	return mCurrentScore;
}

int SessionData::getHighScore() const {
	return mHighScore;
}

int SessionData::getLevelHealth() const {
	return mLevelHealth;
}

int SessionData::getLevelScore() const {
	return mLevelScore;
}

const std::string& SessionData::getMapPath() const {
	return mMapPath;
}
} //namespace game::data {
