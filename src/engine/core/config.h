/*****************************************************************//**
 * @file   config.h
 * @brief  配置类
 * @version 1.0
 *
 * @author Shallowshades
 * @date   2025.10.21
 *********************************************************************/

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>	// nlohmann_json

namespace engine::core {
/**
 * @brief 管理应用程序的配置设置
 * 
 * 提供配置项的默认值, 并支持从JSON文件加载/保存配置
 * 如果加载失败或者文件不存在, 将使用默认值.
 */
class Config final {
public:
	explicit Config(const std::string& filePath);						///< @brief 构造函数, 指定配置文件路径
	[[nodiscard]] bool loadFromFile(const std::string& filePath);		///< @brief 从指定的JSON文件读取配置; 不可忽略返回值
	[[nodiscard]] bool saveToFile(const std::string& filePath);			///< @brief 将当前配置保存到指定的JSON文件; 不可忽略返回值

private:
	void fromJson(const nlohmann::json& data);							///< @brief 删除拷贝构造
	nlohmann::ordered_json toJson() const;								///< @brief 删除拷贝赋值构

	// 禁用拷贝和移动语义
	Config(const Config&) = delete;										///< @brief 删除拷贝构造
	Config& operator=(const Config&) = delete;							///< @brief 删除拷贝赋值构造
	Config(Config&&) = delete;											///< @brief 删除移动构造
	Config& operator=(Config&&) = delete;								///< @brief 删除移动赋值构造

public:
	static constexpr std::string_view mLogTag = "Config";				///< @brief 日志标记

	std::string  mWindowTitle = "SunnyLand";							///< @brief 窗口名称
	int mWindowWidth = 1280;											///< @brief 窗口宽度
	int mWindowHeight = 720;											///< @brief 窗口高度
	bool mWindowResizable = true;										///< @brief 窗口是否可以调整尺寸
	bool mVsyncEnabled = true;											///< @brief 是否启动垂直同步
	int mTargetFps = 144;												///< @brief 性能设置: 目标FPS, 设置0表示不限制
	float mMusicVolume = 0.5f;											///< @brief 音频设置: 音乐大小
	float mSoundVolume = 0.5f;											///< @brief 音频设置: 音效大小

	// 存储动作名称到SDL_Scancode名称列表的映射
	std::unordered_map<std::string, std::vector<std::string>> mInputMappings = {
		// 提供一些合理的默认值, 以防配置文件加载失败或者缺少此部分 
		{"MoveLeft", {"A", "Left"}},
		{"MoveRight", {"D", "Right"}},
		{"MoveUp", {"W", "Up"}},
		{"MoveDown", {"S", "Down"}},
		{"Jump", {"J", "Space"}},
		{"Attack", {"K", "MouseLeft"}},
		{"Pause", {"P", "Escape"}},
		// 可以继续添加更多的默认动作
	};
};
} // namespace engine::core

#endif // CONFIG_H