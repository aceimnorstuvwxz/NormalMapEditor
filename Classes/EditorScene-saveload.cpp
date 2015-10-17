// (C) 2015 Turnro.com

#include "EditorScene.h"
#include "TRLocale.h"
#include "format.h"
#include "intersection.h"
#include "EditState.h"

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/filewritestream.h"


USING_NS_CC;

void EditorScene::save()
{
    CCLOG("save");

    // buildup json data
    rjson::Document doc;
    doc.SetObject();
    rjson::Value data;
    data.SetArray();

    for (int frameIndex = 0; frameIndex < NUM_FRAME; frameIndex++) {
        rjson::Value obj;
        obj.SetObject();

        // frame index
        obj.AddMember("frameIndex", frameIndex, doc.GetAllocator());

        // pid max
        obj.AddMember("pidIndex", _pidIndex[frameIndex], doc.GetAllocator());

        // points
        rjson::Value points;
        points.SetArray();
        for (auto pair : _points[frameIndex]) {
            rjson::Value point;
            point.SetObject();
            point.AddMember("pid", pair.second->pid, doc.GetAllocator());
            point.AddMember("x", pair.second->position.x, doc.GetAllocator());
            point.AddMember("y", pair.second->position.y, doc.GetAllocator());
            point.AddMember("height", pair.second->height, doc.GetAllocator());
            points.PushBack(point, doc.GetAllocator());
        }
        obj.AddMember("points", points, doc.GetAllocator());

        // triangles
        rjson::Value triangles;
        triangles.SetArray();
        for (auto tri : _triangles[frameIndex]) {
            rjson::Value triangle;
            triangle.SetObject();
            triangle.AddMember("pa", tri->a->pid, doc.GetAllocator());
            triangle.AddMember("pb", tri->b->pid, doc.GetAllocator());
            triangle.AddMember("pc", tri->c->pid, doc.GetAllocator());
            triangle.AddMember("cr", tri->color.x, doc.GetAllocator());
            triangle.AddMember("cg", tri->color.y, doc.GetAllocator());
            triangle.AddMember("cb", tri->color.z, doc.GetAllocator());
            triangle.AddMember("ca", tri->color.w, doc.GetAllocator());
            triangles.PushBack(triangle, doc.GetAllocator());
        }
        obj.AddMember("triangles", triangles, doc.GetAllocator());
        
        data.PushBack(obj, doc.GetAllocator());
    }
    doc.AddMember("frames", data, doc.GetAllocator());


    auto fn = fmt::sprintf("/Users/chenbingfeng/Documents/NormalMapEditor/%s.json", EditState::s()->_moduleName);

    FILE* fp = fopen(fn.c_str(), "w");
    assert(fp != NULL);

    char writeBuffer[65536];
    rjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    rjson::Writer<rjson::FileWriteStream> writer(os);
    doc.Accept(writer);
    fclose(fp);
}


void EditorScene::load()
{
    CCLOG("load");
    /*
    //clear
    _data.clear();
    _floatAssets.clear();
    _abyssAssets.clear();
    _skyAssets.clear();

    //load
    FILE* fp = fopen(file.c_str(), "r");

    if (fp == NULL) {
        CCLOG("error, file %s do not exist.", file.c_str());
        return;
    }

    char readBuffer[65536];
    rjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

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
        if ((*iter).HasMember("glass")) {
            int glass = (*iter)["glass"].GetInt();
            mynode._glass = glass != 0;
        } else {
            mynode._glass = false;
        }
        // end
        if ((*iter).HasMember("end")) {
            int end = (*iter)["end"].GetInt();
            mynode._end = end != 0;
        } else {
            mynode._end = false;
        }
        // gate
        if ((*iter).HasMember("gate")) {
            int gate = (*iter)["gate"].GetInt();
            mynode._gate = gate != 0;
        } else {
            mynode._gate = false;
        }

        // uncover
        if ((*iter).HasMember("uncover")) {
            int uncover = (*iter)["uncover"].GetInt();
            mynode._uncover = uncover != 0;
        } else {
            mynode._uncover = false;
        }

        _data.push_back(mynode);
    }
    if (doc.HasMember("float_assets")) {
        // 加载assets，即“轨道”周边的建筑物的基点。。。
        auto& assets = doc["float_assets"];
        for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
            FloatAssetNode assetNode;
            assetNode._position.x = (*iter)["x"].GetDouble();
            assetNode._position.y = (*iter)["y"].GetDouble();
            assetNode._position.z = (*iter)["z"].GetDouble();
            _floatAssets.push_back(assetNode);
        }
    }
    if (doc.HasMember("abyss_assets")) {
        // 加载assets，即“轨道”周边的建筑物的基点。。。
        auto& assets = doc["abyss_assets"];
        for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
            AbyssAssetNode assetNode;
            assetNode._position.x = (*iter)["x"].GetDouble();
            assetNode._position.y = (*iter)["y"].GetDouble();
            assetNode._position.z = (*iter)["z"].GetDouble();
            _abyssAssets.push_back(assetNode);
        }
    }
    if (doc.HasMember("sky_assets")) {
        // 加载assets，即“轨道”周边的建筑物的基点。。。
        auto& assets = doc["sky_assets"];
        for (auto iter = assets.Begin(); iter != assets.End(); iter++) {
            SkyAssetNode assetNode;
            assetNode._position.x = (*iter)["x"].GetDouble();
            assetNode._position.y = (*iter)["y"].GetDouble();
            assetNode._position.z = (*iter)["z"].GetDouble();
            _skyAssets.push_back(assetNode);
        }
    }*/
}
