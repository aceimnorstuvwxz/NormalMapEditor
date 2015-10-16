// (C) 2015 Turnro.com

#include "SettingsMenuFrame.h"
#include "TRLocale.h"
#include "SimpleAudioEngine.h"
#include "format.h"

USING_NS_CC;

SettingsMenuFrame* SettingsMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new SettingsMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool SettingsMenuFrame::init(MenuFrameProtocal *controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0, -650, 0});

    float x_btn = -40;
    float x_lb = 95;
    float width = 180;
    float height = width*0.16;
    float y_now = 210;
    float y_step = 55;

    auto funcMusic = [this](){
        float oldVolume = TRLocale::s().getMusicVolume();
        float newVolume = 1.f;
        if (oldVolume > 0.9) {
            newVolume = 0.75;
        } else if (oldVolume > 0.7) {
            newVolume = 0.5;
        } else if (oldVolume > 0.45) {
            newVolume = 0.25;
        } else if (oldVolume > 0.2) {
            newVolume = 0.0;
        } else if (oldVolume < 0.1) {
            newVolume = 1.0;
        }

        TRLocale::s().setMusicVolume(newVolume);
        TRLocale::s().applyVolumeSettings();
        resetText();

//        if (TRLocale::s().isMusicEnable())
//            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(fmt::sprintf("sounds/ambience/menu.mp3", static_cast<int>(rand_0_1()*100)%10).c_str(), true);
//        else
//            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    };

    auto funcSfx = [this](){
        float oldVolume = TRLocale::s().getSfxVolume();
        float newVolume = 1.f;
        if (oldVolume > 0.9) {
            newVolume = 0.75;
        } else if (oldVolume > 0.7) {
            newVolume = 0.5;
        } else if (oldVolume > 0.45) {
            newVolume = 0.25;
        } else if (oldVolume > 0.2) {
            newVolume = 0.0;
        } else if (oldVolume < 0.1) {
            newVolume = 1.0;
        }

        TRLocale::s().setSfxVolume(newVolume);
        TRLocale::s().applyVolumeSettings();
        resetText();

        //        if (TRLocale::s().isMusicEnable())
        //            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(fmt::sprintf("sounds/ambience/menu.mp3", static_cast<int>(rand_0_1()*100)%10).c_str(), true);
        //        else
        //            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    };

    _btnMusic = MenuButton::create("sett_music", width, height, funcMusic, _controller);
    _btnMusic->setPosition3D({x_btn, y_now, 0});
    this->addChild(_btnMusic);

    _lbMusic = Label::createWithTTF("", TRLocale::s().font(), 36);
    _lbMusic->setPosition3D({x_lb, y_now, 0});
    _lbMusic->setScale(0.5);

    this->addChild(_lbMusic);
    y_now -= y_step;

    _btnSound = MenuButton::create("sett_sfx", width, height, funcSfx, _controller);
    _btnSound->setPosition3D({x_btn, y_now, 0});
    this->addChild(_btnSound);

    _lbSfx = Label::createWithTTF("", TRLocale::s().font(), FONT_SIZE);
    _lbSfx->setPosition3D({x_lb, y_now, 0});
    _lbSfx->setScale(0.5);

    this->addChild(_lbSfx);
    y_now -= y_step;

    auto anisetlan = [this](){
        TRLocale::s().shiftLanguage();
        this->resetText();
        _controller->resetText();
    };

    _btnLanguage = MenuButton::create("sett_language", width, height, anisetlan, _controller);
    _btnLanguage->setPosition3D({x_btn, y_now, 0});
    this->addChild(_btnLanguage);

    _lbLanguage = Label::createWithTTF("", TRLocale::s().font(), FONT_SIZE);
    _lbLanguage->setPosition3D({x_lb, y_now, 0});
    _lbLanguage->setScale(0.5);

    this->addChild(_lbLanguage);

    _buttons.push_back(_btnMusic);
    _buttons.push_back(_btnSound);
    _buttons.push_back(_btnLanguage);

    return true;
}

void SettingsMenuFrame::resetText()
{
    TTFConfig cfg(TRLocale::s().font().c_str(), FONT_SIZE, GlyphCollection::DYNAMIC);
    _lbMusic->setTTFConfig(cfg);
    _lbSfx->setTTFConfig(cfg);
    _lbLanguage->setTTFConfig(cfg);

    _lbMusic->setString(TRLocale::s().getMusicVolume() >= 0.1f ? fmt::sprintf("%d", static_cast<int>(TRLocale::s().getMusicVolume()*100)%101) : TRLocale::s()["sett_off"]);
    _lbSfx->setString(TRLocale::s().getSfxVolume() >= 0.1f ? fmt::sprintf("%d", static_cast<int>(TRLocale::s().getSfxVolume()*100)%101) : TRLocale::s()["sett_off"]);
    _lbLanguage->setString(TRLocale::s()["language_name"]);

    for (auto b : _buttons) {
        b->resetText();
    }
}

void SettingsMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onPressed(cursor);
    }
}
void SettingsMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onMoved(cursor);
    }
}
void SettingsMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onCanceled(cursor);
    }
}
void SettingsMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
    for (auto btn : _buttons) {
        btn->onReleased(cursor);
    }
}

std::string SettingsMenuFrame::getTitle()
{
    return "head_options";
}

void SettingsMenuFrame::onTurnOut()
{
    TRLocale::s().store();
}