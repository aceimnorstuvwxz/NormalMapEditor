// (C) 2015 Turnro.com

#include "DynamicGlassPool.h"
#include "EditState.h"

USING_NS_CC;

PixelPlane* DynamicGlassPool::genIdleGlass()
{
    auto node = PixelPlane::create("3d/pp_glass.fsh",EditState::s()->getBgmTextureFile());
    node->setScale(1);
    node->setCameraMask(_camera->getCameraMask());
    _layer->addChild(node);
    const float len = 0.5;
    for (int i = 0; i < 5; i++) {
        float z = i*0.2;
        Vec3 bl = {-len,-len,z};
        Vec3 br = {len,-len,z};
        Vec3 tl = {-len,len,z};
        Vec3 tr = {len,len,z};
        node->drawRoad(bl, br, tl, tr);
    }
    node->setGlassLength(len*2);

    node->setVisible(false);
    return node;
}

void DynamicGlassPool::init(cocos2d::Layer *layer, cocos2d::Camera *camera)
{
    _layer = layer;
    _camera = camera;
    _idleGlasses.reserve(N);
    for (int i = 0; i < N; i++) {
        _idleGlasses.push_back(genIdleGlass());
    }
}

PixelPlane* DynamicGlassPool::brrow()
{
    PixelPlane* ret;
    if (_idleGlasses.size() == 0) {
        CCLOG("dyn gen glass>>>>>>>>>>>>>>");
        ret = genIdleGlass();
    } else {
        ret = _idleGlasses.back();
        _idleGlasses.pop_back();
    }
    ret->setVisible(true);
    ret->setZOrder(--_zorderIndex);
    return ret;
}

void DynamicGlassPool::giveback(PixelPlane *glass)
{
    glass->setVisible(false);
    _idleGlasses.push_back(glass);
}