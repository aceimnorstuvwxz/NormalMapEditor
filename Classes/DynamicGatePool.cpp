// (C) 2015 Turnro.com

#include "DynamicGatePool.h"
#include "GateShape.h"
#include "EditState.h"

USING_NS_CC;

inline cocos2d::Vec3 fetchGatePoint(int pos)
{
    int index = gate_index[pos];
    return {gate_position[index*8],gate_position[index*8+1],gate_position[index*8+2]};
}

PixelPlane* DynamicGatePool::genIdleGlass()
{

    auto node = PixelPlane::create("3d/pp_gate.fsh",EditState::s()->getBgmTextureFile());
    node->setScale(1.5);
    node->setCameraMask(_camera->getCameraMask());
    _layer->addChild(node);
    int INDEX_LEN = sizeof(gate_index)/sizeof(int);
    for (int i = 0; i < INDEX_LEN; i += 6) {
        node->drawTriangle(fetchGatePoint(i), fetchGatePoint(i+1), fetchGatePoint(i+2), {0,0},{0,1},{1,0});
        node->drawTriangle(fetchGatePoint(i+3), fetchGatePoint(i+4), fetchGatePoint(i+5), {0,0},{0,1},{1,0});
    }
    node->setVisible(false);
    return node;
}

void DynamicGatePool::init(cocos2d::Layer *layer, cocos2d::Camera *camera)
{
    _layer = layer;
    _camera = camera;
    _idleGlasses.reserve(N);
    for (int i = 0; i < N; i++) {
        _idleGlasses.push_back(genIdleGlass());
    }
}

PixelPlane* DynamicGatePool::brrow()
{
    PixelPlane* ret;
    if (_idleGlasses.size() == 0) {
        CCLOG("dyn gen gate>>>>>>>>>>>>>>");
        ret = genIdleGlass();
    } else {
        ret = _idleGlasses.back();
        _idleGlasses.pop_back();
    }
    ret->setVisible(true);
    ret->setZOrder(_zorderIndex--);
    return ret;
}

void DynamicGatePool::giveback(PixelPlane *glass)
{
    glass->setVisible(false);
    glass->stopAllActions();
    _idleGlasses.push_back(glass);
}