
#include "DDMapData.h"
#include <memory>
#include <cmath>
#include "DDConfig.h"

USING_NS_CC;

int DDMapData::_agentIdIndex;
DDMapData DDMapData::_instance;
bool operator==(const MapPos& r, const MapPos& l)
{ return r.x == l.x && r.y == l.y;}

void DDMapData::store()
{
    CCLOG("mapdata store");
    UserDefault::getInstance()->setBoolForKey("has_saved_game", _hasSavedGame);
    UserDefault::getInstance()->setBoolForKey("has_finish_tutorial", _hasFinishTutorial);
    UserDefault::getInstance()->setIntegerForKey("template_index", _templateIndex);
    UserDefault::getInstance()->setIntegerForKey("agent_id_index", _agentIdIndex);
}

void DDMapData::recover()
{
    CCLOG("mapdata recover");
    _hasSavedGame = UserDefault::getInstance()->getBoolForKey("has_saved_game");//默认false
    _hasFinishTutorial = UserDefault::getInstance()->getBoolForKey("has_finish_tutorial");//默认false
    _templateIndex = UserDefault::getInstance()->getIntegerForKey("template_index", 0);
    _agentIdIndex = UserDefault::getInstance()->getIntegerForKey("agent_id_index", 10000);

    // 地图数据的恢复并不在这里。

}

//void DDMapData::registerBattleFieldObserver(std::function<void(DDBattleObserveMessage message, int aid_src, int aid_des)> battleFieldObserver)
//{
//    _battleFieldObserver = battleFieldObserver;
//}
//
//void DDMapData::registerBigmapFieldObserver(std::function<void(DDBigmapObserveMessage message, MapPos mappos)> bigmapFieldObserver)
//{
//    _bigmapFieldObserver = bigmapFieldObserver;
//}

void DDMapData::configProtocals(DDBattleFieldProtocal* battleProtocal)
{
    _battleFieldProtocal = battleProtocal;
}

bool DDMapData::isMinMapActive(const MapPos& mappos)
{
    return _presentingMinmaps.count(mappos) > 0 &&
        _presentingMinmaps[mappos]->state == DDMinMap::T_ACTIVE;
}

constexpr static const int THREAT_FULL_MAP = 20;

inline int fetchThreatMaxOfAgentType(int agentType)
{
    int res = 0;
    switch (agentType) {
        case DDAgent::AT_FRIEND_CORE:
            res = THREAT_FULL_MAP;
            break;

        case DDAgent::AT_FRIEND_CURE_TOWER:
            res = THREAT_FULL_MAP;
            break;

        case DDAgent::AT_FRIEND_LIGHT_TOWER:
            res = THREAT_FULL_MAP;
            break;

        case DDAgent::AT_FRIEND_ARROW_TOWER:
        case DDAgent::AT_FRIEND_CONNON_TOWER:
        case DDAgent::AT_FRIEND_MAGIC_TOWER:
            res = THREAT_FULL_MAP;
            break;
        case DDAgent::AT_FRIEND_MINER:
            res = THREAT_FULL_MAP;
            break;
        case DDAgent::AT_FRIEND_WALL:
            res = -1;

        default:
            break;
    }
    return res;
}

void DDMapData::help_calcMinMapThreatMapAll(const MapPos& mappos) //为minmap计算TreatMap，当初始化、建筑灭失时需要调用此方法进行完全计算。
{
    auto minmap = _presentingMinmaps[mappos];
    help_clearMinMapThreatMap(&(minmap->generalTreatMap));
    help_clearMinMapThreatMap(&(minmap->wallTreatMap));

    for (auto posagent: minmap->posAgentMap) {
        int threat = fetchThreatMaxOfAgentType(posagent.second->type);
        if (threat > 0) {
            help_calcMinMapThreatMapSingle_start(&(minmap->posAgentMap), &(minmap->generalTreatMap), posagent.first, threat);
        } else if (threat == -1) {
            help_calcMinMapThreatMapSingle_start(&(minmap->posAgentMap), &(minmap->wallTreatMap), posagent.first, THREAT_FULL_MAP);
        }
    }
    help_printMinMapThreatMap(&(minmap->generalTreatMap));
    help_printMinMapThreatMap(&(minmap->wallTreatMap));
}

void DDMapData::help_calcMinMapThreatMapSingle(const MapPos& mappos, const AgentPos& agentpos) //当新增建筑时，要为threat map补充这个新建筑的量。
{
    auto minmap = _presentingMinmaps[mappos];
    int threat = fetchThreatMaxOfAgentType(minmap->posAgentMap[agentpos]->type);
    if (threat > 0) {
        help_calcMinMapThreatMapSingle_start(&(minmap->posAgentMap), &(minmap->generalTreatMap), agentpos, threat);
    } else if (threat == -1) {
        help_calcMinMapThreatMapSingle_start(&(minmap->posAgentMap), &(minmap->wallTreatMap), agentpos, THREAT_FULL_MAP);
    }
}

inline bool checkIfAgentCutThreat(DDAgent* agent)
{
    return  false;
}


void DDMapData::help_calcMinMapThreatMapSingle_start(std::unordered_map<AgentPos, DDAgent*>* posAgents, std::unordered_map<AgentPos, int>* threatMap, const AgentPos& pos, int threat)
{
    help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, pos, threat);
    if (threat > 1) {
        int nextThreat = threat -1;
        help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x-1, pos.y}, nextThreat);
        help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x+1, pos.y}, nextThreat);
        help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x, pos.y-1}, nextThreat);
        help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x, pos.y+1}, nextThreat);
    }
}


void DDMapData::help_calcMinMapThreatMapSingle_ast(std::unordered_map<AgentPos, DDAgent*>* posAgents, std::unordered_map<AgentPos, int>* threatMap, const AgentPos& pos, int threat)
{
    if (posAgents->count(pos) > 0 && checkIfAgentCutThreat((*posAgents)[pos])) {
        (*threatMap)[pos] = 0;
        return;
    }
    if (std::abs(pos.x) <= DDConfig::MINMAP_EXPAND && std::abs(pos.y) <= DDConfig::MINMAP_EXPAND) {
        if ((*threatMap)[pos] < threat) {
            (*threatMap)[pos] = threat;
            if (threat > 1) {
                int nextThreat = threat -1;
                help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x-1, pos.y}, nextThreat);
                help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x+1, pos.y}, nextThreat);
                help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x, pos.y-1}, nextThreat);
                help_calcMinMapThreatMapSingle_ast(posAgents, threatMap, {pos.x, pos.y+1}, nextThreat);
            }
        }
    }
}

void DDMapData::help_clearMinMapThreatMap(std::unordered_map<AgentPos, int>* threatMap) //初始化 或 清0 威胁图
{
    for (int x = -DDConfig::MINMAP_EXPAND; x <= DDConfig::MINMAP_EXPAND; x++) {
        for (int y = -DDConfig::MINMAP_EXPAND; y <= DDConfig::MINMAP_EXPAND; y++) {
            (*threatMap)[{x,y}] = 0;
        }
    }
}

void DDMapData::help_printMinMapThreatMap(std::unordered_map<AgentPos, int>* threatMap) //初始化 或 清0 威胁图
{
    for (int y = -DDConfig::MINMAP_EXPAND; y <= DDConfig::MINMAP_EXPAND; y++) {
        std::stringstream ss;
        for (int x = -DDConfig::MINMAP_EXPAND; x <= DDConfig::MINMAP_EXPAND; x++) {
            ss << (*threatMap)[{x,y}] << " ";
        }
        CCLOG("%s", ss.str().c_str());
    }
}

