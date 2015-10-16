// (C) 2015 Turnro.com

#ifndef BuildingField_hpp
#define BuildingField_hpp

#include "cocos2d.h"
#include "DDConfig.h"
#include "DDAgent.h"
#include "DDProtocals.h"

// 7个建筑的区域
class BuildingField:public DDBuildingFieldProtocal
{
public:
    enum ZORDER{
        //先画
        Z_BUILDING_IMAGE,
        Z_SELECT_ICON
        //后画
    };
    void init(cocos2d::Layer* buildingLayer);

    virtual void op_cancelSelection() override; // 在其它地方点击后，通知取消BuildingField区域的选择框！
    virtual void op_tellResourceChanged() override; // 在资源变化时，告知BuildingField可能需要改变各建筑的是否够资源修建性！


protected:
    cocos2d::Layer* _buildingLayer;
    cocos2d::Sprite* _buildingImage;
    cocos2d::Sprite* _selectionIcon;

};

#endif /* BuildingField_hpp */
