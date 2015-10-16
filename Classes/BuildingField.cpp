// (C) 2015 Turnro.com

#include "BuildingField.h"
#include "format.h"
#include "DDMapData.h"
#include "TRLocale.h"

USING_NS_CC;

void BuildingField::init(cocos2d::Layer *buildingLayer)
{
    static bool touch_moved = false;
    _buildingLayer = buildingLayer;

    _buildingImage = Sprite::create("images/template_buildings.png");
    _buildingImage->setPosition(DDConfig::buildingAreaCenter());
    auto rect = DDConfig::buildingAreaRect();
    _buildingImage->setScale(rect.size.width/_buildingImage->getContentSize().width);
    _buildingImage->setZOrder(Z_BUILDING_IMAGE);
    _buildingLayer->addChild(_buildingImage);

    _selectionIcon = Sprite::create("images/template_buildings_select_icon.png");
    _selectionIcon->setScale(rect.size.height/_selectionIcon->getContentSize().height);
    _selectionIcon->setVisible(false);
    _selectionIcon->setZOrder(Z_SELECT_ICON);
    _buildingLayer->addChild(_selectionIcon);


    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        auto point = touch->getLocation();
        auto rect = DDConfig::buildingAreaRect();
        touch_moved = false;
        return rect.containsPoint(point);
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        touch_moved = true;
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        auto rect = DDConfig::buildingAreaRect();
        if (rect.containsPoint(touch->getLocation()) && touch_moved == false) {
            //算出选中了哪一个
            auto point = touch->getLocation();
            float diffX = point.x - rect.origin.x;
            float widthStep = DDConfig::buildingAreaSelectionWidth();
            int which = diffX / widthStep;
            CCLOG("buildingfiled select %d", which);
            _selectionIcon->setVisible(true);
            _selectionIcon->setPosition(rect.origin + Vec2{widthStep*which + widthStep*0.5f, rect.size.height*0.5f});
        }
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _buildingLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _buildingLayer);

}



void BuildingField::op_cancelSelection()
{
    CCLOG("building cancel selection");
}

void BuildingField::op_tellResourceChanged()
{
    CCLOG("building tell resource changed");

}