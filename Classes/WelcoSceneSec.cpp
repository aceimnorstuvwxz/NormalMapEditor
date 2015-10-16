// (C) Turnro.com

#include "WelcoSceneSec.h"
#include "TurnScene.h"
#include "MenuScene.h"
#include "TRStories.h"
#include "SimpleAudioEngine.h"
#include "RolkConfig.h"
#include "GameSharing.h"


USING_NS_CC;

bool WelcoSceneSec::init()
{
    assert(TRBaseScene::init());

    _layer = Layer::create();

    this->addChild(_layer);

    auto size = Director::getInstance()->getVisibleSize();

    auto _camera = Camera::createPerspective(60, size.width/size.height, 0.01, 1000);
    _camera->setPosition3D({0,0,10});
    _camera->lookAt(Vec3{0,0,0},Vec3{0,1,0});
    _camera->setCameraFlag(CameraFlag::USER1);
    _layer->addChild(_camera);
    _camera->setCameraMask((unsigned short)CameraFlag::USER1);


    const float width = 1;
    const float height = 3;
    const float lineWidth = 3.f;
    const float lineWidthFactor = 0.004f;

    const float WAIT_TIME = 0.75f;

    const float TOTAL_DEGREE = 90.f;
    const float PER_INTERVAL = 0.16f;
    for (int i = 0; i < NUM_FRAMES; i++) {
        auto rec = TRDrawNode3D::create();
        Vec3 lb = {-width/2, -height/2,0};
        Vec3 lt = {-width/2, height/2,0};
        Vec3 rb = {width/2, -height/2,0};
        Vec3 rt = {width/2, height/2,0};
        rec->drawLine(lt + Vec3{0, lineWidth*lineWidthFactor, 0}, lb + Vec3{0, -lineWidth*lineWidthFactor, 0});
        rec->drawLine(lt, rt);
        rec->drawLine(rt + Vec3{0, lineWidth*lineWidthFactor, 0}, rb + Vec3{0, -lineWidth*lineWidthFactor, 0});
        rec->drawLine(rb, lb);
        _layer->addChild(rec);
        rec->setCameraMask(_camera->getCameraMask());
        rec->setPosition3D({0,0,0});

        rec->setLineWidth(lineWidth);
        rec->setRotation(i*1.0/(NUM_FRAMES-1)*TOTAL_DEGREE);
//        rec->setLineColor(Color4F{1,1,1,0.1f+i*.8f/NUM_FRAMES});
        _drawNode[i] = rec;
    }

    _lbTitle = Label::createWithTTF("Niflheistla", "fonts/stringfonts/Titillium-Light.ttf", 130);
    _lbTitle->setPosition(genPos({0.5,0.79}));
    _defaultLayer->addChild(_lbTitle);
    _lbTitle->setOpacity(0);

    _lbPresent = Label::createWithTTF("Presented by", "fonts/stringfonts/Titillium-Light.ttf", 20);
    _lbPresent->setPosition(genPos({0.5,0.23}));
    _defaultLayer->addChild(_lbPresent);
    _lbPresent->setOpacity(0);


    _lbInc = Label::createWithTTF("ScatOrc", "fonts/stringfonts/Titillium-Light.ttf", 100);
    _lbInc->setPosition(genPos({0.5,0.17}));
    _defaultLayer->addChild(_lbInc);
    _lbInc->setOpacity(0);

    scheduleOnce([this, TOTAL_DEGREE, PER_INTERVAL](float dt){
        for (int i = 0; i < NUM_FRAMES; i++) {
            _drawNode[i]->runAction(EaseSineInOut::create( Sequence::create(DelayTime::create((NUM_FRAMES-i)*PER_INTERVAL),RotateTo::create(i*PER_INTERVAL, 0.f), NULL)));
        }
    }, WAIT_TIME, "l0");

    scheduleOnce([this](float dt){
        _lbTitle->runAction(FadeIn::create(1.f));
        _lbInc->runAction(FadeIn::create(1.f));
        _lbPresent->runAction(FadeIn::create(1.f));
    }, WAIT_TIME + PER_INTERVAL * NUM_FRAMES, "k1");

    scheduleOnce([this](float dt){
        _animationDone = true;
        for (int i = 0; i < NUM_FRAMES; i++) {
            _drawNode[i]->runAction(Sequence::create(FadeIn::create(0.5f),Repeat::create(Blink::create(2.f, 2), kRepeatForever), NULL));
        }

    }, WAIT_TIME + PER_INTERVAL * NUM_FRAMES + 0.5f, "k3");


#ifdef  __GAME_DEV__
    addCommonBtn({0.1,0.025}, "reset", [](){Director::getInstance()->replaceScene(WelcoSceneSec::create());});
#endif

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        CCLOG("welco click");
        if (_animationDone) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/menu_btn.mp3");
            auto fo = Sequence::create(DelayTime::create(0.2f), FadeOut::create(0.5), NULL);
            for (int i = 0; i < NUM_FRAMES; i++) {
                _drawNode[i]->stopAllActions();
                _drawNode[i]->setVisible(true);
                _drawNode[i]->runAction(fo->clone());
            }
            _lbInc->runAction(fo->clone());
            _lbPresent->runAction(fo->clone());
            _lbTitle->runAction(fo);
            this->scheduleOnce([](float dt){
                Director::getInstance()->replaceScene(MenuScene::create());
            }, 0.7, "k4");
        }

        return true;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
    };
    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    TRStories::s().load();
    TRStories::s().loadEndless();
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("sounds/menu_btn.mp3");

    // GC Login
    GameSharing::initGameSharing();

    return true;
}