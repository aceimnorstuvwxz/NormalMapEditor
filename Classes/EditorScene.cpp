// (C) 2015 Turnro.com

#include "EditorScene.h"
#include "TRLocale.h"

USING_NS_CC;
bool EditorScene::init()
{
    assert(TRBaseScene::init());

    _layer = Layer::create();
    this->addChild(_layer);


    auto sp = Sprite::create("images/dungeoncraft.png");
    sp->setPosition(genPos({0.5,0.75}));
    _layer->addChild(sp);
    auto size = Director::getInstance()->getWinSize();


    return true;
}