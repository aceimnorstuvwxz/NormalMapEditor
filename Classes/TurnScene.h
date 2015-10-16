// (C) 2015 Turnro.com

#ifndef __Turnroll__TurnScene__
#define __Turnroll__TurnScene__

#include "cocos2d.h"
#include "TRBaseScene.h"
#include "TRDrawNode3D.h"
#include "GameUILayer.h"
#include "SuperHero.h"

class TurnScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(TurnScene);

private:
    cocos2d::Layer* _mainLayer;
    cocos2d::Camera* _mainCamera;
    void initMainLayer();
    GameUILayer* _uiLayer;
    SuperHero _superHero;
    Maxmap _maxmap;

    void initTestViews();

    virtual void update(float dt)override;
};

#endif /* defined(__Turnroll__TurnScene__) */
