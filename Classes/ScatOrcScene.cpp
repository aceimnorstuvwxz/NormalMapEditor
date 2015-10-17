// (C) 2015 Turnro.com

#include "ScatOrcScene.h"
#include "EditorScene.h"
#include "WelcomeScene.h"

USING_NS_CC;
bool ScatOrcScene::init()
{
    assert(TRBaseScene::init());

    _layer = Layer::create();
    this->addChild(_layer);

    auto sp2 = Sprite::create("images/white.png");
    sp2->setScale(500);
    _layer->addChild(sp2);


    auto sp = Sprite::create("images/scatorc.png");
    sp->setPosition(genPos({0.5,0.5}));
    _layer->addChild(sp);
    auto size = Director::getInstance()->getWinSize();
//    sp->setScale(size.width/sp->getContentSize().width, size.height/sp->getContentSize().height);
    sp->setOpacity(0);
    sp->runAction(Sequence::create(FadeIn::create(0.1), DelayTime::create(0.1), FadeOut::create(0.1), nullptr));

    scheduleOnce([this](float dt){
        Director::getInstance()->replaceScene(WelcomeScene::create());
    }, 0.4, "gamescene open");


    return true;
}