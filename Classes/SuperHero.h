// (C) 2015 Turnro.com

#ifndef __Turnroll__SuperHero__
#define __Turnroll__SuperHero__


#include "cocos2d.h"
#include "EditState.h"
#include "GameUILayer.h"
#include "TRDrawNode3D.h"
#include "DynamicGlass.h"
#include "DynamicGlassPool.h"
#include "RoadNode.h"
#include "PixelPlane.h"
#include "DynamicGatePool.h"
#include "AcceEffectSpace.h"
#include "LocalSpaceRotation.h"

class SuperHero
{
public:
    enum SH_STATE
    {
        SH_AIR,
        SH_ROAD
    };
    void init( cocos2d::Camera* camera, GameUIState* uistate, Maxmap* maxmap, cocos2d::Layer* mainLayer, cocos2d::Layer* uiLayer);
    float getSpeed();
    float getPercent();

    void step(float dt);

    ///testing///
    void testHitGlass() {hitInstantPower();}
    void testAddSp(float dif) {_roadSpeed += dif;};
    void testCamSp(int idx) {};
    void testDead(){ _dead = true; aniGameOverSettle();}
    void testWin(){ _win = true; aniGameWinSettle();}
    void testPause() {_paused = !_paused;}
private:
    cocos2d::Camera* _camera;
    GameUIState* _uistate;
    cocos2d::Layer* _mainLayer;
    cocos2d::Layer* _uiLayer;
    Maxmap* _maxmap;
    int _zorderIndex = std::numeric_limits<int>::max();
    const float HEIGHT = 0.77f;
    float getRoadStopAcce() { return _win ? -50.f : _roadSpeed > 30.f? -4.f:-2.f;}
    float getRoadForwardAcce() {return  _roadSpeed < 30.f ? 3.5f : _roadSpeed <45.f ? 1.f : _roadSpeed < 55.f ? 0.5f : 0.1f;}
    float getHitGlassAcce() {return  _roadSpeed < 30.f ? 1.25f : _roadSpeed < 45.f ? 0.75f : _roadSpeed < 60.f ? 0.25f : 0.05f;}
    const float SPEED_SHOW_RADIO = 2.f;
    const float GRAVITY = 0.1f;
    const float ROAD_XDIFF_SPEED = 0.001f;
    const float JUMP_SPEED_BASE = 1.f;
    const float JUMP_SPEED_FACTOR = 0.1f;
    const float JUMP_SPEED_MAX = 8.f;
    const float HEIGHT_HITDOWN_MAX = HEIGHT*0.77f;
    const float HEIGHT_HITDOWN_FACTOR = 0.1f;
    const float MULTI_GRAVITY_FACTOR = 2.0f;
    const cocos2d::Quaternion CAMERA_NORMAL_POSTURE = cocos2d::Quaternion::identity();

    int _state = SH_AIR;
    std::vector<RollNode>::iterator _roadIter;
    std::vector<RollNode>::iterator _roadIterNext;
    float _roadLength = 0.f;
    float _roadSpeed = 0.f;
    float _roadOffset = 0.f; // 在所在格的跑动距离
    cocos2d::Vec3 _roadDir = {0,0,0};
    float _roadXdiff = 0.f; //[-1,1] 落地偏离
    bool _isLastTouch = false;
    cocos2d::Vec3 _airSpeed; // 空中自由速度
    cocos2d::Quaternion _posture; // 身体的姿势
    cocos2d::Vec3 _position; // 脚跟位置
    cocos2d::Vec3 _lastPosition;
    cocos2d::Vec3 _headPosition; // 头的位置，摄像头位置
    cocos2d::Quaternion _cameraRelativePosture; // 镜头相对身体的姿势
    cocos2d::Quaternion _cameraStartPosture;
    cocos2d::Quaternion _cameraTargetPosture;
    cocos2d::Quaternion _cameraCurrentPosture; // 镜头的实时姿势
    float _cameraSmoothIndex = 1.f;
    const float _cameraSmoothStep = 0.05f;
    const float _cameraSmoothStepAir = _cameraSmoothStep/2.f;
    void setupCameraSmoothAnimation();
    float _heightStart;
    float _heightTarget;
    float _heightAniIndex = 2.f; // [0-1]时，从当前到target，[1-2.0]回从target返回到默认身高。
    float _heightCurrent;
    void setupHeightAnimation(float targetHeight);

    void setPosture(const cocos2d::Quaternion& newPosture);
    void setCameraRelativePosture(const cocos2d::Quaternion& newPosture);

    // 跑路的微动
    float _cameraMicroMoveIndex = 0.f;
    float _cameraMicroMoveDirection = 1.f;
    const float _cameraMicroMoveBaseStep = 1.f/5.0f;

    cocos2d::Vec3 _cameraMicroMoveAniTarget = {0,0,0};
    const cocos2d::Vec3 _cameraMicroMoveAniUniform = {0,0,0};
    cocos2d::Quaternion _cameraMicroMovePosture;

    // 动态road的控制
    int _minmapIndex;
    bool _isRoadIterInNextMinimap = false;
    PixelPlane* _drawNodeRoadCurrent;
    PixelPlane* _drawNodeRoadNext;
    PixelPlane* _drawNodeRoadPrev;
    RoadNode* _drawNodeStaticGlassCurrent;
    RoadNode* _drawNodeStaticGlassNext;
    RoadNode* _drawNodeStaticGlassPrev;
    void initRoad();
    std::pair<PixelPlane*, RoadNode*> buildRoad(const std::unique_ptr<RollMinMap>& minmap, const RollNode& lastNodeOfLastMinmap, bool hasLastNode, std::vector<PixelPlane*>& glassHolder, std::vector<PixelPlane*>& gateHolder, bool afterEnd); //返回<Road,Glass>
    void setMinmapIndex(int index);

    // DynamicGlass
    DynamicGlassPool _dynamicGlassPool;
    std::vector<PixelPlane*> _dynamicGlassesPrev;
    std::vector<PixelPlane*> _dynamicGlassesCurrent;
    std::vector<PixelPlane*> _dynamicGlassesNext;
    void heroDynamicGlassHitCheckAndLive(float dt);

    // Assets
    // Float/Sky/Abyss
    std::tuple<PixelPlane*, PixelPlane*, PixelPlane*> buildAsset(const std::unique_ptr<RollMinMap>& minmap);
    std::tuple<PixelPlane*, PixelPlane*, PixelPlane*> _assetNodePrev;
    std::tuple<PixelPlane*, PixelPlane*, PixelPlane*> _assetNodeCurrent;
    std::tuple<PixelPlane*, PixelPlane*, PixelPlane*> _assetNodeNext;


    // skybox
    void initSkybox();
    PixelPlane* _pixelPlaneSky;
    PixelPlane* _pixelPlaneLand;

    // gate星门
    DynamicGatePool _dynamicGatePool;
    std::vector<PixelPlane*> _dynamicGatesPrev;
    std::vector<PixelPlane*> _dynamicGatesCurrent;
    std::vector<PixelPlane*> _dynamicGatesNext;

    void actionLandRoad();
    void actionMissAir();
    void actionJumpAir();
    void actionHitGlass();
    void actionSpeedup();
    void actionGameWin();
    void actionGameOver();

    // 镜头空间
    void initCameraSpace();
    static constexpr int NHitGlass = 10;
    DynamicGlass* _cameraSpaceHitGlassLevels[NHitGlass];
    void aniHitGlass();

    AcceEffectSpace* _acceEffect;
    float _acceLeftTime = 0.f;
    void acceStep(float dt);
    void aniAcce(float time);

    // Color&Power效果
    void updateColorPowerEffect();
    cocos2d::Color4F _globalColor = cocos2d::Color4F::WHITE;
    float _globalBasePower = 0;
    float _globalInstantPower = 0;
    void hitInstantPower();
    bool _powerAning = false;
    bool _powerGrowing = true;
    void powerStep(float dt);
    int _hitGlassIndex = 0;
    int _landSoundIndex = 0;

    // UI
    cocos2d::Label* _lbSpeed;
    cocos2d::Label* _lbJindu;

    const int PerPixelPts = 250;
    const float PerPixelWidth = 8;
    int PixelPerLine;
    cocos2d::Label* _lbPts;
    cocos2d::Label* _lbPtsHistory;
    int _historyBestPts;
    float _maxSpeed = 0;
    float _runLength = 0;
    int _hitDots = 0;
    float _timeCost = 0;
    int calcPts();
    cocos2d::Vec2 genPos(const cocos2d::Vec2& pos);
    void initUIViews();
    void uiViewsStep(float dt);
    float _speedLbGlassEffectTimeLeft = 0;

    float _shaderTime = 0.0f;
    bool _win = false;
    bool _dead = false;
    const float AIR_TIME_DEAD = 4.f;
    float _airTime = 0.f;

    // 结算界面
    //通用结算
    bool _paused = false;
    float _aniCameraHeightPlus = 0.f;
    bool _settlementBtnEffect = false;
    cocos2d::Label* _lbChapterName;
    cocos2d::Label* _lbMapName;
    cocos2d::Label* _lbMaxSpeed;
    cocos2d::Label* _lbRunLength;
    cocos2d::Label* _lbDynamicGlass;
    cocos2d::Label* _lbTimeCost;
    cocos2d::Label* _lbPauseIcon;
    int _targetPts;
    int _ptsNow;
    cocos2d::Label* _lbTotalScore;

    cocos2d::Sprite* _spWhiteShadow;
    cocos2d::Label* _lbBtnFirst;
    cocos2d::Label* _lbBtnSecond;
    TRDrawNode3D* _rectScore;
    TRDrawNode3D* _rectBtnFirst;
    TRDrawNode3D* _rectBtnSecond;
    std::vector<cocos2d::Node*> _settleMentViews;
    void initSettlementViews();
    void aniSettlements(bool iswin);//显示通用结算
    void aniGameWinSettle();//胜利时，开始结算
    void aniGameOverSettle();//失败时，开始结算
    void aniGameWinSettleAst();
    void clickBtnFirst();
    void clickBtnSecond();
    bool isRectPointerIn(cocos2d::Node* rect,  float hw, float hh, const cocos2d::Vec2& pos);
    void aniPause();
    
    // 静止时镜头动画
    bool _scatCameraAniContinue = false;
    cocos2d::Vec3 _scatCameraAniStep;
    cocos2d::Vec3 _scatCameraAniCurrent;


    bool _musicEnable;
    bool _sfxEnable;
    bool _isInfinite;


};



#endif /* defined(__Turnroll__SuperHero__) */
