// (C) 2015 Turnro.com

#include "WelcoScene.h"
#include "EditorScene.h"
#include "TRLocale.h"

USING_NS_CC;
bool WelcoScene::init()
{
    assert(TRBaseScene::init());

    _layer = Layer::create();
    this->addChild(_layer);


    auto sp = Sprite::create("images/dungeoncraft.png");
    sp->setPosition(genPos({0.5,0.75}));
    _layer->addChild(sp);
    auto size = Director::getInstance()->getWinSize();
//    sp->setScale(size.width/sp->getContentSize().width, size.height/sp->getContentSize().height);
//    sp->setOpacity(0);
//    sp->runAction(Sequence::create(FadeIn::create(1.0), DelayTime::create(1.0), FadeOut::create(1.0), nullptr));

    auto _battleDialogButtonNeutral = ui::Button::create("images/button_normal.png","images/button_selected.png", "images/button_disable.png");
    _battleDialogButtonNeutral->setPosition(genPos({0.5,0.45}));
    _battleDialogButtonNeutral->setTitleText("Start New Game");
    _battleDialogButtonNeutral->setTitleFontSize(30*DDConfig::fontSizeRadio());
    _battleDialogButtonNeutral->setTitleFontName(TRLocale::s().font());
    _battleDialogButtonNeutral->setScale(DDConfig::battleDialogButtonSize().width/_battleDialogButtonNeutral->getContentSize().width, DDConfig::battleDialogButtonSize().height/_battleDialogButtonNeutral->getContentSize().height);
    _battleDialogButtonNeutral->addTouchEventListener([](Ref*,ui::Widget::TouchEventType type){
        if (type != cocos2d::ui::Widget::TouchEventType::ENDED) {
            return;
        } else {
            Director::getInstance()->replaceScene(EditorScene::create());
        }

    });
    _layer->addChild(_battleDialogButtonNeutral);

    _btnNewGame = _battleDialogButtonNeutral;
    


    return true;
}