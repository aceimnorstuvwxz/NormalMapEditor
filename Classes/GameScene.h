// (C) 2015 Turnro.com

#ifndef GameScene_hpp
#define GameScene_hpp


#include "cocos2d.h"
#include "TRBaseScene.h"
#include <unordered_map>
#include "DDMapData.h"
#include "BattleField.h"
#include "Bigmap.h"
#include "BuildingField.h"
#include "PropertyField.h"

class GameScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(GameScene);
    enum Z_ORDER{
        // 对general layer的先后定义。
        //先

        Z_BIGMAP_MESSAGE_SHADOW,
        Z_BIGMAP_MESSAGE_ABOVE,

        //后
    };
    enum Z_ORDER_LAYERS{
        // 先
        LZ_BIGMAP,
        LZ_BATTLE,
        LZ_BUILDING,
        LZ_PROPERTY,
        LZ_GENERAL
        // 后
    };

protected:
    cocos2d::Layer* _generalLayer;
    void initBaseUI();

    cocos2d::Layer* _bigmapLayer;
    Bigmap _bigmap;
    void initBigmap();


    cocos2d::Layer* _battleLayer;
    BattleField _battleField;
    void initBattleField();

    cocos2d::Sprite* _battleDialogBackgorund;
    cocos2d::Label* _battleDialogMessageLabel;
//    cocos2d::ui::Button* _battleDialogButtonPositive;
//    cocos2d::ui::Button* _battleDialogButtonNegative;
    cocos2d::ui::Button* _battleDialogButtonNeutral;
    bool _battleIsDialogShowing = false;
    void battleButtonNeutralCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
    std::function<void(void)> _battleDialogNeutralFunc;
    void battleDialog(const std::string& message, const std::string& neutralText, std::function<void(void)> func);



    cocos2d::Layer* _buildingLayer;
    BuildingField _buildingField;
    void initBuildingField();

    cocos2d::Layer* _propertyLayer;
    PropertyField _propertyField;
    void initPropertyField();


    virtual void update(float dt)override;

};


#endif /* GameScene_hpp */
