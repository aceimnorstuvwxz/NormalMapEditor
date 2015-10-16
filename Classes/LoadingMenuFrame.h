// (C) 2015 Turnro.com

#ifndef __Turnroll__LoadingMenuFrame__
#define __Turnroll__LoadingMenuFrame__

#include "MenuFrame.h"
#include "MenuButton.h"
#include "PixelPlane.h"

class LoadingMenuFrame: public MenuFrame
{
public:
    virtual bool init(MenuFrameProtocal* controller);
    static LoadingMenuFrame* create(MenuFrameProtocal* controller);

    virtual void onPressed(const cocos2d::Vec2& cursor) override;
    virtual void onMoved(const cocos2d::Vec2& cursor) override;
    virtual void onCanceled(const cocos2d::Vec2& cursor) override;
    virtual void onReleased(const cocos2d::Vec2& cursor) override;
    virtual void resetText() override;
    virtual std::string getTitle() override;
    virtual void onTurnIn() override;

protected:
    cocos2d::Label* _labelChapter;
    cocos2d::Label* _labelName;
    cocos2d::Label* _labelQuote;
    const float TITLE_FONT_SIZE = 42;
    const float QUOTE_FONT_SIZE = 30;
    int _sid;
    static constexpr int N = 3;
    TRDrawNode3D* _loadPoints[N];
};

#endif /* defined(__Turnroll__LoadingMenuFrame__) */
