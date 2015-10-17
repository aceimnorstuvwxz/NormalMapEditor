// (C) 2015 Turnro.com

#ifndef EditorScene_hpp
#define EditorScene_hpp

#include "TRBaseScene.h"
#include "DDConfig.h"
#include "BattleField.h"
#include "delaunay.h"
#include <algorithm>


struct EEPoint
{
    int pid;
    cocos2d::Vec2 position; // relative position
    cocos2d::Sprite* sprite;
    float height;
    cocos2d::Vec3 pos3d(){return {position.x, position.y, height};}
};

struct EETriangle
{
    std::shared_ptr<EEPoint> a;
    std::shared_ptr<EEPoint> b;
    std::shared_ptr<EEPoint> c;
    cocos2d::Vec4 color;
    int calcKey(){

        std::vector<int> pids = {a->pid, b->pid, c->pid};
        std::sort(pids.begin(), pids.end());
        CCLOG("%d %d %d", pids[0], pids[1], pids[2]);
        return pids[0]*1000000 + pids[1] * 1000 + pids[2];
    }
};

struct EELinesNodeVertexFormat
{
    cocos2d::Vec2 position;
};

class EELinesNode:public cocos2d::Node
{
public:
    CREATE_FUNC(EELinesNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;
    void configLines(const std::list<std::shared_ptr<EETriangle>>& triangles);

protected:
    void prepareVertexData(); //初始化VAO/VBO
    GLuint _vao; //共享的VAO
    GLuint _vbo; //共享的VBO
    void prepareShaders(); //初始化shader program
    cocos2d::GLProgramState* _programState = nullptr;//共享的PROGRAM
    cocos2d::CustomCommand _command;
    constexpr static int NUM_MAX_VERTEXS = 10000;
    EELinesNodeVertexFormat _vertexData[NUM_MAX_VERTEXS];
    int _count;
    bool _dirty = true;
};

struct EETrianglesNodeVertexFormat
{
    cocos2d::Vec2 position;
    cocos2d::Vec3 normal;
    cocos2d::Vec4 color;
};

class EETrianglesNode:public cocos2d::Node
{
public:
    enum ShowState{
        SS_PREVIEW = 0,
        SS_NORMAL_MAP = 0,
        SS_TEXTURE = 0
    };
    CREATE_FUNC(EETrianglesNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;
    void configTriangles(const std::list<std::shared_ptr<EETriangle>>& triangles);
    void configShowState(int state) { _showState = state;}
    void updateLightPos(cocos2d::Vec2 pos);

protected:
    int _showState = SS_PREVIEW;
    void prepareVertexData(); //初始化VAO/VBO
    GLuint _vao; //共享的VAO
    GLuint _vbo; //共享的VBO
    void prepareShaders(); //初始化shader program
    cocos2d::GLProgramState* _programState = nullptr;//共享的PROGRAM
    cocos2d::CustomCommand _command;
    constexpr static int NUM_MAX_VERTEXS = 10000;
    EETrianglesNodeVertexFormat _vertexData[NUM_MAX_VERTEXS];
    int _count;
    bool _dirty = true;


    cocos2d::Vec2 _lightPos;
};


class EditorScene:public TRBaseScene
{
public:
    enum Z_ORDER
    {
        Z_TRIANGLES,
        Z_LINES,
        Z_POINTS,
        Z_LIGHT,
    };
    virtual bool init() override;
    CREATE_FUNC(EditorScene);


    // touchPoint 是由touch->getLocation返回的。 editPosition是以界面中心为原点的，不过单位与touchPoint一样。
    static cocos2d::Vec2 help_touchPoint2editPosition(const cocos2d::Vec2& touchpoint);
    // 而relativePosition是[-1,1]的单位。
    static cocos2d::Vec2 help_editPosition2relativePosition(const cocos2d::Vec2& editposition);
    static cocos2d::Vec2 help_relativePosition2editPosition(const cocos2d::Vec2& relativePosition);

protected:
    cocos2d::Layer* _layer;
    cocos2d::Layer* _pointLayer;
    cocos2d::Layer* _presentingLayer;

    // key state
    bool _ks_addPoint = false;
    bool _ks_deletePoint = false;
    bool _ks_selection = false;
    bool _ks_shading = false;
    bool _ks_digging = false;

    void initKeyboardMouse();
    std::shared_ptr<EEPoint> findSelectedPoint(const cocos2d::Vec2& rawpos);
    void addPoint(const cocos2d::Vec2& rawpos);
    void selectPoint(const cocos2d::Vec2 rawpos, bool ismulti);
    void deletePoint(const cocos2d::Vec2& rawpos);

    EELinesNode* _linesNode;
    void initLinesThings();
    void refreshLines();

    // 三角形当3条边存在时，自动就出现了，初始一定的色彩，如果不需要此三角形，可以破坏三角形的结构，或者给一个全透的色彩。
    EETrianglesNode* _trianglesNode;
    void initTrianglesThings();
    void refreshTriangles();

    cocos2d::Sprite* _testLightIcon;
    cocos2d::Vec2 _lightPos;
    void addTestLights();


    std::list<std::shared_ptr<EEPoint>> _selectedPoints;
    void clearSelection();

    void moveUp(bool isup);

    del_point2d_t _delPoints[10000];
    int _delPointsCount = 0;
    void delaunay();


    cocos2d::Vec4 _diggingColor;
    cocos2d::Sprite* _diggColorPanel;
    cocos2d::Label* _diggColorLabel;

    void diggColor(cocos2d::Vec2 rawpos);
    void refreshDiggColor();
    void shadingTriangle(cocos2d::Vec2 rawpos);
    std::shared_ptr<EETriangle> findTriangle(cocos2d::Vec2 rawpos);

    cocos2d::Sprite* _rulerBg;


    // data
    constexpr static int NUM_FRAME = 64;
    constexpr static float DOT_SCALE = 0.08;

    std::unordered_map<int, std::shared_ptr<EEPoint>> _points[NUM_FRAME];
    std::unordered_map<int, cocos2d::Vec4> _triangleColorMap[NUM_FRAME];
    std::list<std::shared_ptr<EETriangle>> _triangles[NUM_FRAME];

    cocos2d::Label* _lbFrameNum;
    void switchAllDots(bool isshow);
    void refreshFrameNum();
    int _frameIndex = 0;
    void nextFrame();
    void prevFrame();
    int _copySrcIndex = 0;
    void plastFrame();

    int pidIndex[NUM_FRAME] = {0};
    int nextPid(){return (pidIndex[_frameIndex])++;}
};

#endif /* EditorScene_hpp */
