// (C) 2015 Turnro.com

#ifndef __Turnroll__DemoScene__
#define __Turnroll__DemoScene__

#include <stdio.h>
#include "TRBaseScene.h"
#include "cocos2d.h"
#include "DynamicGlass.h"
#include "PixelPlane.h"
#include "AcceEffectSpace.h"

class GlassDemoScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(GlassDemoScene);

protected:
    static constexpr int N = 1;
    cocos2d::Layer* _layer;
    cocos2d::Camera* _camera;
    cocos2d::Sprite3D* _testSprite;
    bool _running = false;
    DynamicGlass* _glass;


    virtual void update(float dt)override;
    float _shaderTime = 0;

    // acce
    AcceEffectSpace* _acceEffect;
    float _percent = 0;

};

#endif /* defined(__Turnroll__DemoScene__) */
