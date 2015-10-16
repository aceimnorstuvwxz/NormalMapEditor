// (C) 2015 Turnro.com

#include "MainMenuFrame.h"
#include "GameSharing.h"

USING_NS_CC;

MainMenuFrame* MainMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new MainMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool MainMenuFrame::init(MenuFrameProtocal* controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0,0,0});

    const float Y_SCOPE = 135;
    const float Y_UPDIFF = 50;
    const float WIDTH = 250;
    const float HEIGHT = 60;
    _btnStorys = MenuButton::create("story_mode", WIDTH, HEIGHT, [this](){
        _controller->move2frame(MENU_STORIES);
    }, _controller);
    _btnStorys->setPosition3D({0,Y_UPDIFF+Y_SCOPE,0});
    this->addChild(_btnStorys);

    _btnInfinite = MenuButton::create("infinite_mode", WIDTH, HEIGHT, [this](){
        _controller->move2frame(MENU_ENDLESS);
    }, _controller);
    _btnInfinite->setPosition3D({0,Y_UPDIFF+0,0});
    this->addChild(_btnInfinite);

    _btnRecord = MenuButton::create("records", WIDTH, HEIGHT, [](){
        GameSharing::openGameCenterLeaderboardsUI(0);
    }, _controller);
    _btnRecord->setPosition3D({0,Y_UPDIFF+-Y_SCOPE,0});
    this->addChild(_btnRecord);

    const float Y_SETTING_HELP = -250;
    const float X_SETTING_HELP = 100;
    const float WIDTH_SETTINGS_HELP = 150;
    const float HEIGHT_SETTINGS_HELP = 30;
    _btnSettings = MenuButton::create("options", WIDTH_SETTINGS_HELP, HEIGHT_SETTINGS_HELP, [this](){
        _controller->move2frame(MENU_SETTINGS);
    }, _controller);
    _btnSettings->setPosition3D({-X_SETTING_HELP, Y_SETTING_HELP ,0});
    this->addChild(_btnSettings);

    _btnHelp = MenuButton::create("help", WIDTH_SETTINGS_HELP, HEIGHT_SETTINGS_HELP, [this](){
        _controller->move2frame(MENU_HELP);
    }, _controller);
    _btnHelp->setPosition3D({X_SETTING_HELP, Y_SETTING_HELP ,0});
    this->addChild(_btnHelp);

    _buttons.push_back(_btnStorys);
    _buttons.push_back(_btnInfinite);
    _buttons.push_back(_btnRecord);
    _buttons.push_back(_btnSettings);
    _buttons.push_back(_btnHelp);

    return true;
}

void MainMenuFrame::resetText()
{
    for (auto b : _buttons) {
        b->resetText();
    }
}

void MainMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onPressed(cursor);
    }
}
void MainMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onMoved(cursor);
    }
}
void MainMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onCanceled(cursor);
    }
}
void MainMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
    for (auto btn : _buttons) {
        btn->onReleased(cursor);
    }
}

std::string MainMenuFrame::getTitle()
{
    return "head_main_menu";
}

void MainMenuFrame::onTurnIn()
{
    MenuFrame::onTurnIn();
//    static bool firstTimeSpecial = true;
//    if (firstTimeSpecial) {
//        firstTimeSpecial = false;
        for (auto b : _buttons) {
            b->setOpacity(0);
            b->runAction(FadeIn::create(0.5));
        }
//    }
}
