// (C) 2015 Turnro.com

#ifndef __Turnroll__GameUILayer__
#define __Turnroll__GameUILayer__

#include "cocos2d.h"

struct GameUIState
{
    bool _isTouch = false;
    float _uniformX = 0;
};

class GameUILayer:public cocos2d::Layer
{
public:
    virtual bool init()override;
    CREATE_FUNC(GameUILayer);
    GameUIState* getState();

private:
    float _uniformScaler;
    GameUIState _uiState;
};


#endif /* defined(__Turnroll__GameUILayer__) */
