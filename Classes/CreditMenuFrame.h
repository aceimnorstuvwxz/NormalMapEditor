// (C) Turnro.com

#ifndef __Turnroll__CreditMenuFrame__
#define __Turnroll__CreditMenuFrame__

#include "MenuFrame.h"
#include "MenuButton.h"

class CreditMenuFrame: public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller);
    static CreditMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;

protected:
    const float FONT_SIZE = 36;
    cocos2d::Label* _lbCredits;
};

#endif /* defined(__Turnroll__HelpMenuFrame__) */
