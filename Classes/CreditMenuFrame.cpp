// (C) 2015 Turnro.com

#include "CreditMenuFrame.h"
#include "uiconf.h"

USING_NS_CC;

CreditMenuFrame* CreditMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new CreditMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool CreditMenuFrame::init(MenuFrameProtocal *controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0, -650*2, 0});

    _lbCredits = Label::createWithTTF(TRLocale::s()["credit_info"], uic::font_en, 36);
    _lbCredits->setAlignment(cocos2d::TextHAlignment::CENTER);
    _lbCredits->setScale(0.5);
    this->addChild(_lbCredits);
    _lbCredits->setPosition3D({0,0,0});

    return true;
}

void CreditMenuFrame::resetText()
{
    TTFConfig cfg(TRLocale::s().font().c_str(), FONT_SIZE, GlyphCollection::DYNAMIC);
    _lbCredits->setTTFConfig(cfg);
    _lbCredits->setString(TRLocale::s()["credit_info"]);
}

void CreditMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
}
void CreditMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
}
void CreditMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
}
void CreditMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
}

std::string CreditMenuFrame::getTitle()
{
    return "head_credit";
}