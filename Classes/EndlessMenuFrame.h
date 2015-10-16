// (C) Turnro.com

#ifndef __Turnroll__EndlessMenuFrame__
#define __Turnroll__EndlessMenuFrame__

#include "MenuFrame.h"
#include "MenuButton.h"

class EndlessMenuFrame: public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller);
    static EndlessMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;

protected:
    MenuButton* _btnRandom;
    static const int N_ENDLESS = 4;
    MenuButton* _btnSections[N_ENDLESS];
    MenuButton* _btnNormal1;
    std::vector<MenuButton*> _buttons;
};

#endif /* defined(__Turnroll__HelpMenuFrame__) */
