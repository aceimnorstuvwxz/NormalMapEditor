// (C) 2015 Turnro Game

#include "LogoScene.h"
#include <vector>
#include "TurnScene.h"

USING_NS_CC;

Scene* LogoScene::createScene()
{
    auto scene = Scene::create();

    auto layer = LogoScene::create();

    scene->addChild(layer);

    return scene;
}
inline void scale2size(cocos2d::Sprite* sprite, const cocos2d::Size& size)
{
    sprite->setScaleX(size.width* 1.f/sprite->getContentSize().width);
    sprite->setScaleY(size.height*1.f/sprite->getContentSize().height);
}
bool LogoScene::init()
{
    if (!Layer::init()){
        return false;
    }
    Size windowsSize = Director::getInstance()->getVisibleSize();
    auto center =Vec2{windowsSize.width, windowsSize.height} * 0.5f;
    auto& imagePairs = logoImages;

    // bg
    auto spbg = Sprite::create("images/white_bg.png");
    scale2size(spbg, windowsSize);
    spbg->setPosition(center);
    this->addChild(spbg, 0);

    std::vector<Sprite*> sprites;
    sprites.reserve(imagePairs.size());
    float delaySum = 0.f;
    for (int i = 0; i < imagePairs.size(); i++) {
        auto& pair = imagePairs[i];
        CCLOG("%s %f", pair.first.c_str(), pair.second);
        auto sp = Sprite::create(pair.first);
        sp->setPosition(center);
//        scale2size(sp, windowsSize);
        sp->setOpacity(0);
        this->addChild(sp, 1);
        auto actWait = DelayTime::create(delaySum);
        auto actWait2 = DelayTime::create(pair.second*full_show);
        delaySum += pair.second;
        float fadeTime = pair.second * (1.f - full_show) * 0.5f;
        auto fadeIn = FadeIn::create(fadeTime);
        auto fadeOut = FadeOut::create(fadeTime);
        auto seq = Sequence::create(actWait, fadeIn, actWait2, fadeOut, NULL);
//        if ( i == imagePairs.size() - 1){
//            seq = Sequence::create(actWait, fadeIn, actWait2, NULL);
//        }
        sp->runAction(seq);
    }
//    scheduleOnce([&delaySum](float){Director::getInstance()->replaceScene(WelcoScene::create());}, delaySum, "editor_scene");

    return true;
}