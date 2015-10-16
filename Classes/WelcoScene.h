// (C) 2015 Turnro.com

#ifndef WelcoScene_hpp
#define WelcoScene_hpp

#include "TRBaseScene.h"

class WelcoScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(WelcoScene);

protected:
    cocos2d::Layer* _layer;
    cocos2d::ui::Button* _btnNewGame;
    

};

#endif /* WelcoScene */
