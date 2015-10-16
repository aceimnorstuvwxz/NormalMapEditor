// (C) 2015 Turnro.com

#include "HelpMenuFrame.h"
#include "uiconf.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

HelpMenuFrame* HelpMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new HelpMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool HelpMenuFrame::init(MenuFrameProtocal *controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0, -650, 0});

    float x_diff = 0;
    float width = 250;
    float height = 40;
    float y_now = 210;
    float y_step = 55;

    _btnRate = MenuButton::create("help_rate", width, height, [](){
        CocosDenshion::SimpleAudioEngine::getInstance()->iosGotoRate();
    }, _controller);
    _btnRate->setPosition3D({x_diff, y_now, 0});
    this->addChild(_btnRate);
    y_now -= y_step;

    _btnCredits = MenuButton::create("help_credits", width, height, [this](){_controller->move2frame(MENU_CREDIT);}, _controller);
    _btnCredits->setPosition3D({x_diff, y_now, 0});
    this->addChild(_btnCredits);
    y_now -= y_step;

    _buttons.push_back(_btnRate);
    _buttons.push_back(_btnCredits);

    _lbHelpInfo = Label::createWithTTF(TRLocale::s()["help_info"], uic::font_en, 36);
    _lbHelpInfo->setScale(0.5);
    this->addChild(_lbHelpInfo);
    _lbHelpInfo->setPosition3D({0,0,0});

    return true;
}

void HelpMenuFrame::resetText()
{
    TTFConfig cfg(TRLocale::s().font().c_str(), FONT_SIZE, GlyphCollection::DYNAMIC);
    _lbHelpInfo->setTTFConfig(cfg);
    _lbHelpInfo->setString(TRLocale::s()["help_info"]);
    for (auto b : _buttons) {
        b->resetText();
    }
}

void HelpMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onPressed(cursor);
    }
}
void HelpMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onMoved(cursor);
    }
}
void HelpMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onCanceled(cursor);
    }
}
void HelpMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
    for (auto btn : _buttons) {
        btn->onReleased(cursor);
    }
}

std::string HelpMenuFrame::getTitle()
{
    return "head_help";
}