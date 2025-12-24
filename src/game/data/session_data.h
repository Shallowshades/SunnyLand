/*****************************************************************//**
 * @file   session_data.h
 * @brief  管理不同游戏场景的游戏状态类
 * @version 1.0
 * 
 * @author Shallowshades
 * @date   2025.12.24
 *********************************************************************/

#pragma once
#ifndef SESSION_DATA_H
#define SESSION_DATA_H

#include <string>
#include <memory>
#include <nlohmann/json.hpp> 

namespace game::data {

/**
* @brief 管理不同游戏场景之间的游戏状态
*
* 存储玩家生命值、分数、当前关卡等信息，
* 使这些数据在场景切换时能够保持。
*/
class SessionData final {
public:
    SessionData() = default;
    ~SessionData() = default;

    // 删除复制和移动操作以防止意外复制
    SessionData(const SessionData&) = delete;
    SessionData& operator=(const SessionData&) = delete;
    SessionData(SessionData&&) = delete;
    SessionData& operator=(SessionData&&) = delete;

    // Getters & Setters
    int getCurrentHealth() const;
    int getMaxHealth() const;
    int getCurrentScore() const;
    int getHighScore() const;
    int getLevelHealth() const;
    int getLevelScore() const;
    const std::string& getMapPath() const;
    void setCurrentHealth(int health);
    void setMaxHealth(int maxHealth);
    void addScore(int scoreToAdd);
    void setHighScore(int highScore);
    void setLevelHealth(int levelHealth);
    void setLevelScore(int levelScore);
    void setMapPath(const std::string& mapPath);

    void reset();                                                           ///< @brief 重置游戏数据以准备开始新游戏（保留最高分）
    void setNextLevel(const std::string& map_path);                         ///< @brief 设置下一个场景信息（地图、关卡开始时的得分生命）
    bool saveToFile(const std::string& filename) const;                     ///< @brief 将当前游戏数据保存到JSON文件（存档）
    bool loadFromFile(const std::string& filename);                         ///< @brief 从JSON文件中读取游戏数据（读档）
private:
	int mCurrentHealth = 3;                                                 ///< @brief 当前生命值
    int mMaxHealth = 3;                                                     ///< @brief 最大生命值
    int mCurrentScore = 0;                                                  ///< @brief 当前分数
    int mHighScore = 0;                                                     ///< @brief 最高分数

	int mLevelHealth = 3;                                                   ///< @brief 进入关卡时的生命值（读/存档用）
	int mLevelScore = 0;                                                    ///< @brief 进入关卡时的得分（读/存档用）
	std::string mMapPath = "assets/maps/level1.tmj";                        ///< @brief 关卡路径
};

} // namespace game::state

#endif // SESSION_DATA_H
