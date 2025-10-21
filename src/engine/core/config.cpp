#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace engine::core {
Config::Config(const std::string& filePath) {
	loadFromFile(filePath);
}

bool Config::loadFromFile(const std::string& filePath) {
	std::ifstream file(filePath);

	// 使用默认值
	if (!file.is_open()) {
		spdlog::warn("{} 配置文件 '{}' 未找到. 使用默认设置并创建默认配置文件.", std::string(mLogTag), filePath);
		if (!saveToFile(filePath)) {
			spdlog::error("{} 无法创建默认配置文件 '{}'.", std::string(mLogTag), filePath);
			return false;
		}
		return false;
	}

	// 使用配置文件值
	try {
		nlohmann::json data;
		file >> data;
		fromJson(data);
		spdlog::info("{} 成功从 '{}' 加载配置.", std::string(mLogTag), filePath);
		return true;
	}
	catch (const std::exception& e) {
		spdlog::error("{} 读取配置文件 '{}' 时出错: {}. 使用默认配置", std::string(mLogTag), filePath, e.what());
	}
	return false;
}

bool Config::saveToFile(const std::string& filePath) {
	std::ofstream file(filePath);
	if (!file.is_open()) {
		spdlog::error("{} 无法打开配置文件 '{}' 进行写入", std::string(mLogTag), filePath);
		return false;
	}

	try {
		nlohmann::ordered_json data = toJson();
		file << data.dump(4);
		spdlog::info("{} 成功将配置保存到 '{}'.", std::string(mLogTag), filePath);
		return true;
	}
	catch (const std::exception& e) {
		spdlog::error("{} 写入配置文件 '{}' 时出错: {}", std::string(mLogTag), filePath, e.what());
	}
	return false;
}

void Config::fromJson(const nlohmann::json& data) {
	// 窗口设置
	if (data.contains("window")) {
		const auto& windowConfig = data["window"];
		mWindowTitle = windowConfig.value("title", mWindowTitle);
		mWindowWidth = windowConfig.value("width", mWindowWidth);
		mWindowHeight = windowConfig.value("height", mWindowHeight);
		mWindowResizable = windowConfig.value("resizable", mWindowResizable);
	}

	// 图像设置
	if (data.contains("graphics")) {
		const auto& graphicsConifg = data["graphics"];
		mVsyncEnabled = graphicsConifg.value("vsync", mVsyncEnabled);
	}

	// 帧率设置
	if (data.contains("performance")) {
		const auto& performanceConfig = data["performance"];
		mTargetFps = performanceConfig.value("target_fps", mTargetFps);
		if (mTargetFps < 0) {
			spdlog::warn("{} 目标FPS不能为负数. 设置为0为无限制.", std::string(mLogTag));
			mTargetFps = 0;
		}
	}

	// 音频设置
	if (data.contains("audio")) {
		const auto& audioConfig = data["audio"];
		mMusicVolume = audioConfig.value("music_volume", mMusicVolume);
		mSoundVolume = audioConfig.value("sound_volume", mSoundVolume);
	}

	// 按键映射
	if (data.contains("input_mappings") && data["input_mappings"].is_object()) {
		const auto& mappingsJson = data["input_mappings"];
		try {
			// 直接尝试从JSON对象转换为map<string, vector<string>>
			auto inputMappings = mappingsJson.get<std::unordered_map<std::string, std::vector<std::string>>>();
			// 如果成功, 则将input_mappings 移动到 mInputMappings
			mInputMappings = std::move(inputMappings);
			spdlog::trace("{} 成功从配置文件加载输入映射.", std::string(mLogTag));
		} 
		catch (const std::exception& e) {
			spdlog::warn("{} 配置加载警告: 解析'input_mappings'时发生异常. 使用默认映射. 错误: {}", std::string(mLogTag), e.what());
		}
	}
	else {
		spdlog::trace("{} 配置跟踪: 未找到 'input_mappings' 部分或不是对象. 使用头文件中定义的默认映射.", std::string(mLogTag));
	}
}

nlohmann::ordered_json Config::toJson() const {
	return nlohmann::ordered_json{
		{
			"window", {
				{ "title", mWindowTitle },
				{ "width", mWindowWidth },
				{ "height", mWindowHeight },
				{ "resizable", mWindowResizable }
			}
		},
		{
			"graphics", {
				{ "vsync", mVsyncEnabled }
			}
		},
		{
			"performance", {
				{ "target_fps", mTargetFps }
			}
		},
		{
			"audio", {
				{ "music_volume", mMusicVolume },
				{ "sound_volume", mMusicVolume }
			}
		},
		{
			"input_mappings", {
				mInputMappings
			}
		}
	};
}
} // namespace engine::core
