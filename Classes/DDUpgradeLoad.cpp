// (C) 2015 Arisecbf


#include "DDUpgradeLoad.h"

std::unordered_map<int, std::unordered_map<PropertyType, PropertyUpgradeMap>> DDUpgradeRoadMap::_agentUpgradeMaps;
std::unordered_map<int, int> DDUpgradeRoadMap::_agentActionPeriodMap;


void DDUpgradeRoadMap::build()
{
    _agentUpgradeMaps[DDAgent::AT_FRIEND_WALL] = {};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MINER] = {};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_ARROW_TOWER] = {};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MAGIC_TOWER] = {};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CONNON_TOWER] = {};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CURE_TOWER] = {};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_LIGHT_TOWER] = {};

    // Wall
    _agentUpgradeMaps[DDAgent::AT_FRIEND_WALL][PropertyType::BLOOD_MAX] = {3, -1, 1};

    // Miner
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MINER][PropertyType::BLOOD_MAX] = {3, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MINER][PropertyType::MINE_SPEED] = {1, 10, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MINER][PropertyType::ACTION_DISTANCE] = {1, 3, 1};

    // Arrow Tower
    _agentUpgradeMaps[DDAgent::AT_FRIEND_ARROW_TOWER][PropertyType::BLOOD_MAX] = {3, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_ARROW_TOWER][PropertyType::ATTACK] = {1, 20, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_ARROW_TOWER][PropertyType::ACTION_DISTANCE] = {2, 4, 1};

    // Arrow Tower
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MAGIC_TOWER][PropertyType::BLOOD_MAX] = {3, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MAGIC_TOWER][PropertyType::ATTACK] = {1, 20, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_MAGIC_TOWER][PropertyType::ACTION_DISTANCE] = {2, 4, 1};

    // Cannon Tower
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CONNON_TOWER][PropertyType::BLOOD_MAX] = {3, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CONNON_TOWER][PropertyType::ATTACK] = {5, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CONNON_TOWER][PropertyType::ACTION_DISTANCE] = {2, 5, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CONNON_TOWER][PropertyType::SPUTT_RADIO] = {2, 8, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CONNON_TOWER][PropertyType::SPUTT_DISTANCE] = {1, 3, 1};

    // Cure Tower
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CURE_TOWER][PropertyType::BLOOD_MAX] = {3, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CURE_TOWER][PropertyType::CURE] = {1, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CURE_TOWER][PropertyType::ACTION_DISTANCE] = {1, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_CURE_TOWER][PropertyType::CURE_NUM] = {1, -1, 1};

    // Light Tower
    _agentUpgradeMaps[DDAgent::AT_FRIEND_LIGHT_TOWER][PropertyType::BLOOD_MAX] = {3, -1, 1};
    _agentUpgradeMaps[DDAgent::AT_FRIEND_LIGHT_TOWER][PropertyType::ACTION_DISTANCE] = {4, -1, 1};


    // 机动间隔
    _agentActionPeriodMap[DDAgent::AT_FRIEND_MINER] = 10;
    _agentActionPeriodMap[DDAgent::AT_FRIEND_ARROW_TOWER] = 10;
    _agentActionPeriodMap[DDAgent::AT_FRIEND_MAGIC_TOWER] = 10;
    _agentActionPeriodMap[DDAgent::AT_FRIEND_CONNON_TOWER] = 20;
    _agentActionPeriodMap[DDAgent::AT_FRIEND_CURE_TOWER] = 10;
}


// 产生各种我军的初始状态。
DDAgent* DDUpgradeRoadMap::generateInitFriendAgents(int agentType)
{
    DDAgent* agent = new DDAgent();
    agent->type = agentType;
    switch (agentType) {
        case DDAgent::AT_FRIEND_WALL:
            agent->bloodMax = agent->blood = getPropertyUpgradeMap(agentType, PropertyType::BLOOD_MAX).start;
            break;
        case DDAgent::AT_FRIEND_MINER:
            agent->bloodMax = agent->blood = getPropertyUpgradeMap(agentType, PropertyType::BLOOD_MAX).start;
            agent->mineSpeed = getPropertyUpgradeMap(agentType, PropertyType::MINE_SPEED).start;
            agent->actionDistance = getPropertyUpgradeMap(agentType, PropertyType::ACTION_DISTANCE).start;
            agent->actionPeriod = agent->actionPeriodIndex = _agentActionPeriodMap[agentType];
            break;
        case DDAgent::AT_FRIEND_ARROW_TOWER:
        case DDAgent::AT_FRIEND_MAGIC_TOWER:
            agent->bloodMax = agent->blood = getPropertyUpgradeMap(agentType, PropertyType::BLOOD_MAX).start;
            agent->actionPeriod = agent->actionPeriodIndex = _agentActionPeriodMap[agentType];
            agent->actionDistance = getPropertyUpgradeMap(agentType, PropertyType::ACTION_DISTANCE).start;
            agent->attack = getPropertyUpgradeMap(agentType, PropertyType::ATTACK).start;
            break;
        case DDAgent::AT_FRIEND_CONNON_TOWER:
            agent->bloodMax = agent->blood = getPropertyUpgradeMap(agentType, PropertyType::BLOOD_MAX).start;
            agent->actionPeriod = agent->actionPeriodIndex = _agentActionPeriodMap[agentType];
            agent->actionDistance = getPropertyUpgradeMap(agentType, PropertyType::ACTION_DISTANCE).start;
            agent->attack = getPropertyUpgradeMap(agentType, PropertyType::ATTACK).start;
            agent->cannonSputtDistance = getPropertyUpgradeMap(agentType, PropertyType::SPUTT_DISTANCE).start;
            agent->cannonSputtDamageRadio = getPropertyUpgradeMap(agentType, PropertyType::SPUTT_RADIO).start;
            break;
        case DDAgent::AT_FRIEND_CURE_TOWER:
            agent->bloodMax = agent->blood = getPropertyUpgradeMap(agentType, PropertyType::BLOOD_MAX).start;
            agent->actionPeriod = agent->actionPeriodIndex = _agentActionPeriodMap[agentType];
            agent->actionDistance = getPropertyUpgradeMap(agentType, PropertyType::ACTION_DISTANCE).start;
            agent->cure = getPropertyUpgradeMap(agentType, PropertyType::CURE).start;
            agent->cureNum = getPropertyUpgradeMap(agentType, PropertyType::CURE_NUM).start;
            break;
        case DDAgent::AT_FRIEND_LIGHT_TOWER:
            agent->bloodMax = agent->blood = getPropertyUpgradeMap(agentType, PropertyType::BLOOD_MAX).start;
            agent->actionDistance = getPropertyUpgradeMap(agentType, PropertyType::ACTION_DISTANCE).start;
            break;
        default:
            assert(false);
            break;
    }
    return agent;
}