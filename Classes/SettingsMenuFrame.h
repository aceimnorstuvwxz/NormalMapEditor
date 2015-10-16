// (C) 2015 Turnro.com

#ifndef __Turnroll__SettingsMenuFrame__
#define __Turnroll__SettingsMenuFrame__

#include "MenuFrame.h"
#include "MenuButton.h"

class SettingsMenuFrame: public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller);
    static SettingsMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;

    virtual void onTurnOut();

protected:
    const int FONT_SIZE = 36;
    MenuButton* _btnMusic;
    MenuButton* _btnSound;
    MenuButton* _btnLanguage;
    cocos2d::Label* _lbMusic;
    cocos2d::Label* _lbSfx;
    cocos2d::Label* _lbLanguage;
    std::vector<MenuButton*> _buttons;
};




#endif /* defined(__Turnroll__SettingsMenuFrame__) */
