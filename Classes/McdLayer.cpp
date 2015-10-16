// (C) 2015 Turnro Game

#include "McdLayer.h"
#include "SimpleAudioEngine.h"
#include "uiconf.h"
#include "format.h"

USING_NS_CC;

int McdLayer::_keyAi = 0;

bool McdLayer::init()
{
    assert(Layer::init());

    return true;
}

void McdLayer::onButtonClick(int tag)
{
    if (_cbMap.find(tag) == _cbMap.end()){
        CCLOG("no callback with tag %d", tag);
    } else {
        _cbMap[tag]();
    }
}

ui::Button* McdLayer::decorateButton(const Btn_info& btnInfo , int tag)
{
    auto button = ui::Button::create(btnInfo.img, btnInfo.img_p);
    button->setPressedActionEnabled(true);
    //    button->setAnchorPoint({0,0});
    button->setPosition(genPos(btnInfo.pos));
    button->setTag(tag);
    button->addTouchEventListener(CC_CALLBACK_2(McdLayer::touchEvent, this));
    button->setScale(btnInfo.scale);
    button->setTitleText(btnInfo.text);
    button->setTitleFontName(uic::font_zh);
    button->setTitleFontSize(30);
    this->addChild(button, 1);
    return button;
}

ui::Button* McdLayer::decorateButtonEx(const Btn_info& btnInfo, BTN_CALLBACK callback){
    auto button = ui::Button::create(btnInfo.img, btnInfo.img_p);
    button->setPressedActionEnabled(true);
    button->setPosition(genPos(btnInfo.pos));
    auto mytag = _tagAI++;
    button->setTag(mytag);
    button->addTouchEventListener(CC_CALLBACK_2(McdLayer::touchEvent, this));
    button->setScale(btnInfo.scale);
    button->setTitleText(btnInfo.text);
    button->setTitleFontName(uic::font_zh);
    button->setTitleFontSize(30);
    this->addChild(button, 1);
    _cbMap[mytag] = callback;
    return button;
}

void McdLayer::touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type){

    auto button = static_cast<cocos2d::ui::Button*>(pSender);
    int tag = button->getTag();
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED) {
        return;
    }
    this->onButtonClick(tag);
}

cocos2d::Sprite* McdLayer::decorateImg(const std::string& img, const cocos2d::Vec2& pos)
{
    auto sp = Sprite::create(img);
    sp->setPosition(genPos(pos));
    this->addChild(sp, 2);
    return sp;
}

void McdLayer::withInOut(cocos2d::Node* node)
{
    _inoutNodes.push_back(node);
    node->setOpacity(0);
}

void McdLayer::turnIn(float dt)
{
    auto fadeIn = FadeIn::create(dt);
    for (auto node : _inoutNodes) {
        node->runAction(fadeIn->clone());
    }
}

void McdLayer::turnOut(float dt)
{
    auto fadeOut = FadeOut::create(dt);
    for (auto node : _inoutNodes){
        node->runAction(fadeOut->clone());
    }
}

cocos2d::Vec2 McdLayer::genPos(const cocos2d::Vec2& pos)
{
    auto size = Director::getInstance()->getVisibleSize();
    return {size.width * pos.x, size.height * pos.y};
}

cocos2d::Vec3 McdLayer::genPos3D(const cocos2d::Vec2& pos, float z){
    auto size = Director::getInstance()->getVisibleSize();
    return {size.width * pos.x, size.height * pos.y, z};
}

std::string McdLayer::genKey()
{
    return fmt::sprintf("mcdlayerkey_%d", _keyAi++);
}
