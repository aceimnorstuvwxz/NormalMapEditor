// (C) 2015 Turnro.com

#include "GlassDemoScene.h"
#include "PolyPlaneSprite.h"
#include "EditState.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;


bool GlassDemoScene::init()
{
    assert(TRBaseScene::init());
    _layer = Layer::create();

    this->addChild(_layer);

    auto size = Director::getInstance()->getVisibleSize();

    auto _formCamera = Camera::createPerspective(60, size.width/size.height, 0.01, 1000);
    _formCamera->setPosition3D({0,0,0});
    _formCamera->lookAt(Vec3{0,0,-20},Vec3{0,1,0});
    _formCamera->setCameraFlag(CameraFlag::USER1);
    _layer->addChild(_formCamera);
    _formCamera->setCameraMask((unsigned short)CameraFlag::USER1);
    _camera = _formCamera;

    _glass = DynamicGlass::create(1.2,1.2,10,10);
    _glass->setCameraMask(_camera->getCameraMask());
    _layer->addChild(_glass);
    _glass->setPosition3D({0,0,0});





    this->addCommonBtn({0.1f,0.95f}, "back", [](){
        Director::getInstance()->popScene();
    });
    this->addCommonBtn({0.1f,0.9f}, "hit", [this](){
        _glass->hit(1.0);
    });

    this->addCommonBtn({0.1f,0.85f}, "reset", [this](){
        _glass->reset();
    });

    ///acce
    _acceEffect = AcceEffectSpace::create();
    _acceEffect->setCameraMask(_camera->getCameraMask());
    _acceEffect->setPosition3D({0,0,0});
    _layer->addChild(_acceEffect);

    this->addCommonBtn({0.9f, 0.8f}, "per +", [this](){_percent += 0.1;});
    this->addCommonBtn({0.9f, 0.75f}, "per -", [this](){_percent -= 0.1;});

    scheduleUpdate();

    return  true;
}


void GlassDemoScene::update(float dt)
{
    _shaderTime += dt;
    _glass->step(dt);
    _acceEffect->step(dt);
    _acceEffect->setPercent(_percent);
}