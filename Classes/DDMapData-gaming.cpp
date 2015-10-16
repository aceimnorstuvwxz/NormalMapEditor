
#include "DDMapData.h"
#include <memory>
#include <cmath>
#include "DDConfig.h"
#include <algorithm>
#include "DDUpgradeLoad.h"

USING_NS_CC;

// 专门用来处理计算gameTick()

void DDMapData::gameTick()
{
//    CCLOG("tick step");
    for (auto posmin : DDMapData::s()->getMinMaps()) {
        auto minmap = posmin.second;
        if (minmap->state == DDMinMap::T_ACTIVE && !minmap->blocked)
        {
            tickStepPerMinMap(minmap);
        }
    }
}

void DDMapData::tickStepPerMinMap(const std::shared_ptr<DDMinMap>& minmap)
{
    // 如果直接遍历，那么过程中对posAgentMap无法进行增删，需要引入暂存和事后导入，但这又给isPosEmpty等判断碎片化。导致太琐碎不可靠。
    // 所以这里采用先获取key，然后用keys来去遍历，但要注意某个key可能已经被删除，所以要做好检查。
    std::vector<int> aids;
    aids.reserve(minmap->posAgentMap.size());
    for (auto posagent: minmap->aidAgentMap) {
        aids.push_back(posagent.first);
    }

    for (auto aid : aids) {
        if (minmap->aidAgentMap.count(aid)) {
            auto agent = minmap->aidAgentMap[aid];
            bool battleObserving = isBattleFieldObserving && minmap->pos == battleFieldObservedMinMap;
            switch (agent->type) {
                case DDAgent::AT_3RD_WATER:
                    tickStepPerMinMap_waterPool(minmap, agent);
                    break;
                case DDAgent::AT_3RD_VOLCANO:
                    tickStepPerMinMap_volcano(minmap, agent);
                    break;
                case DDAgent::AT_FRIEND_MINER:
                    tickStepPerMinMap_miner(minmap, agent);
                    break;
                case DDAgent::AT_FRIEND_ARROW_TOWER:
                    tickStepPerMinMap_arrowTower(minmap, agent);
                    break;
                case DDAgent::AT_FRIEND_CONNON_TOWER:
                    tickStepPerMinMap_cannonTower(minmap, agent);
                    break;
                case DDAgent::AT_FRIEND_CURE_TOWER:
                    tickStepPerMinMap_cureTower(minmap, agent);
                    break;
                case DDAgent::AT_FRIEND_MAGIC_TOWER:
                    tickStepPerMinMap_magicTower(minmap, agent);
                    break;
                case DDAgent::AT_FRIEND_CORE:
                    tickStepPerMinMap_core(minmap, agent);
                    break;
                case DDAgent::AT_ENEMY_NEST:
                    tickStepPerMinMap_enemyNest(minmap, agent, battleObserving);
                    break;
                case DDAgent::AT_ENEMY_NEAR:
                    tickStepPerMinMap_enemy(minmap, agent, battleObserving);
                    break;
                case DDAgent::AT_ENEMY_FAR:
                    tickStepPerMinMap_enemy(minmap, agent, battleObserving);
                    break;
                    
                default:
                    break;
            }
        }
    }
}



void DDMapData::help_computePeriodIndex(DDAgent* agent)
{
    // 对于母巢有relax计算
    if (agent->type == DDAgent::AT_ENEMY_NEST && agent->nestActionRelaxIndex > 0) {
        agent->nestActionRelaxIndex--;
    } else {
        agent->actionPeriodIndex--;
        if (agent->actionPeriodIndex < 0) {
            agent->actionPeriodIndex = agent->actionPeriod;
        }
    }
}

/*
void DDMapData::help_computeUnbeatableAmount(DDAgent* activeAgent, bool battleObserving)
{
    if (activeAgent->unbeatableAmount > 0) {
        activeAgent->unbeatableAmount--;
        if (activeAgent->unbeatableAmount <= 0) {
            if (battleObserving) sendBattleFieldMessage(DDBattleObserveMessage::UNBETABLE_LOSS, activeAgent->aid, -1);
        }
    }
}*/

bool DDMapData::help_isFirstStageAction(DDAgent* activeAgent)
{
    // 1的时候机能
    return activeAgent->actionPeriodIndex == 1;
}

bool DDMapData::help_isSecondStageAction(DDAgent* activeAgent)
{
    // 0的时候自疗
    return activeAgent->actionPeriodIndex == 0;
}
/*
bool DDMapData::help_computeSelfCureIndex(DDAgent* activeAgent)
{
    if (activeAgent->selfCureAmount > 0 || activeAgent->selfCureAmount < 0) {
        activeAgent->selfCureAmount--;
        activeAgent->blood = std::min(activeAgent->blood + activeAgent->selfCure, activeAgent->    bloodMax);
        return true;
    }
    return false;
}*/
/*
template <class T>
std::vector<T*> help_findAllInCertainScope(const std::vector<T*>& agents, const AgentPos& mypos, int scope) {
    std::vector<T*> res;
    for (T* agent : agents) {
        if (agent->pos.distance(mypos) <= scope) {
            res.push_back(agent);
        }
    }
    return res;
}

template <class T>
std::vector<T*> help_findAllAtCertainDistance(const std::vector<T*>& agents, const AgentPos& mypos, int distance) {
    std::vector<T*> res;
    for (T* agent : agents) {
        if (agent->pos.distance(mypos) == distance) {
            res.push_back(agent);
        }
    }
    return res;
}
*/
std::vector<AgentPos> help_findAllEmptyAtCertainDistance(const std::shared_ptr<DDMinMap>& minmap, const AgentPos& mypos, int distance)
{
    std::vector<AgentPos> ret;
    for (int dx = -distance; dx <= distance; dx++) {
        int dy = distance - std::abs(dx);
        if (dy != 0) {
            AgentPos a = {mypos.x + dx, mypos.y + dy};
            AgentPos b = {mypos.x + dx, mypos.y - dy};
            if (a.isLegal() && minmap->posAgentMap.count(a) == 0)
                ret.push_back(a);
            if (b.isLegal() && minmap->posAgentMap.count(b) == 0)
                ret.push_back(b);
        } else {
            AgentPos a = {mypos.x + dx, mypos.y};
            if (a.isLegal() && minmap->posAgentMap.count(a) == 0)
                ret.push_back(a);
        }
    }
    return ret;
}

std::pair<bool, AgentPos> help_findNearestRandomEmptyInScope(const std::shared_ptr<DDMinMap>& minmap, const AgentPos& mypos, int myscope)
{
    bool isfound = false;
    AgentPos res;
    for (int d = 1; d <= myscope; d++) {
        auto founds = help_findAllEmptyAtCertainDistance(minmap, mypos, d);
        if (!founds.empty()) {
            isfound = true;
            res = founds[static_cast<int>(rand_0_1()*100)%founds.size()];
            break;
        }
    }
    return {isfound, res};
}
/*
template <class T>
void help_deleteAgentFromMinmap(const std::vector<T*>& agents, std::unordered_map<AgentPos, DDAgent*>& agentMap, AgentPos agentpos)
{
    for (auto iter = agents.begin(); iter != agents.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = agents.erase(iter);
            break;
        }
    }
    delete agentMap[agentpos];
    agentMap.erase(agentpos);
}

void help_deleteAgentFromMinmap(std::shared_ptr<DDMinMap> minmap, AgentPos agentpos)
{
    for (auto iter = minmap->agentMines.begin(); iter != minmap->agentMines.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentMines.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentWaterPools.begin(); iter != minmap->agentWaterPools.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentWaterPools.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentMiners.begin(); iter != minmap->agentMiners.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentMiners.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentWalls.begin(); iter != minmap->agentWalls.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentWalls.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentLightHouses.begin(); iter != minmap->agentLightHouses.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentLightHouses.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentArrowTowers.begin(); iter != minmap->agentArrowTowers.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentArrowTowers.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentCannonTowers.begin(); iter != minmap->agentCannonTowers.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentCannonTowers.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentCureTowers.begin(); iter != minmap->agentCureTowers.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentCureTowers.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentFarEnemies.begin(); iter != minmap->agentFarEnemies.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentFarEnemies.erase(iter);
            break;
        }
    }
    for (auto iter = minmap->agentNearEnemies.begin(); iter != minmap->agentNearEnemies.end(); iter++) {
        if ((*iter)->pos == agentpos) {
            iter = minmap->agentNearEnemies.erase(iter);
            break;
        }
    }
    delete minmap->agentMap[agentpos];
    minmap->agentMap.erase(agentpos);
}*/

void DDMapData::tickStepPerMinMap_waterPool(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_volcano(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_miner(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_arrowTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_cannonTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_cureTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_magicTower(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_core(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}


void DDMapData::tickStepPerMinMap_enemyNest(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent, bool battleObserving)
{
    // 母巢会在周边位置生成敌人，敌人的初始位置在母巢的周边2及内。
    help_computePeriodIndex(agent);
//    help_computeUnbeatableAmount(agent, battleObserving);
    if (help_isFirstStageAction(agent)) {
        // 机能
        auto ifpos = help_findNearestRandomEmptyInScope(minmap, agent->pos, 2);
        if (ifpos.first) {
            AgentPos agentpos = ifpos.second;
            CCLOG("nest create enemy at >%d %d", agentpos.x, agentpos.y);

            // 制作敌人
            this->appearAgent_enemy(minmap, agentpos, agent, battleObserving);

        } else {
//            CCLOG("nest, no empty place for new enemy");
        }
        // 可能会进入睡眠
        if (rand_0_1() < agent->nestChanceToRelax) {
            CCLOG("nest in relax");
            agent->nestActionRelaxIndex = agent->nestActionRelaxPeriod;
        }
    }
    /*else if (help_isSecondStageAction(agent)) {
        // 自我治疗
        if (help_computeSelfCureIndex(miner)) {
            if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::SELF_CURE, miner->aid, -1);
        }
    }
    */

}


void DDMapData::tickStepPerMinMap_enemy(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent, bool battleObserving)
{
    help_computePeriodIndex(agent);
//    help_computeUnbeatableAmount(agent, battleObserving);
    if (help_isFirstStageAction(agent)) {
        // 机能
        int threat = minmap->generalTreatMap[agent->pos];
        // 由于移出口肯定会有FULL_MAP的威胁，所以threat为0肯定是这个位置被四周被堵死了。
        // 堵死分为两种情况，1-天然堵死 2-由于人造的墙堵死
        bool willAttackWall = threat == 0; //当堵死时，会攻击墙！

        //TODO 在传送点，并且willAttackWall == true，则移出到上游！

        // 先尝试进攻
        if (agent->hasLockedTarget && minmap->posAgentMap.count(agent->lockedTargetPos) && (willAttackWall == (minmap->posAgentMap[agent->lockedTargetPos]->type == DDAgent::AT_FRIEND_WALL))) {
            // 如果已有锁定攻击目标(如果在不是需要对墙攻击的情况下，攻击的是墙，那么需要重新选择攻击目标)，并且锁定目标依然存在，那么直接攻击！
            // TODO 进攻 已锁定的！！
        } else {
            if (willAttackWall) {
                // 找一个墙来进攻，找到则锁定，并进攻！找不到则按墙的威胁图走一步。
                //CCLOG("wall attack or move %d %d", threat, minmap->generalTreatMap.size());
            } else {
                // 找一个非墙目标来进攻，找到则锁定，并进攻！找不到则按非墙的威胁图走一步。
                //CCLOG("un wall attack or move");
            }
        }

    }
}
/*
void DDMapData::tickStepPerMinMap_farEnemy(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{

}
void DDMapData::tickStepPerMinMap_nearEnemy(const std::shared_ptr<DDMinMap>& minmap, DDAgent* agent)
{
    
}*/

/*

void DDMapData::tickStepPerMinMap_miners(const MapPos& mappos)
{
    auto minmap = _minMaps[mappos];
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;
    //挖矿机对机能范围内最近(随机)的矿进行作业
    for (auto miner : minmap->agentMiners) {
        help_computePeriodIndex(miner);
        help_computeUnbeatableAmount(miner, battleObserving);
        if (help_isFirstStageAction(miner)) {
            //机能-挖矿
            auto findRes = help_findNearestRandomInScope(minmap->agentMines, miner->pos, miner->actionDistance);
            if (findRes.first) {
                bool isExausted = findRes.second->amount <= miner->mineSpeed;
                int howMuchMinded = isExausted ? findRes.second->amount : miner->mineSpeed;
                findRes.second->amount = isExausted ? 0 : findRes.second->amount - miner->mineSpeed;
                int aid_to = findRes.second->aid;
                resourceMineAmount += howMuchMinded;
                if (isExausted) {
                    // TODO template version has compile error
//                    help_deleteAgentFromMinmap(minmap->agentMines, minmap->agentMap, miner->pos);
                    help_deleteAgentFromMinmap(minmap, findRes.second->pos);
                    _bigmapFieldObserver(DDBigmapObserveMessage::MINE_EXAUSTED, mappos);
                }
                if (battleObserving) {
                    _battleFieldObserver(DDBattleObserveMessage::MINE_SUCCESS, miner->aid, aid_to);
                }
            } else {
                if (battleObserving) {
                    _battleFieldObserver(DDBattleObserveMessage::MINE_IDLE, miner->aid, -1);
                }
            }

        }
        if (help_isSecondStageAction(miner)) {
            //自疗
            if (help_computeSelfCureIndex(miner)) {
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::SELF_CURE, miner->aid, -1);
            }
        }
    }
}

void DDMapData::tickStepPerMinMap_waterPools(const MapPos& mappos)
{
    auto minmap = _minMaps[mappos];
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;
    //水塘对机能范围内所有敌军或我军进行治疗。
    for (auto pool : minmap->agentWaterPools) {
        help_computePeriodIndex(pool);
        help_computeUnbeatableAmount(pool, battleObserving);
        if (help_isFirstStageAction(pool)) {
            //机能-治疗
            std::vector<DDActiveAgent*> agentsToBeCure;
            for (auto a : help_findAllInCertainScope(minmap->agentMiners, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }
            for (auto a : help_findAllInCertainScope(minmap->agentWalls, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }
            for (auto a : help_findAllInCertainScope(minmap->agentLightHouses, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }
            for (auto a : help_findAllInCertainScope(minmap->agentArrowTowers, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }
            for (auto a : help_findAllInCertainScope(minmap->agentCannonTowers, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }
            for (auto a : help_findAllInCertainScope(minmap->agentNearEnemies, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }
            for (auto a : help_findAllInCertainScope(minmap->agentFarEnemies, pool->pos, pool->actionDistance)) {
                agentsToBeCure.push_back(a);
            }


            for (auto a : agentsToBeCure) {
                a->blood = std::min(a->blood + pool->cure, a->bloodMax);
                if (battleObserving) {
                    _battleFieldObserver(DDBattleObserveMessage::CURE, pool->aid, a->aid);
                }
            }
        }
    }

}

std::vector<DDActiveAgent*> help_findFriendActiveAgentsInScope(std::shared_ptr<DDMinMap> minmap, AgentPos agentpos, int scope, std::function<bool(DDActiveAgent*)> filter)
{
    std::vector<DDActiveAgent*> res;
    for (auto a : help_findAllInCertainScope(minmap->agentWalls, agentpos, scope)) {
        res.push_back(a);
    }
    for (auto a : help_findAllInCertainScope(minmap->agentMiners, agentpos, scope)) {
        res.push_back(a);
    }
    for (auto a : help_findAllInCertainScope(minmap->agentLightHouses, agentpos, scope)) {
        res.push_back(a);
    }
    for (auto a : help_findAllInCertainScope(minmap->agentArrowTowers, agentpos, scope)) {
        res.push_back(a);
    }
    for (auto a : help_findAllInCertainScope(minmap->agentCannonTowers, agentpos, scope)) {
        res.push_back(a);
    }
    for (auto a : help_findAllInCertainScope(minmap->agentCureTowers, agentpos, scope)) {
        res.push_back(a);
    }
    if (filter) {
        std::vector<DDActiveAgent*> ret;
        for (auto a : res) {
            if (filter(a)) ret.push_back(a);
        }
        return ret;
    } else {
        return res;
    }
}

std::vector<DDActiveAgent*> help_findEnemyActiveAgentsInScope(std::shared_ptr<DDMinMap> minmap, AgentPos agentpos, int scope, std::function<bool(DDActiveAgent*)> filter)
{
    std::vector<DDActiveAgent*> res;
    for (auto a : help_findAllInCertainScope(minmap->agentNearEnemies, agentpos, scope)) {
        res.push_back(a);
    }
    for (auto a : help_findAllInCertainScope(minmap->agentFarEnemies, agentpos, scope)) {
        res.push_back(a);
    }
    if (filter) {
        std::vector<DDActiveAgent*> ret;
        for (auto a : res) {
            if (filter(a)) ret.push_back(a);
        }
        return ret;
    } else {
        return res;
    }
}


void DDMapData::tickStepPerMinMap_cureTowers(const MapPos& mappos)
{

    auto minmap = _minMaps[mappos];
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;
    //挖矿机对机能范围内最近(随机)的矿进行作业
    for (auto cureTower : minmap->agentCureTowers) {
        help_computePeriodIndex(cureTower);
        help_computeUnbeatableAmount(cureTower, battleObserving);
        if (help_isFirstStageAction(cureTower)) {
            //机能-治疗其它范围内某数量的友军(可能包括自己)
            auto allFriendInScope = help_findFriendActiveAgentsInScope(minmap, cureTower->pos, cureTower->actionDistance, [](DDActiveAgent* a)->bool{return a->blood < a->bloodMax;});
            std::sort(allFriendInScope.begin(), allFriendInScope.end(), [](DDActiveAgent* a, DDActiveAgent* b){
                return a->blood * 1.f / a->bloodMax < b->blood * 1.f / b->bloodMax;
            });
            for (int i = 0; i < cureTower->cureNum && i < allFriendInScope.size(); i++) {
                allFriendInScope[i]->blood = std::min(allFriendInScope[i]->bloodMax, allFriendInScope[i]->blood + cureTower->cure);
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::CURE, cureTower->aid, allFriendInScope[i]->aid);
            }
        }
        if (help_isSecondStageAction(cureTower)) {
            //自疗
            if (help_computeSelfCureIndex(cureTower)) {
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::SELF_CURE, cureTower->aid, -1);
            }
        }
    }
}

void DDMapData::tickStepPerMinMap_volcanos(const MapPos& mappos)
{
    auto minmap = _minMaps[mappos];
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;
    //火山对范围内所有的agent进行攻击
    for (auto volcano : minmap->agentVolcanos) {
        help_computePeriodIndex(volcano);
        help_computeUnbeatableAmount(volcano, battleObserving);
        if (help_isFirstStageAction(volcano)) {
            //机能
            auto allFriendInScope = help_findFriendActiveAgentsInScope(minmap, volcano->pos, volcano->actionDistance, nullptr);
            auto allEnemyInScope = help_findEnemyActiveAgentsInScope(minmap, volcano->pos, volcano->actionDistance, nullptr);

            auto funceach = [this, volcano, minmap, battleObserving](DDActiveAgent* des) {
                des->blood -= des->unbeatableAmount >0 ? 0 : volcano->attack;
                int to_aid = des->aid;
                if (des->blood <= 0) {
                    help_deleteAgentFromMinmap(minmap, des->pos);
                }
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::ATTACK, volcano->aid, to_aid);
            };
            for (auto a : allFriendInScope) {
                funceach(a);
            }
            for (auto a : allEnemyInScope) {
                funceach(a);
            }
        }
    }
}

std::pair<bool, DDActiveAgent*> help_findNearestRandomEnemy(std::shared_ptr<DDMinMap> minmap, AgentPos agentpos, int scope)
{
    auto nearResRaw = help_findNearestRandomInScope(minmap->agentNearEnemies, agentpos, scope);
    auto farResRaw = help_findNearestRandomInScope(minmap->agentFarEnemies, agentpos, scope);
    std::pair<bool, DDActiveAgent*> nearRes = {nearResRaw.first, nearResRaw.second};
    std::pair<bool, DDActiveAgent*> farRes = {farResRaw.first, farResRaw.second};

    if (nearRes.first && !farRes.first) {
        return {true, nearRes.second};
    }
    if (farRes.first && !nearRes.first) {
        return {true, farRes.second};
    }
    if (!nearRes.first && !farRes.first) {
        return {false, nullptr};
    }
    if (nearRes.first && farRes.first) {
        int nearDis = nearRes.second->pos.distance(agentpos);
        int farDis = farRes.second->pos.distance(agentpos);
        if (nearDis == farDis) return rand_0_1() < 0.5f ? nearRes : farRes;
        else return nearDis < farDis ? nearRes : farRes;
    }
    return {false, nullptr};
}

void DDMapData::tickStepPerMinMap_arrowTowers(const MapPos& mappos)
{
    auto minmap = _minMaps[mappos];
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;

    for (auto arrowTower : minmap->agentArrowTowers) {
        help_computePeriodIndex(arrowTower);
        help_computeUnbeatableAmount(arrowTower, battleObserving);
        if (help_isFirstStageAction(arrowTower)) {
            //机能-攻击最近的敌人
            auto enemy = help_findNearestRandomEnemy(minmap, arrowTower->pos, arrowTower->actionDistance);

            auto attackFunc = [this, arrowTower, minmap, battleObserving](DDActiveAgent* des) {
                des->blood -= des->unbeatableAmount >0 ? 0 : arrowTower->attack;
                int to_aid = des->aid;
                if (des->blood <= 0) {
                    help_deleteAgentFromMinmap(minmap, des->pos);
                }
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::ATTACK, arrowTower->aid, to_aid);
            };

            if (enemy.first) {
                attackFunc(enemy.second);
            }
        }
        if (help_isSecondStageAction(arrowTower)) {
            //自疗
            if (help_computeSelfCureIndex(arrowTower)) {
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::SELF_CURE, arrowTower->aid, -1);
            }
        }

    }
}

void DDMapData::tickStepPerMinMap_cannonTowers(const MapPos& mappos)
{
    auto minmap = _minMaps[mappos];
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;

    for (auto cannonTower : minmap->agentCannonTowers) {
        help_computePeriodIndex(cannonTower);
        help_computeUnbeatableAmount(cannonTower, battleObserving);
        if (help_isFirstStageAction(cannonTower)) {
            //机能-攻击最近的敌人，同时溅射周边的敌人
            // TODO 能够找到最集中的敌人群！
            auto enemy = help_findNearestRandomEnemy(minmap, cannonTower->pos, cannonTower->actionDistance);

            if (enemy.first) {
                auto sputtEnemy = help_findEnemyActiveAgentsInScope(minmap, enemy.second->pos, cannonTower->sputtDistance, nullptr);

                auto attackFunc = [this, cannonTower, minmap, battleObserving](DDActiveAgent* des) {
                    des->blood -= des->unbeatableAmount >0 ? 0 : cannonTower->attack;
                    int to_aid = des->aid;
                    if (des->blood <= 0) {
                        help_deleteAgentFromMinmap(minmap, des->pos);
                    }
                    if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::ATTACK, cannonTower->aid, to_aid);
                };

                attackFunc(enemy.second);

                auto sputtAttackFunc = [this, cannonTower, minmap, battleObserving](DDActiveAgent* des) {
                    des->blood -= des->unbeatableAmount >0 ? 0 : cannonTower->attack*cannonTower->sputtDamageRadio;
                    int to_aid = des->aid;
                    if (des->blood <= 0) {
                        help_deleteAgentFromMinmap(minmap, des->pos);
                    }
                    if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::ATTACK_SPUTT, cannonTower->aid, to_aid);
                };
                for (auto spdes : sputtEnemy) {
                    sputtAttackFunc(spdes);
                }
            }
        }
        if (help_isSecondStageAction(cannonTower)) {
            //自疗
            if (help_computeSelfCureIndex(cannonTower)) {
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::SELF_CURE, cannonTower->aid, -1);
            }
        }
    }
}

void DDMapData::tickStepPerMinMap_core(const MapPos& mappos)
{
    auto minmap = _minMaps[mappos];
    if (!minmap->isCore()) return;
    bool battleObserving = isBattleFieldObserving && mappos == battleFieldObservedMinMap;
    auto core = minmap->agentCore;
    help_computePeriodIndex(core);
    help_computeUnbeatableAmount(core, battleObserving);
    if (help_isFirstStageAction(core)) {
        //机能-群攻，单攻
        auto enemy = help_findNearestRandomEnemy(minmap, core->pos, core->actionDistance);

        if (enemy.first) {
            auto attackFunc = [this, core, minmap, battleObserving](DDActiveAgent* des) {
                des->blood -= des->unbeatableAmount >0 ? 0 : core->attack;
                int to_aid = des->aid;
                if (des->blood <= 0) {
                    help_deleteAgentFromMinmap(minmap, des->pos);
                }
                if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::ATTACK, core->aid, to_aid);
            };
            attackFunc(enemy.second);
        }
    }
    if (help_isSecondStageAction(core)) {
        //自疗
        if (help_computeSelfCureIndex(core)) {
            if (battleObserving) _battleFieldObserver(DDBattleObserveMessage::SELF_CURE, core->aid, -1);
        }
    }
}

void DDMapData::tickStepPerMinMap_nearEnemies(const MapPos& mappos)
{

}

void DDMapData::tickStepPerMinMap_farEnemies(const MapPos& mappos)
{

}

void DDMapData::tickStepPerMinMap_enemyNests (const MapPos& mappos)
{

}*/


void DDMapData::appearAgent_enemy(const std::shared_ptr<DDMinMap>& minmap, const AgentPos& agentpos, DDAgent* nest, bool battleObserving)
{
    // 新产生敌人。
    auto enemy = new DDAgent();
    enemy->aid = nextAgentId();
    enemy->pos = agentpos;

    bool isNear = rand_0_1() < nest->nestChanceToNear;
    bool isBoss = rand_0_1() < nest->nestChanceToBoss;

    enemy->type = isNear ? DDAgent::AT_ENEMY_NEAR : DDAgent::AT_ENEMY_FAR;
    enemy->level = 0;//TODO
    enemy->bloodMax = enemy->blood = nest->nestBlood * (isBoss ? 2:1);
    enemy->attack = nest->nestAttack;
    enemy->actionDistance = isNear ? nest->nestAttackDistanceNear : nest->nestAttackDistanceFar;
    enemy->actionPeriod = enemy->actionPeriodIndex = nest->nestAttackPeriod;
    enemy->elementType = nest->nestElementType;

    minmap->posAgentMap[agentpos] = enemy;
    minmap->aidAgentMap[enemy->aid] = enemy;

    // 通知
    if (battleObserving) {
        sendBattleFieldMessage(DDBattleObserveMessage::ENEMY_NEW, nest->aid, enemy->aid);
    }
}

// 新产生友军建筑，总是在battleObserved的minmap，总是会observing。
void DDMapData::appearAgent_friend(const AgentPos& agentpos, int friendAgentType)
{
    CCLOG("add friend type = %d", friendAgentType);
    auto agent = DDUpgradeRoadMap::generateInitFriendAgents(friendAgentType);
    agent->aid = nextAgentId();
    agent->pos = agentpos;
    getBattleObservedMinMap()->posAgentMap[agentpos] = agent;
    getBattleObservedMinMap()->aidAgentMap[agent->aid] = agent;
    sendBattleFieldMessage(DDBattleObserveMessage::FRIEND_NEW, agent->aid, -1);
}

// 卖出友军建筑，或者，移出3RD的石头和树，总是在battleObserved的minmap，总是会observing。
void DDMapData::removeAgent(const AgentPos& agentpos)
{

}