
#include "DDMapData.h"
#include <memory>
#include <cmath>
#include "DDConfig.h"
#include <algorithm>
#include "format.h"

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"

USING_NS_CC;

// 地图数据的纯和取


// 从模板中获取每个minmap的范本
void DDMapData::loadMinmapTemplates()
{
    CCLOG("load minmap templates");
    std::string templateFilePath = fmt::sprintf("mapdata/%d.json", _templateIndex);

    _templateMinmaps.clear();

    auto filedata = FileUtils::getInstance()->getStringFromFile(templateFilePath);

    rjson::Document doc;
    doc.Parse(filedata.c_str());

    _unblockedMinmapTotal = doc["minmap_unblocked_count"].GetInt();

    assert(doc.HasMember("minmaps"));
    auto& minmaps = doc["minmaps"];
    for (auto iter = minmaps.Begin(); iter != minmaps.End(); iter++){
        auto minmap = DDMinMap::create();
        minmap->pos.x = (*iter)["pos"]["x"].GetInt();
        minmap->pos.y = (*iter)["pos"]["y"].GetInt();
        minmap->state = DDMinMap::T_NON_ACTIVE;
        minmap->blocked = !((*iter)["blocked"].GetInt() == 0);
        if (minmap->blocked) {
            _templateMinmaps[minmap->pos] = minmap;
            continue;
        }
        minmap->mainElementType = static_cast<DDElementType>( (*iter)["main_element_type"].GetInt());
        minmap->secondaryElementType = static_cast<DDElementType>( (*iter)["secondary_element_type"].GetInt());

        auto& agents = (*iter)["agents"];
        for(auto jter = agents.Begin(); jter != agents.End(); jter++) {
            auto agent = new DDAgent();
            agent->pos.x = (*jter)["pos"]["x"].GetInt();
            agent->pos.y = (*jter)["pos"]["y"].GetInt();
            agent->type = (*jter)["type"].GetInt();
            agent->blood = agent->bloodMax = (*jter)["blood"].GetInt();
            agent->attack = (*jter)["attack"].GetInt();
            agent->cure = (*jter)["cure"].GetInt();
            agent->actionDistance = (*jter)["action_distance"].GetInt();
            agent->actionPeriod = agent->actionPeriodIndex = (*jter)["action_period"].GetInt();
            agent->mineCapacity = agent->mineAmount = (*jter)["mine_capacity"].GetInt();
            agent->cannonSputtDamageRadio = 0.25f;//TODO
            agent->cannonSputtDistance = 1;
            agent->isBoss = false;
            agent->nestChanceToRelax = (*jter)["chance_to_relax"].GetDouble();
            agent->nestActionRelaxPeriod = (*jter)["action_relax_reriod"].GetInt();
            agent->nestActionRelaxIndex = 0;
            agent->nestChanceToNear = (*jter)["nest_chance_to_near"].GetDouble();
            agent->nestChanceToBoss = (*jter)["nest_chance_to_boss"].GetDouble();
            agent->nestBlood = (*jter)["nest_blood"].GetInt();
            agent->nestAttack = (*jter)["nest_attack"].GetInt();
            agent->nestElementType = static_cast<DDElementType>((*jter)["nest_element_type"].GetInt());
            agent->nestAttackDistanceNear = (*jter)["nest_attack_distance_near"].GetInt();
            agent->nestAttackDistanceFar = (*jter)["nest_attack_distance_far"].GetInt();
            agent->nestAttackPeriod = (*jter)["nest_attack_period"].GetInt();
            minmap->posAgentMap[agent->pos] = agent;
            minmap->aidAgentMap[agent->aid] = agent;
        }
        _templateMinmaps[minmap->pos] = minmap;
    }
}


void DDMapData::help_movefromTemplate2preseting(const MapPos& mappos)
{
    // check
    if (_presentingMinmaps.find(mappos) != _presentingMinmaps.end()) {
        return;
    }
    if (_templateMinmaps.find(mappos) == _templateMinmaps.end()) {
        return;
    }

    //move
    auto minmap = _templateMinmaps[mappos];
    minmap->state = minmap->isCore() ? DDMinMap::T_ACTIVE: DDMinMap::T_ACTIVABLE;
    _presentingMinmaps[mappos] = minmap;

    //都是在实际加入presetings的时候进行threat map的初始计算
    help_calcMinMapThreatMapAll(mappos);
}

void DDMapData::help_makeArroundActivable(const MapPos& mappos)
{
    help_movefromTemplate2preseting({mappos.x,mappos.y+1});
    help_movefromTemplate2preseting({mappos.x,mappos.y-1});
    help_movefromTemplate2preseting({mappos.x-1,mappos.y});
    help_movefromTemplate2preseting({mappos.x+1,mappos.y});
}

void DDMapData::loadPresentingMinmapsForNew()
{
    help_movefromTemplate2preseting({0,0}); //核心
    help_makeArroundActivable({0,0}); //核心周边
//    help_makeArroundActivable({1,0}); //核心周边
//    help_makeArroundActivable({2,0}); //核心周边
//    help_makeArroundActivable({2,0}); //核心周边

}

void DDMapData::loadPresentingMinmapsFromSavedGame()
{
    
}


