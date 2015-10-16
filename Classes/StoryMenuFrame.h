// (C) 2015 Turnro.com

#ifndef __Turnroll__StoryMenuFrame__
#define __Turnroll__StoryMenuFrame__

#include <vector>

#include "cocos2d.h"
#include "MenuFrame.h"
#include "MenuButton.h"
#include "MenuFrameProtocal.h"


class StoryMenuFrame:public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller) override;
    static StoryMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;
    virtual void onTurnOut() override;

protected:
    float _camDiff;
    const float CH_FONT_SIZE = 27;
    float _diffNow = 0;
    bool _moved = false;
    cocos2d::Vec3 _originPos;
    cocos2d::Vec2 _lastCursor;
    std::vector<MenuButton*> _buttons;
    std::vector<std::pair<cocos2d::Label*, std::string>> _keyOfLb;
};

#endif /* defined(__Turnroll__StoryMenuFrame__) */
