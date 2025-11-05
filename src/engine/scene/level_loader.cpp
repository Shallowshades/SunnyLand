#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/transform_component.h"
#include "../object/game_object.h"
#include "../object/game_object.h"
#include "../scene/scene.h"
#include "../core/context.h"
#include "../render/sprite.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include <filesystem>

namespace engine::scene {
bool LevelLoader::loadLevel(const std::string& mapPath, Scene& scene) {
	mMapPath = mapPath;
	// 1. 加载json文件
	std::ifstream file(mMapPath);
	if (!file.is_open()) {
		spdlog::error("{} : 无法打开关卡文件: {}", std::string(mLogTag), mapPath);
		return false;
	}

	// 2. 解析json数据
	nlohmann::json data;
	try {
		file >> data;
	}
	catch (const nlohmann::json::parse_error& e) {
		spdlog::error("{} : 解析json数据失败: {}", std::string(mLogTag), e.what());
		return false;
	}

	// 3.加载图层数据
	if (!data.contains("layers") || !data["layers"].is_array()) {
		spdlog::error("{} : 地图文件 {} 缺少或者无效的 'layers' 数组.", std::string(mLogTag), mMapPath);
		return false;
	}
	for (const auto& layerData : data["layers"]) {
		// 获取各图层对象中的类型type字段
		std::string layerType = layerData.value("type", "none");
		if (!layerData.value("visible", true)) {
			spdlog::info("{} : 图层 '{}' 不可见, 跳过加载.", std::string(mLogTag), layerData.value("name", "Unnamed"));
			continue;
		}

		// 根据图层类型决定加载方法
		if (layerType == "imagelayer") {
			loadImageLayer(layerData, scene);
		}
		else if (layerType == "tilelayer") {
			loadTileLayer(layerData, scene);
		}
		else if (layerType == "objectgroup") {
			loadObjectLayer(layerData, scene);
		}
		else {
			spdlog::warn("{} : 不支持的图层类型: {}", std::string(mLogTag), layerType);
		}
	}

	spdlog::info("{} : 关卡加载器完成: {}", std::string(mLogTag), mMapPath);
	return true;
}

void LevelLoader::loadImageLayer(const nlohmann::json& layerJson, Scene& scene) {
	// 获取纹理相对路径 (会自动处理'\/'符号)
	const std::string& imagePath = layerJson.value("image", "");
	if (imagePath.empty()) {
		spdlog::error("{} : 图层 '{}' 缺少 'image' 属性.", std::string(mLogTag), layerJson.value("name", "Unnamed"));
		return;
	}

	// 解析图片路径
	auto textureId = resolvePath(imagePath);
	// 获取图层偏移量
	const glm::vec2 offset = glm::vec2(layerJson.value("offsetx", 0.f), layerJson.value("offsety", 0.f));
	// 获取视差因子及重复标志
	const glm::vec2 scrollFactor = glm::vec2(layerJson.value("parallaxx", 1.f), layerJson.value("parallaxy", 1.f));
	const glm::bvec2 repeat = glm::bvec2(layerJson.value("repeatx", false), layerJson.value("repeaty", false));
	// 获取图层名称
	const std::string& layerName = layerJson.value("name", "Unnamed");

	/// TODO: 待获取其他属性
	
	// 创建游戏对象并添加Transform, Parallax 组件
	auto gameObject = std::make_unique<engine::object::GameObject>(layerName);
	gameObject->addComponent<engine::component::TransformComponent>(offset);
	gameObject->addComponent<engine::component::ParallaxComponent>(textureId, scrollFactor, repeat);

	// 添加到场景中
	scene.addGameObject(std::move(gameObject));
	spdlog::info("{} : 加载图层: '{}' 完成", std::string(mLogTag), layerName);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layerJson, Scene& scene) {
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson, Scene& scene) {
}

std::string LevelLoader::resolvePath(std::string imagePath) {
	try {
		// 获取地图文件的父目录（相对于可执行文件） “assets/maps/level1.tmj” -> “assets/maps”
		auto mapDir = std::filesystem::path(mMapPath).parent_path();
		// 合并路径（相对于可执行文件）并返回。 
		// std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，得到一个干净的路径
		auto finalPath = std::filesystem::canonical(mapDir / imagePath);
		return finalPath.string();
	}
	catch (const std::exception& e) {
		spdlog::error("{} : 路径解析失败: {}", std::string(mLogTag), e.what());
		return imagePath;
	}
}
} // namespace engine::scene
