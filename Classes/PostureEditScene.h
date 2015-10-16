// (C) 2015 Turnro.com

#ifndef __Turnroll__DemoScene2p__
#define __Turnroll__DemoScene2p__

#include <stdio.h>
#include "TRBaseScene.h"
#include "cocos2d.h"
#include "PixelPlane.h"


class PostureEditScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(PostureEditScene);

protected:
    static constexpr int N = 7;
    const float MOVE_STEP = 0.1f;
    const float ANI_TIME = 1.f;

    cocos2d::Layer* _layer;
    cocos2d::Camera* _camera;
    PixelPlane* _aniObj;
    PixelPlane* _postureObjs[N];
    int _selectIndex =0;

    PixelPlane* genAirObj();
    PixelPlane* genRoadObj();

    virtual void update(float dt)override;
    float _shaderTime = 0;
    cocos2d::Label* _lb;
    bool _aning = false;

};

#endif /* defined(__Turnroll__DemoScene__) */
