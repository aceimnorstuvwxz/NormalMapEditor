
#include "DDMapData.h"
#include <memory>
#include <cmath>
#include "DDConfig.h"

USING_NS_CC;


DDMinMap::~DDMinMap()
{

    for (auto ag : posAgentMap) {
        delete ag.second;
    }
}

// 各元素在不同属性下出现的概率
const float occurcyRadioStone[5] = {0.5,0.5,0.5,0.5,0.8};//石头
const float occurcyRadioTree[5] = {0.2,0.9,0.7,0.3,0.6};//树
const float occurcyRadioWater[5] = {0.2,0.5,0.8,0.2,0.5};//水潭
const float occurcyRadioVolcano[5] = {0.6,0.2,0.2,0.8,0.3};//火山
const float occurcyRadioMine[5] = {0.9,0.5,0.5,0.5,0.6};//矿

// 各元素在出现时出现的个数
const std::pair<int, int> numScopeStone = {2,15};
const std::pair<int, int> numScopeTree = {2,15};
const std::pair<int, int> numScopeWater = {1,15};
const std::pair<int, int> numScopeVolcano = {1,2};
const std::pair<int, int> numScopeMine = {2,10};
const std::pair<int, int> numScopeNest = {0,4};

// 各元素的连续性
const float continuesStone = 0.5;
const float continuesTree = 0.7;
const float continuesWater = 0.5;
const float continuesVolcano = 0;
const float continuesMine = 0.5;

//一些配置项
constexpr static int WATER_ACTION_PERIOD = 10;
constexpr static int VOLCONO_ACTION_PERIOD = 10;

constexpr static float WATER_CURE_LENGTH_RADIO = 1.f/5;
constexpr static float VOLCONO_ATTACK_LENGTH_RADIO = 1.f/5;
constexpr static int VOLCONO_ATTACK_DISTANCE = 5;

const int MINE_BASE_CAPACITY = 30;
const int MINE_CAPACITY_LENGTH_RADIO = 5;
const std::pair<float, float> mineCapacityRadio = {0.5f, 2.f};

const std::pair<int, int> periodScopeNestAction = {10,50};
const std::pair<float, float> nestChanceToRelaxScope = {0.01,0.1};
const std::pair<int, int> nestRelaxPeriodScope = {50,1000};

const float nestChanceToHasBoss = 0.5f;
const std::pair<float, float> nestBossRadioScope = {0.05,0.25};

const float nestChanceToBeNear = 0.5f;
const std::pair<float, float> nestMostNearAsNearScope = {0.8,1};
const std::pair<float, float> nestMostFarAsNearScope = {0, 0.2};

const int NEST_BLOOD_BASE = 1;
const int NEST_ATTACK_BASE = 1;
const float NEST_BLOOD_LENGTH_RADIO = 1.f/2;
const std::pair<float, float> nestBloodRadioScope = {1.0, 1.5};
const float NEST_ATTACK_LENGTH_RADIO = 1.f/2;
const std::pair<float, float> nestAttackRadioScope = {1.0, 1.5};
const float NEST_CHANCE_AS_MAIN_ELEMENT_TYPE = 0.75f;

const int NEST_ATTACK_DISTANCE_BASE = 2;
const float NEST_ATTACK_DISTANCE_LENGTH_RADIO = 1.f/5;
const std::pair<float, float> nestAttackDistanceRadioScope = {1.0,1.5};

const int DEFAULT_ENEMY_ACTION_PERIOD = 10;
const std::pair<float, float> defaultEnemyActionPeriodScope = {1.0,1.5};
const int DEFAULT_FRIEND_ACTION_PERIOD = 10;


inline bool calcAgentContinues(float radio)
{
    return rand_0_1() < radio;
}

inline bool calcElementAgentOccurcy(const float* occradio, DDElementType elementType)
{
    float radio = occradio[static_cast<int>(elementType)];
    return rand_0_1() < radio;
}

inline int calcRandomScope(const std::pair<int, int>& minmax)
{
    return  minmax.first + (minmax.second - minmax.first) * rand_0_1();
}

inline float calcRandomScope(const std::pair<float, float>& minmax)
{
    return  minmax.first + (minmax.second - minmax.first) * rand_0_1();
}

bool DDMinMap::isPosEmpty(const AgentPos& agentPos) const
{
    return posAgentMap.count(agentPos) == 0;
}

AgentPos findRandomEmptyAgentPos(DDMinMap* minmap)
{
    while (true) {
        AgentPos pos = {static_cast<int>(rand_0_1()*1000)%DDConfig::BATTLE_NUM - DDConfig::BATTLE_NUM/2, static_cast<int>(rand_0_1()*1000)%DDConfig::BATTLE_NUM - DDConfig::BATTLE_NUM/2};
        if (minmap->isPosEmpty(pos)) {
            return pos;
        }
    }
}

template <class T>
AgentPos findContinuesAgentPos(T agents, DDMinMap* minmap)
{
    std::vector<AgentPos> emptyContinuesPoses;
    auto func = [&emptyContinuesPoses, &minmap](AgentPos basePos,int xdiff, int ydiff){
        AgentPos newpos = AgentPos{basePos.x+xdiff, basePos.y+ydiff};
        if (newpos.x >= -DDConfig::BATTLE_NUM/2 && newpos.x <= DDConfig::BATTLE_NUM/2 &&
            newpos.y >= -DDConfig::BATTLE_NUM/2 && newpos.y <= DDConfig::BATTLE_NUM/2 &&
            minmap->isPosEmpty(newpos)){
            emptyContinuesPoses.push_back(newpos);
        }
    };
    for (auto agent : agents) {
        AgentPos basePos =  agent->pos;
        func(basePos, -1, 0);
        func(basePos, 1, 0);
        func(basePos, 0, 1);
        func(basePos, 0, -1);

        func(basePos, 1, 1);
        func(basePos, 1, -1);
        func(basePos, -1, 1);
        func(basePos, -1, -1);
    }

    if (emptyContinuesPoses.empty()) {
        return findRandomEmptyAgentPos(minmap);
    } else {
        int randomIndex = static_cast<int>(emptyContinuesPoses.size()*rand_0_1())%emptyContinuesPoses.size();
        return emptyContinuesPoses[randomIndex];
    }

}
/*
void putAgentInStone(DDMinMap* minmap)
{
    bool continues = calcAgentContinues(continuesStone);
    AgentPos agentpos;
    if (continues) {
        agentpos = findContinuesAgentPos<std::vector<DDStoneBarrier*>>(minmap->agentStones, minmap);
    } else {
        agentpos = findRandomEmptyAgentPos(minmap);
    }

    auto pagent = new DDStoneBarrier();
    pagent->agentType = DDAgent::AT_3RD_STONE;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;
    pagent->barrierType = DDBarrier::T_STONE;

    minmap->agentStones.push_back(pagent);
    minmap->agentMap[agentpos] = pagent;
}

void putAgentInTree(DDMinMap* minmap)
{
    bool continues = calcAgentContinues(continuesTree);
    AgentPos agentpos;
    if (continues) {
        agentpos = findContinuesAgentPos<std::vector<DDTreeBarrier*>>(minmap->agentTrees, minmap);
    } else {
        agentpos = findRandomEmptyAgentPos(minmap);
    }

    auto pagent = new DDTreeBarrier();
    pagent->agentType = DDAgent::AT_3RD_TREE;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;
    pagent->barrierType = DDBarrier::T_TREE;

    minmap->agentTrees.push_back(pagent);
    minmap->agentMap[agentpos] = pagent;
}

void putAgentInWater(DDMinMap* minmap)
{
    bool continues = calcAgentContinues(continuesWater);
    AgentPos agentpos;
    if (continues) {
        agentpos = findContinuesAgentPos<std::vector<DDWaterPoolAgent*>>(minmap->agentWaterPools, minmap);
    } else {
        agentpos = findRandomEmptyAgentPos(minmap);
    }

    auto pagent = new DDWaterPoolAgent();
    pagent->agentType = DDAgent::AT_3RD_WATER;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;
    pagent->blood = 1;
    pagent->attack = 0;
    pagent->cure = 1;
    pagent->actionDistance = 1;
    pagent->actionPeriod = WATER_ACTION_PERIOD;
    pagent->actionPeriodIndex = pagent->actionPeriod;

    minmap->agentWaterPools.push_back(pagent);
    minmap->agentMap[agentpos] = pagent;
}

void putAgentInVolcano(DDMinMap* minmap)
{
    bool continues = calcAgentContinues(continuesVolcano);
    AgentPos agentpos;
    if (continues) {
        agentpos = findContinuesAgentPos<std::vector<DDVolcanoAgent*>>(minmap->agentVolcanos, minmap);
    } else {
        agentpos = findRandomEmptyAgentPos(minmap);
    }

    auto pagent = new DDVolcanoAgent();
    pagent->agentType = DDAgent::AT_3RD_VOLCANO;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;
    pagent->blood = 1;
    pagent->attack = 1 + minmap->pos.length() * VOLCONO_ATTACK_LENGTH_RADIO;
    pagent->cure = 0;
    pagent->actionDistance = VOLCONO_ATTACK_DISTANCE;
    pagent->actionPeriod = VOLCONO_ACTION_PERIOD;
    pagent->actionPeriodIndex = pagent->actionPeriod;

    minmap->agentVolcanos.push_back(pagent);
    minmap->agentMap[agentpos] = pagent;
}

void putAgentInMine(DDMinMap* minmap)
{
    bool continues = calcAgentContinues(continuesMine);
    AgentPos agentpos;
    if (continues) {
        agentpos = findContinuesAgentPos<std::vector<DDMine*>>(minmap->agentMines, minmap);
    } else {
        agentpos = findRandomEmptyAgentPos(minmap);
    }

    auto pagent = new DDMine();
    pagent->agentType = DDAgent::AT_3RD_MINE;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;

    int length = minmap->pos.length();
    pagent->capacity = (MINE_BASE_CAPACITY + MINE_CAPACITY_LENGTH_RADIO * length) * calcRandomScope(mineCapacityRadio);
    pagent->amount = pagent->capacity;

    minmap->agentMines.push_back(pagent);
    minmap->agentMap[agentpos] = pagent;
}

void putAgentInNest(DDMinMap* minmap)
{
    AgentPos agentpos = findRandomEmptyAgentPos(minmap);

    auto pagent = new DDEnemyNestAgent();
    pagent->agentType = DDAgent::AT_ENEMY_NEST;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;

    pagent->actionPeriod = calcRandomScope(periodScopeNestAction);
    pagent->actionIndex = pagent->actionPeriod;
    pagent->chanceToRelax = calcRandomScope(nestChanceToRelaxScope);
    pagent->actionRelaxPeriod = calcRandomScope(nestRelaxPeriodScope);
    pagent->actionRelaxIndex = 0;

    pagent->chanceToNear = rand_0_1() < nestChanceToBeNear ? calcRandomScope(nestMostNearAsNearScope):calcRandomScope(nestMostFarAsNearScope);
    pagent->chanceToBoss = rand_0_1() < nestChanceToHasBoss ? calcRandomScope(nestBossRadioScope) : 0.f;

    int length = minmap->pos.length();
    pagent->blood = (NEST_BLOOD_BASE + length * NEST_BLOOD_LENGTH_RADIO) * calcRandomScope(nestBloodRadioScope);
    pagent->attack = (NEST_ATTACK_BASE + length * NEST_ATTACK_LENGTH_RADIO) * calcRandomScope(nestAttackRadioScope);

    pagent->elementType = rand_0_1() < NEST_CHANCE_AS_MAIN_ELEMENT_TYPE ? minmap->mainElementType : minmap->secondaryElementType;

    pagent->attackDistanceNear = 1;
    pagent->attackDistanceFar = (NEST_ATTACK_DISTANCE_BASE + NEST_ATTACK_DISTANCE_LENGTH_RADIO * length) * calcRandomScope(nestAttackDistanceRadioScope);
    pagent->attackPeriod = DEFAULT_ENEMY_ACTION_PERIOD * calcRandomScope(defaultEnemyActionPeriodScope);

    minmap->agentEnemyNests.push_back(pagent);
    minmap->agentMap[agentpos] = pagent;
}




std::shared_ptr<DDMinMap> DDMinMap::createMinMap(const MapPos &mappos)
{
    auto map = new DDMinMap();

    map->pos = mappos;
    map->state = T_NON_ACTIVE;

    //根据距离(到core的距离)产生小地图，小地图的参数与距离相关
    //是否需要一些模板？还是完全随机？

    //先产生主次元素类型
    map->mainElementType = genRandomElementType();
    map->secondaryElementType = genRandomElementType();

    //石头
    if (calcElementAgentOccurcy(occurcyRadioStone, map->mainElementType))
    {
        int num = calcRandomScope(numScopeStone);
        for (int i = 0; i < num; i++) {
            putAgentInStone(map);
        }
    }


    //树木
    if (calcElementAgentOccurcy(occurcyRadioTree, map->mainElementType))
    {
        int num = calcRandomScope(numScopeTree);
        for (int i = 0; i < num; i++) {
            putAgentInTree(map);
        }
    }

    //水潭
    if (calcElementAgentOccurcy(occurcyRadioWater, map->mainElementType))
    {
        int num = calcRandomScope(numScopeWater);
        for (int i = 0; i < num; i++) {
            putAgentInWater(map);
        }
    }

    //火山
    if (calcElementAgentOccurcy(occurcyRadioVolcano, map->mainElementType))
    {
        int num = calcRandomScope(numScopeVolcano);
        for (int i = 0; i < num; i++) {
            putAgentInVolcano(map);
        }
    }

    //矿
    if (calcElementAgentOccurcy(occurcyRadioMine, map->mainElementType))
    {
        int num = calcRandomScope(numScopeMine);
        for (int i = 0; i < num; i++) {
            putAgentInMine(map);
        }
    }

    //母巢
    {
        int num = calcRandomScope(numScopeNest);
        for (int i = 0; i < num; i++) {
            putAgentInNest(map);
        }
    }

    return std::shared_ptr<DDMinMap>(map);
}

void putAgentInCore(DDMinMap* minmap)
{

    AgentPos agentpos = {0,0};
    auto pagent = new DDFriendCoreAgent();
    pagent->agentType = DDAgent::AT_FRIEND_CORE;
    pagent->agentLevel = 0;
    pagent->aid = DDMapData::nextAgentId();
    pagent->pos = agentpos;
    pagent->blood = 10;
    pagent->attack = 10;
    pagent->cure = 10;
    pagent->actionDistance = 3;
    pagent->actionPeriod = DEFAULT_FRIEND_ACTION_PERIOD;
    pagent->actionPeriodIndex = pagent->actionPeriod;

    minmap->agentCore = pagent;
    minmap->agentMap[agentpos] = pagent;
}*/

std::shared_ptr<DDMinMap> DDMinMap::create()
{
    auto map = new DDMinMap();
    return  std::shared_ptr<DDMinMap>(map);
}
