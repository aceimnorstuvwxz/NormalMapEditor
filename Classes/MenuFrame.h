// (C) 2015 Turnro.com

#ifndef __Turnroll__MenuFrameNode__
#define __Turnroll__MenuFrameNode__

#include "cocos2d.h"
#include "MenuFrameProtocal.h"
#include <TRLocale.h>

// 一个menu页面的基类
class MenuFrame: public cocos2d::Sprite3D
{
public:
    virtual bool init(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) = 0;
    virtual void onMoved(const cocos2d::Vec2& cursor) = 0;
    virtual void onCanceled(const cocos2d::Vec2& cursor) = 0;
    virtual void onReleased(const cocos2d::Vec2& cursor) = 0;
    virtual void resetText() = 0;

    virtual std::string getTitle() = 0;
    virtual void onTurnIn();
    virtual void onTurnOut();

protected:
    MenuFrame(){};
    MenuFrameProtocal* _controller;

};

#endif /* defined(__Turnroll__MenuFrameNode__) */
