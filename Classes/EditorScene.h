// (C) 2015 Turnro.com

#ifndef EditorScene_hpp
#define EditorScene_hpp

#include "TRBaseScene.h"
#include "DDConfig.h"
#include "BattleField.h"


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
    void configLines(const std::list<std::shared_ptr<EELine>>& lines);

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
    void updateLights(LightNode* lights);

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


    cocos2d::Vec4 _lights[DDConfig::NUM_LIGHT];// TODO 这个数据变成全局的，而不是对每个node的，因为都一样!
    cocos2d::Vec4 _lightsColor[DDConfig::NUM_LIGHT];//光源颜色
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
    static std::shared_ptr<EEPoint> help_checkTwoLineConnection(const std::shared_ptr<EELine>& a, const std::shared_ptr<EELine>& b);

protected:
    cocos2d::Layer* _layer;
    cocos2d::Layer* _pointLayer;




    // key state
    bool _ks_addPoint = false;
    bool _ks_deletePoint = false;
    bool _ks_addLine = false;
    bool _ks_deleteLine = false;

    void initKeyboardMouse();

    std::list<std::shared_ptr<EEPoint>> _points;
    std::list<std::shared_ptr<EELine>> _lines;
    std::list<std::shared_ptr<EETriangle>> _triangles;
    void addPoint(const cocos2d::Vec2& rawpos);
    void deletePoint(const cocos2d::Vec2& rawpos);

    std::shared_ptr<EEPoint> _firstSelectedPoint = nullptr;
    std::shared_ptr<EEPoint> findSelectedPoint(const cocos2d::Vec2& rawpos);
    void addLine(const cocos2d::Vec2& rawpos);
    void deleteLine(const cocos2d::Vec2& rawpos);
    void deleteLineWithPoint(const std::shared_ptr<EEPoint>& point);
    EELinesNode* _linesNode;
    void initLinesThings();
    void refreshLines();

    // 三角形当3条边存在时，自动就出现了，初始一定的色彩，如果不需要此三角形，可以破坏三角形的结构，或者给一个全透的色彩。
    EETrianglesNode* _trianglesNode;
    void initTrianglesThings();
    void refreshTriangles();

    constexpr static int NUM_TEST_LIGHT = 1;
    cocos2d::Sprite* _testLightIcon[NUM_TEST_LIGHT];
    LightNode* _testLight[NUM_TEST_LIGHT];
    int _testMovingIndex = 0;
    LightNodeManager _lightManager;
    void addTestLights();



};

#endif /* EditorScene_hpp */
