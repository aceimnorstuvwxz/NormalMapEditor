// (C) 2015 Turnro.com

#include "TurnScene.h"
#include "EditState.h"
#include "format.h"
#include "SimpleAudioEngine.h"
#include "RolkConfig.h"
#include "TRLocale.h"

#define TR_DEV

USING_NS_CC;

bool TurnScene::init()
{
    assert(TRBaseScene::init());

    EditState::s()->_needPause = false;
#ifdef  TR_DEV
    initTestViews();
#endif

    initMainLayer();

    _maxmap.load(EditState::s()->getMapFile());// 费时

    _uiLayer = GameUILayer::create();
    this->addChild(_uiLayer);

    _superHero.init(_mainCamera, _uiLayer->getState(), &_maxmap, _mainLayer, _defaultLayer);

    scheduleUpdate();
    
    return true;
}

void TurnScene::initMainLayer()
{
    auto layer = Layer::create();
    this->addChild(layer);

    auto size = Director::getInstance()->getVisibleSize();

    auto camera = Camera::createPerspective(60, size.width/size.height, 0.1, 1000000);
    camera->setPosition3D({0,0,50});
    camera->lookAt({0,0,0});
    camera->setCameraFlag(CameraFlag::USER1);
    layer->addChild(camera);
    camera->setCameraMask((unsigned short)CameraFlag::USER1);

    _mainLayer = layer;
    _mainCamera = camera;

    if (TRLocale::s().isMusicEnable()) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(EditState::s()->getBgmFile().c_str(), true);
    }
    if (TRLocale::s().isSfxEnable())
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(EditState::s()->getEnvSoundFile().c_str(),true);

}

void TurnScene::initTestViews()
{

#ifdef  __GAME_DEV__
    static int sindex = 0;
    addCommonBtn({0.1,0.35}, "RESET", [](){Director::getInstance()->replaceScene(TurnScene::create());});
    addCommonBtn({0.1,0.3}, "BACK", [](){Director::getInstance()->popScene();});

    addCommonBtn({0.1,0.4}, "hitglass", [this](){_superHero.testHitGlass();});
    addCommonBtn({0.9,0.4}, "add sp", [this](){_superHero.testAddSp(5);});
    addCommonBtn({0.9,0.35}, "minus sp", [this](){_superHero.testAddSp(-5);});
    addCommonBtn({0.9,0.2}, "camsp effs", [this](){_superHero.testCamSp(sindex++);});
    addCommonBtn({0.9,0.15}, "dead", [this](){_superHero.testDead();});
    addCommonBtn({0.9,0.1}, "win", [this](){_superHero.testWin();});
    addCommonBtn({0.9,0.05}, "pause", [this](){_superHero.testPause();});
#endif
}



void TurnScene::update(float dt)
{
    _superHero.step(dt);
}
