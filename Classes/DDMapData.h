
#ifndef DDMapData_hpp
#define DDMapData_hpp


#include <unordered_map>
#include "DDAgent.h"
#include "DDConfig.h"
#include "DDProtocals.h"


struct MapPos
{
    int x, y;
    MapPos(int x, int y):x(x),y(y){};
    MapPos(){x = y = 0;};
    float length(){return std::abs(x)+std::abs(y);};//根据地图的特性，并不是sqrt的。
};

bool operator==(const MapPos& r, const MapPos& l);

namespace std{
    template<>
    class hash<MapPos> {
        public :
        size_t operator()(const MapPos &p) const {
            return ((p.y+100) * 1000 + p.x + 100);
        }
    };
}

class DDMinMap
{
public:
    enum State{
        T_ACTIVABLE,//可被激活
        T_ACTIVE,//活动的
        T_NON_ACTIVE //还不可激活
    };
    static std::shared_ptr<DDMinMap> create();
    static std::shared_ptr<DDMinMap> createMinMap(const MapPos& mappos);
    static std::shared_ptr<DDMinMap> createCoreMinMap();
    virtual ~DDMinMap();
    bool blocked; //被阻止的
    MapPos pos;
    int state;
    int distance2core; //距离核心的距离
    DDElementType mainElementType;
    DDElementType secondaryElementType;

    bool isCore() const{return pos.x == 0 && pos.y == 0;}
    bool isPosEmpty(const AgentPos& agentPos) const;
    /*
    std::vector<DDMine*> agentMines;//矿
    std::vector<DDStoneBarrier*> agentStones;//石头
    std::vector<DDTreeBarrier*> agentTrees;//树
    std::vector<DDWaterPoolAgent*> agentWaterPools;//水塘
    std::vector<DDVolcanoAgent*> agentVolcanos;//火山
    std::vector<DDFriendMinerAgent*> agentMiners;//矿厂
    std::vector<DDFriendWallAgent*> agentWalls;//墙
    std::vector<DDFriendLightHouseAgent*> agentLightHouses;//灯塔
    std::vector<DDFriendArrowTowerAgent*> agentArrowTowers;//箭塔
    std::vector<DDFriendCannonTowerAgent*> agentCannonTowers;//炮塔
    std::vector<DDFriendCureTowerAgent*> agentCureTowers;//治疗塔
    DDFriendCoreAgent* agentCore = nullptr;//核心（只有核心才有的建筑）
    std::vector<DDEnemyNestAgent*> agentEnemyNests;//母巢
    std::vector<DDEnemyFarMonsterAgent*> agentFarEnemies;//远程怪
    std::vector<DDEnemyNearMonsterAgent*> agentNearEnemies;//近战怪*/

    // 分别用pos和aid对agents进行索引，这俩个容器的内容是一致的。
    std::unordered_map<AgentPos, DDAgent*> posAgentMap;
    std::unordered_map<int, DDAgent*> aidAgentMap;


    // TreatMap用来为怪物寻路服务，加速！
    std::unordered_map<AgentPos, int> generalTreatMap;
    std::unordered_map<AgentPos, int> wallTreatMap;

protected:
    DDMinMap(){};
};



enum class DDBigmapObserveMessage
{
    MINE_EXAUSTED,//一个矿被挖空
};

// 运行时数据结构
class DDMapData
{
public:
    static DDMapData* s() {return &_instance;};
    std::unordered_map<MapPos, std::shared_ptr<DDMinMap>>& getMinMaps(){return _presentingMinmaps;}
    std::shared_ptr<DDMinMap>& getBattleObservedMinMap() {
        return _presentingMinmaps[battleFieldObservedMinMap];
    }
    static int nextAgentId(){return _agentIdIndex++;};
    int resourceMineAmount;
    int resourceGasAmount;
    
    void store();
    void recover();

    // 选择一个随机的minmap templates
    void randomChooseTemplates() { _templateIndex = 0;}//TODO
    // 从模板中读取每个minmap的模板数据
    void loadMinmapTemplates();
    // 从游戏存档中读取游戏数据
    void loadPresentingMinmapsFromSavedGame();
    // 新开局设定已活动minmaps
    void loadPresentingMinmapsForNew();

    // 新建Agent（包括nest产怪，玩家放置友军），都是从调用新放agent接口开始的，由mapdata发消息给battle field进行后续动作。（即，即使是看似由battle field发起的动作，battle field上新出现的agent也是由mapdata发消息给Battle field后，由battle field处理消息才表现出来的）

    // 新产生敌人，根据nest内配置参数，在minmap/agentpos位置产生一个敌人，同时还有消息发送。
    void appearAgent_enemy(const std::shared_ptr<DDMinMap>& minmap, const AgentPos& agentpos, DDAgent* nest, bool battleObserving);

    // 新产生友军建筑，总是在battleObserved的minmap，总是会observing。
    void appearAgent_friend(const AgentPos& agentpos, int friendAgentType);

    // 卖出友军建筑，或者，移出3RD的石头和树，总是在battleObserved的minmap，总是会observing。
    void removeAgent(const AgentPos& agentpos);

    bool hasSavedGame(){ return _hasSavedGame;}
    int getTemplateIndex(){ return _templateIndex;}


    bool isBattleFieldObserving = false;
    MapPos battleFieldObservedMinMap = {0,0};
    
    bool isBigmapObserving = false;

    void configProtocals(DDBattleFieldProtocal* battleProtocal);

    bool isMinMapActive(const MapPos& mappos);


//    void registerBattleFieldObserver(std::function<void(DDBattleObserveMessage message, int aid_src, int aid_des)> battleFieldObserver);
//    void registerBigmapFieldObserver(std::function<void(DDBigmapObserveMessage message, MapPos mappos)> bigmapFieldObserver);

    void gameTick(); //驱动一个游戏内帧Period。

protected:
    DDBattleFieldProtocal* _battleFieldProtocal;
    int _templateIndex;//模板的id，用来支持多个template地图，暂时开发中，只有默认的一个地图。
    bool _hasSavedGame;
    bool _hasFinishTutorial;
    static int _agentIdIndex; //从10000开始，之前的0-10000留给离线生成的agent
    static DDMapData _instance;

    MapPos _selectedMinMapPos;

    void sendBattleFieldMessage(DDBattleObserveMessage message, int aid_src, int aid_des) { _battleFieldProtocal->op_mapdata_message(message, aid_src, aid_des);}

//    void sendBigmapFieldMessage(DDBigmapObserveMessage message, MapPos mappos) { _bigmapFieldObserver(message, mappos); }

    void tickStepPerMinMap(const std::shared_ptr<DDMinMap>& minmap);

    void tickStepPerMinMap_waterPool(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_volcano(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_miner(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_arrowTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_cannonTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_cureTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_magicTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_core(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);
    void tickStepPerMinMap_enemyNest(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent, bool battleObserving);
    void tickStepPerMinMap_enemy(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent, bool battleObserving);

//    void tickStepPerMinMap_farEnemy(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);

//    void tickStepPerMinMap_nearEnemy(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent);


    void help_computePeriodIndex(DDAgent* activeAgent);
    bool help_isFirstStageAction(DDAgent* activeAgent);
    bool help_isSecondStageAction(DDAgent* activeAgent);
//    bool help_computeSelfCureIndex(DDAgent* activeAgent);
//    void help_computeUnbeatableAmount(DDAgent* activeAgent, bool battleObserving);

    int _unblockedMinmapTotal; // 非blocked的minmap总数(占领通关值）
    std::unordered_map<MapPos, std::shared_ptr<DDMinMap>> _templateMinmaps;

    std::unordered_map<MapPos, std::shared_ptr<DDMinMap>> _presentingMinmaps; // 在表演的minmap们，包括可激活的和活动

    void help_movefromTemplate2preseting(const MapPos& mappos); //将某个从templates移到preseting中，自动设置成待激活(实际并不会从templates中移走)，核心会设成已激活。
    void help_makeArroundActivable(const MapPos& mappos); //将某个周围的变成待激活

    void help_calcMinMapThreatMapAll(const MapPos& mappos); //为minmap计算TreatMap，当初始化、建筑灭失时需要调用此方法进行完全计算。

    void help_calcMinMapThreatMapSingle(const MapPos& mappos, const AgentPos& agentpos); //当新增建筑时，要为threat map补充这个新建筑的量。

    void help_calcMinMapThreatMapSingle_start(std::unordered_map<AgentPos, DDAgent*>* posAgents, std::unordered_map<AgentPos, int>* threatMap, const AgentPos& pos, int threat); //递归的点-降渲染threat map

    void help_calcMinMapThreatMapSingle_ast(std::unordered_map<AgentPos, DDAgent*>* posAgents, std::unordered_map<AgentPos, int>* threatMap, const AgentPos& pos, int threat); //递归的点-降渲染threat map

    void help_clearMinMapThreatMap(std::unordered_map<AgentPos, int>* threatMap); //初始化 或 清0 威胁图
    void help_printMinMapThreatMap(std::unordered_map<AgentPos, int>* threatMap); //打印威胁图

};



#endif /* DDMapData_hpp */
