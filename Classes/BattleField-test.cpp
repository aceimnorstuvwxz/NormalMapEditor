// (C) 2015 Turnro.com

#include "BattleField.h"

USING_NS_CC;

void BattleField::addTestLights()
{
    for (int i = 0; i < NUM_TEST_LIGHT; i++) {
        auto sp = Sprite::create("images/test_light.png");
        sp->setScale(DDConfig::battleCubeWidth()/sp->getContentSize().width);
        sp->setPosition(help_agentPos2agentFloatPos({i-3,-2}));
        sp->setZOrder(BattleField::Z_TEST_LIGHT);
        sp->setScale(0.25);
        _battleLayer->addChild(sp);

        _testLightIcon[i] = sp;
        _testLight[i] = _lightManager.brrow();
        _testLight[i]->quality = 8;
        _testLight[i]->height = 2+i;

    }

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        bool res = false;
        for (int i = 0; i < NUM_TEST_LIGHT; i++) {
            auto point = touch->getLocation();
            auto size = _testLightIcon[i]->getContentSize();
            auto pos = _testLightIcon[i]->getParent()->getPosition()+ _testLightIcon[i]->getPosition();
            Rect rect = {pos.x - 0.5f*size.width, pos.y - 0.5f*size.height, size.width, size.height};

            if (rect.containsPoint(point)) {
                res = true;
                _testMovingIndex = i;
                break;
            }

        }
        return res;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        _testLightIcon[_testMovingIndex]->setPosition(_testLightIcon[_testMovingIndex]->getPosition() + touch->getDelta());
        _testLight[_testMovingIndex]->agentFloatPos = help_battleLayerPos2agentFloatPos(_testLightIcon[_testMovingIndex]->getPosition());
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _battleLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _battleLayer);
}