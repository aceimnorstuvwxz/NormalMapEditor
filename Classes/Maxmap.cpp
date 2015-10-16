// (C) 2015 Turnro.com

#include <string>
#include <fstream>
#include <iostream>
#include "Maxmap.h"
#include "cocos2d.h"
#include "format.h"

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"

USING_NS_CC;
void Maxmap::load(const std::string& maxmap, bool infiniteMode)
{
    _infinite = infiniteMode;
    loadMaxmapRawList(maxmap);
    loadRawMinmaps();
    if (!infiniteMode) {
        cookMinmapsAll();
    }
}

bool Maxmap::checkIndex(int index)
{
    if (_infinite) {
        return true;
    } else {
        return index < _maxmapRawList.size();
    }
}

void Maxmap::loadMaxmapRawList(const std::string& maxmap)
{
    _maxmapRawList.clear();

    auto data = FileUtils::getInstance()->getStringFromFile(maxmap);

    rjson::Document doc;
    doc.Parse(data.c_str());

    auto& list = doc["list"];
    for (auto iter = list.Begin(); iter != list.End(); iter++) {
        std::string minmap = (*iter).GetString();
        _maxmapRawList.push_back(minmap);
    }
    CCLOG("minmap list count=%lu", _maxmapRawList.size());
}

void Maxmap::loadRawMinmaps()
{
    // 这个raw的minmaps不用进行clear，这些min可以在不同max中通用。
    for (auto iter = _maxmapRawList.begin(); iter != _maxmapRawList.end(); iter++) {
        if (_rawMinmaps.find(*iter) == _rawMinmaps.end()) {
            auto p = std::make_shared<RollMinMap>();
            _rawMinmaps[*iter] = p;
            p->load(fmt::sprintf("maps/minmaps/%s.minmap", *iter));
        }
    }
    CCLOG("unique minmap count=%lu", _rawMinmaps.size());
}


void Maxmap::cookMinmapAppend(const std::string& minmapName)
{
    auto minmap = _rawMinmaps[minmapName];
    if (_cookedMinmaps.size() == 0) {
        //如果之前不存在，那么直接拷贝过去。
        _cookedMinmaps.push_back(std::unique_ptr<RollMinMap>(new RollMinMap()));
        (*_cookedMinmaps.begin())->_data = minmap->_data;
        CCLOG("first minmap copyed %lu", (*_cookedMinmaps.begin())->_data.size());

        // 复制assets信息
        auto& newCookedMinmap = _cookedMinmaps.back();
        newCookedMinmap->_floatAssets = minmap->_floatAssets;
        newCookedMinmap->_skyAssets = minmap->_skyAssets;
        newCookedMinmap->_abyssAssets = minmap->_abyssAssets;
        newCookedMinmap->_end = minmap->_end;
    } else {
        // 第一个和之前的最后一个拟合，然后后续的所有节点都要进化。
        // 找到末尾的RollMinMap和RollNode
        auto& lastMinimap = _cookedMinmaps.back();
        RollNode targetNodeOfLastMiniMap = lastMinimap->_data.back();
        RollNode firstNodeOfCurrentMiniMap = minmap->_data.front();

        auto inversed = firstNodeOfCurrentMiniMap._posture.getInversed();
        auto notinversed = firstNodeOfCurrentMiniMap._posture;
        auto rotate = firstNodeOfCurrentMiniMap._posture.getInversed();
        rotate.multiply(targetNodeOfLastMiniMap._posture);

        Vec3 move = targetNodeOfLastMiniMap._position - firstNodeOfCurrentMiniMap._position;
        Vec3 anchorPos = firstNodeOfCurrentMiniMap._position;
        _cookedMinmaps.push_back(std::unique_ptr<RollMinMap>(new RollMinMap()));
        auto& newMinimap = _cookedMinmaps.back();
        auto jter = minmap->_data.begin();
        jter++;
        auto oldRotateInverse = firstNodeOfCurrentMiniMap._posture.getInversed();
        bool flagCookassets = true;
        for (; jter != minmap->_data.end(); jter++) {
            newMinimap->_data.push_back(*jter);
            auto& node = newMinimap->_data.back();
            auto newPostureRaw = inversed;
            newPostureRaw.multiply(node._posture);
            auto newPosture = rotate;
            newPosture.multiply(newPostureRaw);
            auto final = notinversed;//注意这里不是node._posture。
            final.multiply(newPosture);
            node._posture = final;

            node._position = anchorPos + targetNodeOfLastMiniMap._posture * (oldRotateInverse*(node._position - anchorPos));
            node._position += move;

            if (flagCookassets) {
                flagCookassets = false;
                for (auto zter = minmap->_floatAssets.begin(); zter != minmap->_floatAssets.end(); zter++) {
                    FloatAssetNode asset = *zter;
                    asset._position = anchorPos + targetNodeOfLastMiniMap._posture * (oldRotateInverse*(asset._position - anchorPos));
                    newMinimap->_floatAssets.push_back(asset);
                }
                for (auto zter = minmap->_abyssAssets.begin(); zter != minmap->_abyssAssets.end(); zter++) {
                    AbyssAssetNode asset = *zter;
                    asset._position = anchorPos + targetNodeOfLastMiniMap._posture * (oldRotateInverse*(asset._position - anchorPos));
                    newMinimap->_abyssAssets.push_back(asset);
                }
                for (auto zter = minmap->_skyAssets.begin(); zter != minmap->_skyAssets.end(); zter++) {
                    SkyAssetNode asset = *zter;
                    asset._position = anchorPos + targetNodeOfLastMiniMap._posture * (oldRotateInverse*(asset._position - anchorPos));
                    newMinimap->_skyAssets.push_back(asset);
                }
            }
        }
        newMinimap->_end = minmap->_end;
    }

}

void Maxmap::cookMinmapsAll()
{
    this->_cookedMinmaps.clear();
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    // 烘焙所有的道路
    for (auto iter = _maxmapRawList.begin(); iter != _maxmapRawList.end(); iter++) {
        cookMinmapAppend(*iter);
    }
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    for (int i = 0; i < _cookedMinmaps.size(); i++) {
        if (_cookedMinmaps[i]->_end) {
            _endIndex = i;break;
        }
    }

    std::cout << "finished computation at " << std::ctime(&end_time)
    << "elapsed time: " << elapsed_seconds.count() << "s\n";
    CCLOG("%lu", _cookedMinmaps.size());
}

std::unique_ptr<RollMinMap>&  Maxmap::getCookedMinMap(int index)
{
    if (index >= _cookedMinmaps.size()) {
        // 依次烘焙RawList中[_cookedMinmaps.size(), index]这些minmaps
        for (int i = static_cast<int>(_cookedMinmaps.size()); i <= index; i++) {
            cookMinmapAppend(_maxmapRawList[i]);
        }
    }
    return _cookedMinmaps[index];
}


int Maxmap::getSize()
{
    return static_cast<int>(_cookedMinmaps.size());
}

int Maxmap::getEndIndex()
{
    return _endIndex;
}
