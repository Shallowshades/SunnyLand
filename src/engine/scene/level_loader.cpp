#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/transform_component.h"
#include "../component/tilelayer_component.h"
#include "../object/game_object.h"
#include "../object/game_object.h"
#include "../scene/scene.h"
#include "../core/context.h"
#include "../render/sprite.h"
#include "../utils/math.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include <filesystem>

namespace engine::scene {
bool LevelLoader::loadLevel(const std::string& mapPath, Scene& scene) {
	// 1. 加载json文件
	std::ifstream file(mapPath);
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
	
	// 3.获取基本地图信息 (名称, 地图尺寸, 瓦片尺寸)
	mMapPath = mapPath;
	mMapSize = glm::ivec2(data.value("width", 0), data.value("height", 0));
	mTileSize = glm::ivec2(data.value("tilewidth", 0), data.value("tileheight", 0));

	// 4.加载tileset数据
	if (data.contains("tilesets") && data["tilesets"].is_array()) {
		for (const auto& tileset : data["tilesets"]) {
			if (!tileset.contains("source") || !tileset["source"].is_string() || !tileset.contains("firstgid") || !tileset["firstgid"].is_number_integer()) {
				spdlog::error("{} : tilesets 对象中缺少有效 'source' 或 'firstgid' 字段", std::string(mLogTag));
				continue;
			}
			auto tilesetPath = resolvePath(tileset["source"], mMapPath);
			int firstGid = tileset["firstgid"];
			loadTileset(tilesetPath, firstGid);
		}
	}

	// 5.加载图层数据
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
	auto textureId = resolvePath(imagePath, mMapPath);
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
	if (!layerJson.contains("data") || !layerJson["data"].is_array()) {
		spdlog::error("{} 图层 '{}' 缺少 'data' 属性.", std::string(mLogTag), layerJson.value("name", "Unnamed"));
		return;
	}
	// 准备 TileInfo Vector (瓦片数量 = 地图宽度 * 地图高度)
	std::vector<engine::component::TileInfo> tiles;
	tiles.reserve(mMapSize.x * mMapSize.y);

	// 获取图层数据 (瓦片id列表)
	const auto& data = layerJson["data"];

	// 根据gid获取必要信息, 并依次填充 TileInfo Vector
	for (const auto& gid : data) {
		tiles.push_back(getTileInfoByGid(gid));
	}

	// 获取图层名称
	const std::string& layerName = layerJson.value("name", "Unnamed");
	// 创建游戏对象
	auto gameObject = std::make_unique<engine::object::GameObject>(layerName);
	// 添加TileLayer组件
	gameObject->addComponent<engine::component::TileLayerComponent>(mTileSize, mMapSize, std::move(tiles));
	// 添加到场景
	scene.addGameObject(std::move(gameObject));
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson, Scene& scene) {
}

engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid) {
	if (gid == 0) {
		return engine::component::TileInfo();
	}
	
	// upper_bound: 查找mTitlesetData中键大于gid的第一个元素, 返回迭代器
	auto tilesetIter = mTilesetData.upper_bound(gid);
	if (tilesetIter == mTilesetData.begin()) {
		spdlog::error("{} gid 为 {} 的瓦片未找到图块集.", std::string(mLogTag), gid);
		return engine::component::TileInfo();
	}
	--tilesetIter; // 前移找到不大于gid的最近一个元素

	const auto& tileset = tilesetIter->second;
	// 计算瓦片在图块集中的局部ID
	auto localId = gid - tilesetIter->first;
	// 获取图块集文件路径
	const std::string filePath = tileset.value("file_path", "");
	if (filePath.empty()) {
		spdlog::error("{} : Tileset 文件 '{}' 缺少 'file_path' 属性.", std::string(mLogTag), tilesetIter->first);
		return engine::component::TileInfo();
	}

	// 图块集分为两种情况, 需要分别考虑
	// 单一图片
	if (tileset.contains("image")) {
		// 获取图片路径
		auto textureId = resolvePath(tileset["image"].get<std::string>(), filePath);
		// 计算瓦片在图片网格中的坐标
		auto coordinateX = localId % tileset["columns"].get<int>();
		auto coordinateY = localId / tileset["columns"].get<int>();
		// 根据坐标确定源矩阵
		SDL_FRect textureRect = {
			static_cast<float>(coordinateX * mTileSize.x),
			static_cast<float>(coordinateY * mTileSize.y),
			static_cast<float>(mTileSize.x),
			static_cast<float>(mTileSize.y)
		};
		engine::render::Sprite sprite{ textureId, textureRect };
		return engine::component::TileInfo(sprite, engine::component::TileType::NORMAL);
	}
	// 多图片
	else {
		if (!tileset.contains("tiles")) {
			spdlog::error("{} : Tileset 文件 '{}' 缺少 'tiles' 属性.", std::string(mLogTag), tilesetIter->first);
			return engine::component::TileInfo();
		}
		// 遍历tiles数组, 根据id查找对应的瓦片
		const auto& tilesJson = tileset["tiles"];
		for (const auto& tileJson : tilesJson) {
			auto tileId = tileJson.value("id", 0);
			if (tileId == localId) {
				if (!tileJson.contains("image")) {
					spdlog::error("{} : Tileset 文件 '{}' 中瓦片缺少 'image' 属性.", tilesetIter->first, tileId);
					return engine::component::TileInfo();
				}

				// 获取图片路径
				auto textureId = resolvePath(tileJson["image"].get<std::string>(), filePath);
				// 确认图片尺寸
				auto imageWidth = tileJson.value("imagewidth", 0);
				auto imageHeight = tileJson.value("imageheight", 0);
				// 从Json中获取源矩阵信息
				SDL_FRect textureRect = {
					static_cast<float>(tileJson.value("x", 0)),
					static_cast<float>(tileJson.value("y", 0)),
					static_cast<float>(tileJson.value("width", imageWidth)),
					static_cast<float>(tileJson.value("height", imageHeight))
				};
				engine::render::Sprite sprite{ textureId, textureRect };
				return engine::component::TileInfo(sprite, engine::component::TileType::NORMAL);
			}
		}
	}

	spdlog::error("{} : 图块集 '{}' 中未找到gid未 {} 的瓦片.", std::string(mLogTag), tilesetIter->first, gid);
	return engine::component::TileInfo();
}

void LevelLoader::loadTileset(const std::string& tilesetPath, int firstGid) {
	std::ifstream tilesetFile(tilesetPath);
	if (!tilesetFile.is_open()) {
		spdlog::error("{} 无法打开 Tileset 文件 : {}", std::string(mLogTag), tilesetPath);
		return;
	}

	nlohmann::json tilesetJson;
	try {
		tilesetFile >> tilesetJson;
	}
	catch (const nlohmann::json::parse_error& e) {
		spdlog::error("{} : 解析 Tileset JSON 文件 '{}' 失败: {} (at byte {})", std::string(mLogTag), tilesetPath, e.what(), e.byte);
		return;
	}
	tilesetJson["file_path"] = tilesetPath;
	mTilesetData[firstGid] = std::move(tilesetJson);
	spdlog::info("{} : Tileset 文件 '{}' 加载完成, firstgid: {}", std::string(mLogTag), tilesetPath, firstGid);
}

std::string LevelLoader::resolvePath(const std::string& relativePath, const std::string& filePath) {
	try {
		// 获取地图文件的父目录（相对于可执行文件） “assets/maps/level1.tmj” -> “assets/maps”
		auto mapDir = std::filesystem::path(filePath).parent_path();
		// 合并路径（相对于可执行文件）并返回。 
		// std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，得到一个干净的路径
		auto finalPath = std::filesystem::canonical(mapDir / relativePath);
		return finalPath.string();
	}
	catch (const std::exception& e) {
		spdlog::error("{} : 路径解析失败: {}", std::string(mLogTag), e.what());
		return relativePath;
	}
}
} // namespace engine::scene
