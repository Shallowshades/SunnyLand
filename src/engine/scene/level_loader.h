/*****************************************************************//**
 * @file   level_loader.h
 * @brief  关卡载入器
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.11.05
 *********************************************************************/

#pragma once
#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>

namespace engine::component { struct TileInfo; }

namespace engine::scene {
class Scene;
/**
 * @brief 关卡载入器类.
 */
class LevelLoader final {
public:
	LevelLoader() = default;

	/**
	 * @brief 加载关卡数据到指定的Scene对象中.
	 * 
	 * @param mapPath Tiled JSON地图文件的路径
	 * @param scene 要加载数据的目标 Scene 对象
	 * @return bool 是否加载成功
	 */
	bool loadLevel(const std::string& mapPath, Scene& scene);

private:
	void loadImageLayer(const nlohmann::json& layerJson, Scene& scene);		///< @brief 加载图片图层
	void loadTileLayer(const nlohmann::json& layerJson, Scene& scene);		///< @brief 加载瓦片图层
	void loadObjectLayer(const nlohmann::json& layerJson, Scene& scene);	///< @brief 加载对象图层
	
	/**
	 * @brief 根据全局Id, 获取瓦片信息.
	 * 
	 * @param gid 全局id
	 * @return engine::component::TileInfo 瓦片信息
	 */
	engine::component::TileInfo getTileInfoByGid(int gid);

	/**
	 * @brief 加载Tiled tileset 文件 (.tsj).
	 * 
	 * @param tilesetPath Tileset 文件路径
	 * @param firstGid 此tileset的第一个全局id
	 */
	void loadTileset(const std::string& tilesetPath, int firstGid);

	/**
	 * @brief 解析图片路径, 合并地图路径和相对路径. 例如
	 *	- 1. 文件路径: "assets/maps/level1.tmj"
	 *	- 2. 相对路径: "../textures/Layers/back.png"
	 *  - 3. 最终路径: "assets/textures/Layers/back.png"
	 * 
	 * @param relative 相对路径
	 * @param filePath 文件路径
	 * @return std::string 解析后的完整路径
	 */
	std::string resolvePath(const std::string& relativePath, const std::string& filePath);
private:
	static constexpr std::string_view mLogTag = "LevelLoader";				///< @brief 日志标记		

	std::string mMapPath;													///< @brief 地图路径
	glm::ivec2 mMapSize;													///< @brief 地图尺寸(瓦片数量)
	glm::ivec2 mTileSize;													///< @brief 瓦片尺寸(像素)
	std::map<int, nlohmann::json> mTilesetData;								///< @brief 瓦片集数据
};
} // namespace engine::scene

#endif // !LEVEL_LOADER_H

