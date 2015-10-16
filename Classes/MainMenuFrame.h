// (C) 2015 Turnro.com

#ifndef __Turnroll__MainMenuFrame__
#define __Turnroll__MainMenuFrame__

#include "cocos2d.h"
#include "MenuFrame.h"
#include "MenuButton.h"
#include "MenuFrameProtocal.h"
#include <vector>

class MainMenuFrame:public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller);
    static MainMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;
    virtual void onTurnIn() override;
protected:
    MenuButton* _btnStorys;
    MenuButton* _btnInfinite;
    MenuButton* _btnRecord;
    MenuButton* _btnSettings;
    MenuButton* _btnHelp;
    std::vector<MenuButton*> _buttons;
};

#endif /* defined(__Turnroll__MainMenuFrame__) */
