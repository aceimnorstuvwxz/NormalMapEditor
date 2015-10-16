// (C) 2015 Turnro.com

#include "PropertyField.h"
#include "format.h"
#include "DDMapData.h"
#include "TRLocale.h"

USING_NS_CC;

void PropertyField::init(cocos2d::Layer *propertyLayer)
{
    _propertyLayer = propertyLayer;

    _headImage = Sprite::create("images/property/idle_head.png");
    _headImage->setPosition(DDConfig::relativePos(0.13, 0.13));
    _headImage->setScale(DDConfig::relativeScaler(1.f));
    _headImage->setZOrder(Z_HEAD_IMAGE);
    _propertyLayer->addChild(_headImage);

    _titleLabel = Label::createWithTTF("Idle title", TRLocale::s().font(), 30*DDConfig::fontSizeRadio());
    _titleLabel->setPosition(DDConfig::relativePos(0.5, 0.12));
    _titleLabel->setWidth(DDConfig::relativeScaler(1.f)*300);
    _titleLabel->setAlignment(TextHAlignment::LEFT);
    _titleLabel->setZOrder(Z_TEXT);
    _propertyLayer->addChild(_titleLabel);

    _describeLabel = Label::createWithTTF("Describe text string...", TRLocale::s().font(), 25*DDConfig::fontSizeRadio());
    _describeLabel->setPosition(DDConfig::relativePos(0.5, 0.095));
    _describeLabel->setWidth(DDConfig::relativeScaler(1.f)*300);
    _describeLabel->setAlignment(TextHAlignment::LEFT);
    _describeLabel->setZOrder(Z_TEXT);
    _propertyLayer->addChild(_describeLabel);

    float x_now = 0.31;
    float x_step = 0.12;
    float y_image = 0.045;
    float y_num_diff = -0.012;
    float y_cost_diff = -0.028;
    float x_cost_diff = 0.005;
    float x_text_width = 60*DDConfig::relativeScaler(1.0);

    auto genPropertyNode = [this, y_num_diff, y_cost_diff,x_cost_diff, x_text_width](float x, float y){
        PropertyNode ret;
        auto image = Sprite::create("images/property/idle_property.png");
        image->setPosition(DDConfig::relativePos(x, y));
        image->setScale(DDConfig::relativeScaler(0.7f));
        image->setZOrder(Z_PROPERTY_IMAGE);
        _propertyLayer->addChild(image);
        ret.image = image;

        auto num = Label::createWithTTF("2/10", TRLocale::s().font(), 18*DDConfig::fontSizeRadio());
        num->setPosition(DDConfig::relativePos(x, y + y_num_diff));
        num->setZOrder(Z_PROPERTY_NUM);
        num->setWidth(x_text_width);
        num->setAlignment(TextHAlignment::RIGHT);
        _propertyLayer->addChild(num);
        ret.numLabel = num;


        auto cost = Label::createWithTTF("200", TRLocale::s().font(), 18*DDConfig::fontSizeRadio());
        cost->setPosition(DDConfig::relativePos(x+x_cost_diff, y + y_cost_diff));
        cost->setZOrder(Z_PROPERTY_COST);
        cost->setWidth(x_text_width);
        cost->setAlignment(TextHAlignment::CENTER);
        _propertyLayer->addChild(cost);
        ret.costLabel = cost;
        return  ret;
    };

    // 下方基本图标序列
    for (int i = 0; i < NUM_PROPERTY_MAX; i++) {
        _propertyNodes[i] = genPropertyNode(x_now, y_image);
        x_now += x_step;
    }

    // 五行属性图标
    _elementTypeIcon = genPropertyNode(x_now, 0.105);

    // 移除售卖
    _removeIcon = genPropertyNode(x_now, y_image);

/*
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
     */

}