// (C) 2015 Turnro Game

#include "TRBaseScene.h"
#include "uiconf.h"
#include "format.h"

USING_NS_CC;

int TRBaseScene::_keyAi = 0;

bool TRBaseScene::init()
{
    assert(Scene::init());

    _defaultLayer = McdLayer::create();
    this->addChild(_defaultLayer);

    return true;
}

void TRBaseScene::addCommonBg()
{
    auto bg = Sprite::create("images/common_bg.jpg");
    bg->setPosition(genPos({.5f,.5f}));
    _defaultLayer->addChild(bg);
}

void TRBaseScene::addCommonBackBtn(COMMON_CALLBACK callback)
{
    assert(callback != nullptr);
    _backCallBack = callback;
    _defaultLayer->decorateButtonEx({"images/common_back.png",
        {0.1f,0.9f}, 2.f}, [&](){this->_backCallBack();});
}

ui::Button* TRBaseScene::addCommonBtn(const Vec2& pos, const std::string& text, COMMON_CALLBACK callback)
{
    assert(callback != nullptr);
    int tag = _btnAI++;
    _cbMap[tag] = callback;
    return _defaultLayer->decorateButtonEx({"images/common_btn_bg.png", pos, 1.f, text},
                                    [this,tag](){this->_cbMap[tag]();});
}

Label* TRBaseScene::addCommonLabel(const cocos2d::Vec2 &pos, const std::string &text, int size)
{
    auto lb = Label::createWithTTF(text, "fonts/stringfonts/Titillium-Light.ttf", size);
    lb->setPosition(genPos(pos));
    _defaultLayer->addChild(lb);
    return lb;
}


cocos2d::Vec2 TRBaseScene::genPos(const cocos2d::Vec2& pos)
{
    auto size = Director::getInstance()->getVisibleSize();
    return {size.width * pos.x, size.height * pos.y};
}

cocos2d::Vec3 TRBaseScene::genPos3D(const cocos2d::Vec2& pos, float z){
    auto size = Director::getInstance()->getVisibleSize();
    return {size.width * pos.x, size.height * pos.y, z};
}

std::string TRBaseScene::genKey()
{
    return fmt::sprintf("genkey%d", _keyAi++);
}