// (C) 2015 Turnro.com

#include "RollNode.h"
#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/filewritestream.h"

USING_NS_CC;
void RollMinMap::load(const std::string& file)
{
    //clear
    _data.clear();

    //load
    auto filedata = FileUtils::getInstance()->getStringFromFile(file);

    rjson::Document doc;
    doc.Parse(filedata.c_str());

    assert(doc.HasMember("data"));
    auto& data = doc["data"];
    for (auto iter = data.Begin(); iter != data.End(); iter++){
        RollNode mynode;
        // position
        auto& position = (*iter)["position"];
        int tmpIndex = 0;
        assert(position.Size() == 3);
        for (auto jter = position.Begin(); jter != position.End(); jter++, tmpIndex++) {
            if (tmpIndex == 0) {
                mynode._position.x = jter->GetDouble();
            } else if (tmpIndex == 1) {
                mynode._position.y = jter->GetDouble();
            } else {
                mynode._position.z = jter->GetDouble();
            }
        }
        // posture
        auto& normal = (*iter)["posture"];
        tmpIndex = 0;
        assert(normal.Size() == 4);
        for (auto jter = normal.Begin(); jter != normal.End(); jter++, tmpIndex++) {
            if (tmpIndex == 0) {
                mynode._posture.x = jter->GetDouble();
            } else if (tmpIndex == 1) {
                mynode._posture.y = jter->GetDouble();
            } else if (tmpIndex == 2){
                mynode._posture.z = jter->GetDouble();
            } else {
                mynode._posture.w = jter->GetDouble();
            }
        }
        // type
        mynode._type = (*iter)["type"].GetInt();
        // glass
        mynode._glass = (*iter)["glass"].GetInt() != 0;
        // end
        mynode._end = (*iter)["end"].GetInt() != 0;
        if (mynode._end) _end = true;

        // gate
        if (iter->HasMember("gate")) {
            mynode._gate = (*iter)["gate"].GetInt() != 0;
        } else {
            mynode._gate = false;
        }

        // uncover
        if (iter->HasMember("uncover")) {
            mynode._uncover = (*iter)["uncover"].GetInt() != 0;
        } else {
            mynode._uncover = false;
        }

        _data.push_back(mynode);
    }

    // Assets 轨道周边的物体
    // 兼容老的，作为float assets
    {
        if (doc.HasMember("assets")) {
            auto& assets = doc["assets"];
            for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
                FloatAssetNode assetNode;
                assetNode._position.x = (*iter)["x"].GetDouble();
                assetNode._position.y = (*iter)["y"].GetDouble();
                assetNode._position.z = (*iter)["z"].GetDouble();
                _floatAssets.push_back(assetNode);
            }
        }
    }

    // Float Assets
    {
        if (doc.HasMember("float_assets")) {
            auto& assets = doc["float_assets"];
            for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
                FloatAssetNode assetNode;
                assetNode._position.x = (*iter)["x"].GetDouble();
                assetNode._position.y = (*iter)["y"].GetDouble();
                assetNode._position.z = (*iter)["z"].GetDouble();
                _floatAssets.push_back(assetNode);
            }
        }
    }
    // Abyss Assets
    {
        if (doc.HasMember("abyss_assets")) {
            auto& assets = doc["abyss_assets"];
            for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
                AbyssAssetNode assetNode;
                assetNode._position.x = (*iter)["x"].GetDouble();
                assetNode._position.y = (*iter)["y"].GetDouble();
                assetNode._position.z = (*iter)["z"].GetDouble();
                _abyssAssets.push_back(assetNode);
            }
        }
    }
    // Float Assets
    {
        if (doc.HasMember("sky_assets")) {
            auto& assets = doc["sky_assets"];
            for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
                SkyAssetNode assetNode;
                assetNode._position.x = (*iter)["x"].GetDouble();
                assetNode._position.y = (*iter)["y"].GetDouble();
                assetNode._position.z = (*iter)["z"].GetDouble();
                _skyAssets.push_back(assetNode);
            }
        }
    }
}