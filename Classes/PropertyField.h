// (C) 2015 Turnro.com

#ifndef PropertyField_hpp
#define PropertyField_hpp

#include "cocos2d.h"
#include "DDConfig.h"
#include "DDAgent.h"
#include "DDProtocals.h"
#include "DDUpgradeLoad.h"

struct PropertyNode
{
    cocos2d::Sprite* image;
    cocos2d::Label* numLabel;
    cocos2d::Label* costLabel;
};

inline std::string properType2string(PropertyType type)
{
    std::string ret;
    switch (type) {
        case PropertyType::ATTACK:
            ret = "ATTACK";
            break;
        case PropertyType::CURE:
            ret = "CURE";
            break;
        case PropertyType::CURE_NUM:
            ret = "CURE_NUM";
            break;
        case PropertyType::MINE_SPEED:
            ret = "MINE_SPEED";
            break;
        case PropertyType::ACTION_DISTANCE:
            ret = "ACTION_DISTANCE";
            break;
        case PropertyType::BLOOD_MAX:
            ret = "BLOOD_MAX";
            break;
        case PropertyType::MINE_AMOUNT:
            ret = "MINE_AMOUNT";
            break;
        case PropertyType::BOSS:
            ret = "BOSS";
            break;
        case PropertyType::SPUTT_DISTANCE:
            ret = "SPUTT_DISTANCE";
            break;
        case PropertyType::SPUTT_RADIO:
            ret = "SPUTT_RADIO";
            break;
        case PropertyType::ELEMENT_NONE:
            ret = "ELEMENT_NONE";
            break;
        case PropertyType::ELEMENT_METAL:
            ret = "ELEMENT_METAL";
            break;
        case PropertyType::ELEMENT_WOOD:
            ret = "ELEMENT_WOOD";
            break;
        case PropertyType::ELEMENT_WATER:
            ret = "ELEMENT_WATER";
            break;
        case PropertyType::ELEMENT_FIRE:
            ret = "ELEMENT_FIRE";
            break;
        case PropertyType::ELEMENT_EARTH:
            ret = "ELEMENT_EARTH";
            break;
        case PropertyType::REMOVE_3RD:
            ret = "REMOVE_3RD";
            break;
        case PropertyType::SELL_FRIEND:
            ret = "SELL_FRIEND";
            break;

        default:
            assert(false);
            break;
    }
    return ret;
}

// 下方头像，属性区域！！
class PropertyField:public DDPropertyFieldProtocal
{
public:
    enum ZORDER{
        //先画
        Z_HEAD_IMAGE,
        Z_TEXT,
        Z_PROPERTY_IMAGE,
        Z_PROPERTY_NUM,
        Z_PROPERTY_COST
        //后画
    };
    void init(cocos2d::Layer* propertyLayer);




protected:
    cocos2d::Layer* _propertyLayer;

    cocos2d::Sprite* _headImage; //头像
    cocos2d::Label* _titleLabel; //主字符串
    cocos2d::Label* _describeLabel; //描述字符串
    constexpr static int NUM_PROPERTY_MAX = 5;
    PropertyNode _propertyNodes[NUM_PROPERTY_MAX]; //属性们
    PropertyNode _removeIcon; //移除/出售 按钮
    PropertyNode _elementTypeIcon; //element属性按钮

};

#endif /* PropertyField_hpp */
