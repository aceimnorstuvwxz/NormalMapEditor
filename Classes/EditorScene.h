// (C) 2015 Turnro.com

#ifndef EditorScene_hpp
#define EditorScene_hpp

#include "TRBaseScene.h"


struct EEPoint
{
    int pid;
    cocos2d::Vec2 position;
    cocos2d::Sprite* sprite;
    float height;
};

struct EELine
{
    std::shared_ptr<EEPoint> a;
    std::shared_ptr<EEPoint> b;
};

struct EETriangle
{
    std::shared_ptr<EEPoint> a;
    std::shared_ptr<EEPoint> b;
    std::shared_ptr<EEPoint> c;
    cocos2d::Vec4 color;
};


class EditorScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(EditorScene);

protected:
    cocos2d::Layer* _layer;
    cocos2d::Layer* _pointLayer;




    // key state
    bool _ks_addPoint = false;
    bool _ks_deletePoint = false;
    bool _ks_addLine = false;
    bool _ks_deleteLine = false;
    cocos2d::Vec2 help_touchPoint2editPosition(const cocos2d::Vec2& touchpoint);
    void initKeyboardMouse();


    std::vector<std::shared_ptr<EEPoint>> points;
    std::vector<std::shared_ptr<EELine>> lines;
    std::vector<std::shared_ptr<EETriangle>> triangles;

};

#endif /* EditorScene_hpp */
