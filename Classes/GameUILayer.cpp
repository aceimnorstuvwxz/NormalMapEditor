// (C) 2015 Turnro.com

#include "GameUILayer.h"

USING_NS_CC;

GameUIState* GameUILayer::getState()
{
    return &_uiState;
}

bool GameUILayer::init()
{
    assert(Layer::init());
    _uniformScaler = 1.f / Director::getInstance()->getWinSize().width;
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){

        _uiState._isTouch = true;
        auto pos = touch->getLocation();
        _uiState._uniformX = pos.x * _uniformScaler;

        return true;
     };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        _uiState._uniformX = touch->getLocation().x * _uniformScaler;

    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        _uiState._isTouch = false;
    };
    listener->onTouchCancelled = [this](Touch* touch, Event* event){
        _uiState._isTouch = false;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}