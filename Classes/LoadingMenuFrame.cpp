// (C) 2015 Turnro.com

#include "LoadingMenuFrame.h"
#include "TRStories.h"
#include "TurnScene.h"
#include "SimpleAudioEngine.h"
#include "format.h"
#include "RolkConfig.h"

USING_NS_CC;

LoadingMenuFrame* LoadingMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new LoadingMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool LoadingMenuFrame::init(MenuFrameProtocal *controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0, -650*2, 0});

    _labelChapter = Label::createWithTTF("", TRLocale::s().font(), TITLE_FONT_SIZE/2);
    _labelChapter->setScale(0.5);
    _labelChapter->setPosition3D({0, 35*8-15, 0});
    this->addChild(_labelChapter);

    _labelName = Label::createWithTTF("", TRLocale::s().font(), TITLE_FONT_SIZE);
    _labelName->setScale(0.5);
    _labelName->setPosition3D({0, 35*5, 0});
    this->addChild(_labelName);

    _labelQuote = Label::createWithTTF("", TRLocale::s().font(), QUOTE_FONT_SIZE);
    _labelQuote->setScale(0.5);
    _labelQuote->setPosition3D({0, 35*3, 0});
    this->addChild(_labelQuote);

    for (int i = 0; i < N; i++) {
        const float r = 5.f;
        const float xs = 30.f;
        Vec3 tl = {-r, r, 0};
        Vec3 tr = {r, r, 0};
        Vec3 bl = {-r, -r, 0};
        Vec3 br = {r, -r, 0};
        auto p = TRDrawNode3D::create();
        p->setTriangleColor(Color4F{1,0,1,0.5});
        p->drawTriangle(tr, tl, bl);
        p->drawTriangle(tr, bl, br);
        p->setPosition3D({N/2*(-xs) + i*xs,0,0});
        _loadPoints[i] = p;
        this->addChild(p);
    }

    return true;
}

void LoadingMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
}
void LoadingMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
}
void LoadingMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
}
void LoadingMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
}

std::string LoadingMenuFrame::getTitle()
{
    return "head_loading";
}

void LoadingMenuFrame::onTurnIn()
{
    static bool s_sfxloaded = false;
    MenuFrame::onTurnIn();

    float timestep = 0.5f;
    for (int i = 0; i < N; i++) {
        _loadPoints[i]->stopAllActions();
        _loadPoints[i]->setVisible(false);
        _loadPoints[i]->runAction(RepeatForever::create(Sequence::create( DelayTime::create(timestep),DelayTime::create(i*timestep), Show::create(), DelayTime::create((N-i)*timestep), Hide::create(), NULL)));
    }

    if (!s_sfxloaded) {
        s_sfxloaded = true;
        // Preload sounds
        // preload有bug，当第二次执行时，会播放出来。在此防止重复reload。
        scheduleOnce([](float dt){
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(EditState::s()->getBgmFile().c_str());
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(EditState::s()->getEnvSoundFile().c_str());
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_speedup.wav");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_miss.wav");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_jump.wav");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_hitglass/0.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_hitglass/1.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_hitglass/2.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_hitglass/3.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/0.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/1.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/2.mp3");

            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/3.mp3");/*
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/4.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/5.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/6.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/7.mp3");
            CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/action_land/8.mp3");*/
        }, 1.1f, "run4");
    }



    scheduleOnce([this](float dt){
        Director::getInstance()->replaceScene(TurnScene::create());
    }, 2.6f + 3.f * rand_0_1(), "run6");

    _labelName->setOpacity(0);
    _labelQuote->setOpacity(0);
    _labelChapter->setOpacity(0);
    auto ac = FadeIn::create(0.5);
    _labelName->runAction(ac->clone());
#ifdef  DESP_CHAPTER_NAME
    _labelChapter->runAction(ac->clone());
#endif
    _labelQuote->runAction(ac);
}

void LoadingMenuFrame::resetText()
{
    TTFConfig cfg_chapter(TRLocale::s().font().c_str(), TITLE_FONT_SIZE/2, GlyphCollection::DYNAMIC);
    TTFConfig cfg_title(TRLocale::s().font().c_str(), TITLE_FONT_SIZE, GlyphCollection::DYNAMIC);
    TTFConfig cfg_quote(TRLocale::s().font().c_str(), QUOTE_FONT_SIZE, GlyphCollection::DYNAMIC);
    _labelChapter->setTTFConfig(cfg_chapter);
    _labelName->setTTFConfig(cfg_title);
    _labelQuote->setTTFConfig(cfg_quote);
    _labelChapter->setString(fmt::sprintf("-- %s  --", EditState::s()->getChapterName()));
    _labelName->setString(EditState::s()->getSectionName());
    _labelQuote->setString(EditState::s()->getSectionQuote());
}