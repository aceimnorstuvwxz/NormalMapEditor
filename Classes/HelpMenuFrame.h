// (C) Turnro.com

#ifndef __Turnroll__HelpMenuFrame__
#define __Turnroll__HelpMenuFrame__

#include "MenuFrame.h"
#include "MenuButton.h"

class HelpMenuFrame: public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller);
    static HelpMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;

protected:
    const float FONT_SIZE = 36;
    cocos2d::Label* _lbHelpInfo;
    MenuButton* _btnRate;
    MenuButton* _btnCredits;
    std::vector<MenuButton*> _buttons;
};

#endif /* defined(__Turnroll__HelpMenuFrame__) */
