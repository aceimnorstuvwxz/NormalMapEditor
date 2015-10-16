// (C) 2015 Turnro.com

#ifndef EditorScene_hpp
#define EditorScene_hpp

#include "TRBaseScene.h"


struct EEPoint
{
    int pid;
    cocos2d::Vec2 position; // relative position
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
    enum Z_ORDER
    {
        Z_POINTS,
    };
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
    // touchPoint 是由touch->getLocation返回的。 editPosition是以界面中心为原点的，不过单位与touchPoint一样。
    cocos2d::Vec2 help_touchPoint2editPosition(const cocos2d::Vec2& touchpoint);
    // 而relativePosition是[-1,1]的单位。
    cocos2d::Vec2 help_editPosition2relativePosition(const cocos2d::Vec2& editposition);
    cocos2d::Vec2 help_relativePosition2editPosition(const cocos2d::Vec2& relativePosition);
    void initKeyboardMouse();


    std::list<std::shared_ptr<EEPoint>> _points;
    std::vector<std::shared_ptr<EELine>> _lines;
    std::vector<std::shared_ptr<EETriangle>> _triangles;
    void addPoint(const cocos2d::Vec2& rawpos);
    void deletePoint(const cocos2d::Vec2& rawpos);

};

#endif /* EditorScene_hpp */
