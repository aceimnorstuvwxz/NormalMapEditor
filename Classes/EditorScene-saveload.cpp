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
    auto fn = fmt::sprintf("/Users/chenbingfeng/Documents/NormalMapEditor/%s.json", EditState::s()->_moduleName);

    //clear
    for (auto a: _points) {
        a.clear();
    }
    for (auto b : _triangles) {
        b.clear();
    }
    for (auto c : _triangleColorMap) {
        c.clear();
    }

    //load
    FILE* fp = fopen(fn.c_str(), "r");

    if (fp == NULL) {
        CCLOG("load, file %s do not exist.", fn.c_str());
        return;
    }

    char readBuffer[65536];
    rjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    assert(doc.HasMember("frames"));
    auto& data = doc["frames"];
    for (auto iter = data.Begin(); iter != data.End(); iter++){
        int frameIndex = (*iter)["frameIndex"].GetInt();

        // pidIndex
        _pidIndex[frameIndex] = (*iter)["pidIndex"].GetInt();

        // points
        auto& points = (*iter)["points"];
        for (auto jter = points.Begin(); jter != points.End(); jter++) {
            auto point = std::make_shared<EEPoint>();
            point->pid = (*jter)["pid"].GetInt();
            point->position.x = (*jter)["x"].GetDouble();
            point->position.y = (*jter)["y"].GetDouble();
            point->height = (*jter)["height"].GetDouble();
            point->sprite = Sprite::create("images/point_normal.png");
            point->sprite->setPosition(help_relativePosition2editPosition(point->position));
            point->sprite->setZOrder(Z_POINTS);
            point->sprite->setScale(DOT_SCALE);
            _pointLayer->addChild(point->sprite);
            point->sprite->setVisible(false);
            _points[frameIndex][point->pid] = point;
        }

        // triangles
        auto& triangles = (*iter)["triangles"];
        for (auto jter = triangles.Begin(); jter != triangles.End(); jter++) {
            auto triangle = std::make_shared<EETriangle>();
            triangle->a = _points[frameIndex][(*jter)["pa"].GetInt()];
            triangle->b = _points[frameIndex][(*jter)["pb"].GetInt()];
            triangle->c = _points[frameIndex][(*jter)["pc"].GetInt()];

            _triangleColorMap[frameIndex][triangle->calcKey()] = Vec4{static_cast<float>((*jter)["cr"].GetDouble()),static_cast<float>((*jter)["cg"].GetDouble()),static_cast<float>((*jter)["cb"].GetDouble()),static_cast<float>((*jter)["ca"].GetDouble())};
            triangle->color = _triangleColorMap[frameIndex][triangle->calcKey()];
            _triangles[frameIndex].push_back(triangle);
        }
    }
    switchAllDots(true);
}
