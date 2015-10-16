// (C) 2015 Arisecbf

#ifndef DDUpgradeLoad_hpp
#define DDUpgradeLoad_hpp

#include "DDConfig.h"
#include "DDAgent.h"

// 怪物，第三方 的属性由 地图数据 离线生成。
// 而我军的属性配置都在这里了。


enum class PropertyType:int
{
    ATTACK = 0,
    CURE = 1,
    CURE_NUM = 2,
    MINE_SPEED = 3,
    ACTION_DISTANCE = 4,
    BLOOD_MAX = 5,
    MINE_AMOUNT = 6,
    BOSS = 7,
    SPUTT_DISTANCE = 8,
    SPUTT_RADIO = 9,
    ELEMENT_NONE = 10,
    ELEMENT_METAL = 11,
    ELEMENT_WOOD = 12,
    ELEMENT_WATER = 13,
    ELEMENT_FIRE = 14,
    ELEMENT_EARTH = 15,
    REMOVE_3RD = 16,
    SELL_FRIEND = 17,
};

namespace std{
        template<>
        class hash<PropertyType> {
            public :
            size_t operator()(const PropertyType &p) const {
                return static_cast<int>(p);
            }
        };
}

// 我军建筑的升级路线图
// friend agents的升级路线图 定义{类型-》可升级属性-》初始等级，最高等级，各等级的升级花费}
struct PropertyUpgradeMap
{
    int start;
    int end; // -1表示无终点
    int cost;
};

class DDUpgradeRoadMap
{
public:
    static std::unordered_map<int, std::unordered_map<PropertyType, PropertyUpgradeMap>> _agentUpgradeMaps;
    static std::unordered_map<int, int> _agentActionPeriodMap;
    static void build();
    //获取某个类型的agent的可以升级的属性列表。
    std::vector<PropertyType> getUpdradableProperties(int agentType)
    {
        std::vector<PropertyType> ret;
        switch (agentType) {
            case DDAgent::AT_FRIEND_WALL:
                ret = {PropertyType::BLOOD_MAX};
                break;
            case DDAgent::AT_FRIEND_MINER:
                ret = {PropertyType::BLOOD_MAX, PropertyType::MINE_SPEED, PropertyType::ACTION_DISTANCE};
                break;
            case DDAgent::AT_FRIEND_ARROW_TOWER:
            case DDAgent::AT_FRIEND_MAGIC_TOWER:
                ret = {PropertyType::BLOOD_MAX, PropertyType::ATTACK, PropertyType::ACTION_DISTANCE};
                break;
            case DDAgent::AT_FRIEND_CONNON_TOWER:
                ret = {PropertyType::BLOOD_MAX, PropertyType::ATTACK, PropertyType::ACTION_DISTANCE, PropertyType::SPUTT_RADIO, PropertyType::SPUTT_DISTANCE};
                break;
            case DDAgent::AT_FRIEND_CURE_TOWER:
                ret = {PropertyType::BLOOD_MAX, PropertyType::CURE, PropertyType::ACTION_DISTANCE, PropertyType::CURE_NUM};
                break;
            case DDAgent::AT_FRIEND_LIGHT_TOWER:
                ret = {PropertyType::BLOOD_MAX, PropertyType::ACTION_DISTANCE};
                break;

            default:
                ret = {};
                break;
        }
        return ret;
    }


    static PropertyUpgradeMap& getPropertyUpgradeMap(int agentType, PropertyType propertyType)
    {

        return _agentUpgradeMaps[agentType][propertyType];
    }

    // 产生各种我军的初始状态。
    static DDAgent* generateInitFriendAgents(int agentType);
};


#endif /* DDAgent_hpp */
