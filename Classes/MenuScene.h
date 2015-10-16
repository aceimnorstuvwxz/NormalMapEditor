// (C) 2015 Turnro.com

#ifndef __Turnroll__MenuScene__
#define __Turnroll__MenuScene__

#include <unordered_map>

#include "cocos2d.h"
#include "TRBaseScene.h"
#include "TRDrawNode3D.h"
#include "MenuFrame.h"
#include "MenuFrameProtocal.h"
#include "MenuButton.h"
#include "DynamicGlass.h"
#include "LoadingMenuFrame.h"


class MenuScene: public TRBaseScene, MenuFrameProtocal
{
public:
    static MenuScene* create(int frameId = MENU_MAIN);
    virtual bool init(int frameId);
    virtual void move2frame(int frameid) override;
    virtual void resetText() override;
    virtual cocos2d::Camera* getMenuCamera();
    virtual void update(float dt)override;

protected:
    const float FRAME_MOVE_TIME_FACTOR = 0.0008f;
    cocos2d::Layer* _layer;
    cocos2d::Camera* _camera;

    /*
     Unified Rendering
     The rendering of 2D and 3D object has been unified, which enables Sprite, Label, Particle to be rendered in 3D space by adding them as children of Sprite3D or Billboard. You can achieve effects like blob shadow, 3D particle, Visual damage number popups.
     因此我们按建议使用Sprite3D作为组合容器。
     */
    const float FONT_SIZE = 100;

    cocos2d::Sprite3D* _cameraNode;
    cocos2d::Label* _lbCameraHeadText;
    TRDrawNode3D* _drawNodeHeadBg;
    TRDrawNode3D* _drawNodeHeadBg2;
    TRDrawNode3D* _drawNodeTailBg;
    cocos2d::Label* _lbCameraTailText;
    bool _isTailShowing = true;
    bool _isHeadShowing = true;
    cocos2d::Vec3 _backButtonTopRelativePos;
    void initCameraNode();
    void animateTail(bool isshow, float aniTime, bool totalOut = false);
    void animateHead(bool isshow, float aniTime, bool totalOut = false);
    bool isCursorInBack(const cocos2d::Vec2& cursor);
    void setBackPressed(bool ispressed);

    void moveback();

    bool _isAnyFrameSelected = false;
    int _selectedFrameId = MENU_MAX;
    std::unordered_map<int, MenuFrame*> _menuFrames;
    LoadingMenuFrame* _loadingMenuFrame;
    void initMenuFrames();
    void initTouchListener();

    cocos2d::Layer* _glassLayer;
    DynamicGlass* _glass;
    float _glassFactor = 0.5;
    bool _glassFactorAdding = true;
    bool _isMoving = false;
    bool _firstTimeSpecial = true;

};

#endif /* defined(__Turnroll__MenuScene__) */
