// (C) Turnro.com

#ifndef __Turnroll__WelcoSceneSec__
#define __Turnroll__WelcoSceneSec__

#include "cocos2d.h"
#include "TRBaseScene.h"
#include "TRDrawNode3D.h"

class WelcoSceneSec:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(WelcoSceneSec);

protected:
    static const int NUM_FRAMES = 10;
    cocos2d::Layer* _layer;
    cocos2d::Camera* _camera;
    TRDrawNode3D* _drawNode[NUM_FRAMES];
    cocos2d::Label* _lbTitle;
    cocos2d::Label* _lbPresent;
    cocos2d::Label* _lbInc;
    bool _animationDone = false;
};



#endif /* defined(__Turnroll__WelcoScene__) */
