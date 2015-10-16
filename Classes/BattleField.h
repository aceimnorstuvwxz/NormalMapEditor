// (C) 2015 Turnro.com

#ifndef BattleField_hpp
#define BattleField_hpp

#include "cocos2d.h"
#include "DDConfig.h"
#include "DDAgent.h"
#include "DDProtocals.h"


class LightNode;

// DistanceBox用来反应action distance，机能范围！
struct DistanceBoxVertexPormat
{
    cocos2d::Vec2 position;
    cocos2d::Vec4 edge; // 上，顺时针
    cocos2d::Vec4 corner; //上右角，顺时针
};

class DistanceBox:public cocos2d::Node
{
public:
    CREATE_FUNC(DistanceBox);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;

    void configDistance(int distance);
    void configCenter(const AgentPos& center, bool isgood);
    void show();
    void dismiss();
    void step(float dt);//真实时间，用来驱动动画。

protected:
    void prepareVertexData(); //初始化VAO/VBO
    GLuint _vao; //共享的VAO
    GLuint _vbo; //共享的VBO
    void prepareShaders(); //初始化shader program
    cocos2d::GLProgramState* _programState = nullptr;//共享的PROGRAM
    cocos2d::CustomCommand _command;
    constexpr static int NUM_MAX_VERTEXS = DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM;
    DistanceBoxVertexPormat _vertexData[NUM_MAX_VERTEXS];
    int _count;
    const cocos2d::Vec4 COLOR_GOOD = {152/255.0, 245/255.0, 255/255.0, 1.0};
    const cocos2d::Vec4 COLOR_BAD = {255/255.0, 38/255.0, 38/255.0, 1.0};

    cocos2d::Vec4 _color;
    bool _dirty = true;
    int _distance = -1;
};
// 红绿层，用来表示那些区域可以放入新建筑
struct RedGreenCoverVertexFormat
{
    cocos2d::Vec2 position;
    float state; // 0 不显示， 1 绿 2 红
};

class RedGreenCover:public cocos2d::Node
{
public:
    enum COVER_STATE
    {
        CS_NONE = 0,
        CS_GREEN = 1,
        CS_RED = 2,
    };
    CREATE_FUNC(RedGreenCover);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;

    void configCover(const AgentPos& pos, int state);
    void show();
    void dismiss();
    void step(float dt);//真实时间，用来驱动动画。

protected:
    constexpr static float ANI_STEP_PER_SEC = 1.f;
//    void setRadio(const AgentPos& pos, float radio);
//    float getRadio(const AgentPos& pos);
//    constexpr static float COVER_SCALER = 1.25f;//每个cube之间会有重叠
    void prepareVertexData(); //初始化VAO/VBO
    GLuint _vao; //共享的VAO
    GLuint _vbo; //共享的VBO
    void prepareShaders(); //初始化shader program
    cocos2d::GLProgramState* _programState = nullptr;//共享的PROGRAM
    cocos2d::CustomCommand _command;
    RedGreenCoverVertexFormat _vertexData[DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM];

    bool _dirty = true;
};


// 在视野外区域覆盖的shadow cover，这个是一张图渲染全部区域的。
// 只有一个draw call，但是当视野变动时，每次都要glBufferData传数据到显卡。

struct ShadowCoverVertexFormat
{
    cocos2d::Vec2 position;
    cocos2d::Tex2F texcoord;
    float radio;
};
class ShadowCover: public cocos2d::Node
{
public:
    CREATE_FUNC(ShadowCover);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;


    void coverAll();
    void reveal(const AgentPos& pos);
    void cover(const AgentPos& pos);
    void aniReveal(const AgentPos& pos);
    void aniCover(const AgentPos& pos);

    void step(float dt);//真实时间，用来驱动动画。

protected:
    constexpr static float ANI_STEP_PER_SEC = 1.f;
    void setRadio(const AgentPos& pos, float radio);
    float getRadio(const AgentPos& pos);
    constexpr static float COVER_SCALER = 1.25f;//每个cube之间会有重叠
    void prepareVertexData(); //初始化VAO/VBO
    GLuint _vao; //共享的VAO
    GLuint _vbo; //共享的VBO
    void prepareShaders(); //初始化shader program
    cocos2d::GLProgramState* _programState = nullptr;//共享的PROGRAM
    cocos2d::BlendFunc _blendFunc;
    cocos2d::CustomCommand _command;
    ShadowCoverVertexFormat _vertexData[DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6];

    bool _dirty = true;
    float _cubeShadowRadio[DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM];
    cocos2d::Texture2D* _textureShadow = nullptr;
    float _cubeAniDir[DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM];
};




class BattleNode:public cocos2d::Node
{
public:
    enum ATTRIB_INDEX{
        MY_VERTEX_ATTRIB_POSITION,
        MY_VERTEX_ATTRIB_TEXCOORD
    };
};

// 单元底层（BattleField背景，建筑背景）
class BattleLowNode:public BattleNode
{
public:
    CREATE_FUNC(BattleLowNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;
    void setFieldBg(const std::string& fieldBgFile);
    void setBuildingBg(const std::string& buildingBgFile);


protected:
    static void prepareVertexData(); //初始化VAO/VBO
    static GLuint _vao; //共享的VAO
    static GLuint _vbo; //共享的VBO
    static void prepareShaders(); //初始化shader program
    static cocos2d::GLProgramState* _programState;//共享的PROGRAM
    cocos2d::BlendFunc _blendFunc;
    cocos2d::CustomCommand _command;
    cocos2d::Texture2D* _textureFieldBg = nullptr;
    cocos2d::Texture2D* _textureBuildingBg = nullptr;
};


// 单元中层 （扩展的，阴影，机能范围线，建筑可放性，选中光环，BOSS光环，低血光环）
class BattleMiddleNode:public BattleNode
{
public:
    CREATE_FUNC(BattleMiddleNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;

    void configShadowFile(const std::string& shadowFile);
    void configHeight(float height);
    void configScopeLine(const cocos2d::Vec4& opacitys);//top/right/bottom/left
    void configPlacement(float opacity, bool isOk);
    void configSelection(float opacity);
    void configBoss(float opacity);
    void configLowBlood(float opacity);
    void updateLights(LightNode* lights);

protected:

    static void prepareVertexData(); //初始化VAO/VBO
    static GLuint _vao; //共享的VAO
    static GLuint _vbo; //共享的VBO
    static void prepareShaders(); //初始化shader program
    static cocos2d::GLProgramState* _programState;//共享的PROGRAM
    cocos2d::BlendFunc _blendFunc;
    cocos2d::CustomCommand _command;
    cocos2d::Texture2D* _textureShadow = nullptr;//阴影
    cocos2d::Texture2D* _textureActionScopeLine = nullptr;//机能范围线
    cocos2d::Texture2D* _texturePlacement = nullptr;//建筑可放性（灰度）
    cocos2d::Texture2D* _textureSelection = nullptr;//选中光环
    cocos2d::Texture2D* _textureBOSS = nullptr;//BOSS光环
    cocos2d::Texture2D* _textureLowBlood = nullptr;//低血光环
    cocos2d::Vec4 _paraActionScopeLine = {0,0,0,0};//top,right,below,left
    cocos2d::Vec4 _paraPlacement = {0,0,0,0};//{r,g,b,isshow}
    cocos2d::Vec4 _paraSelectionBossLowBlood = {0,0,0,0};//{isselect,isboss,islowblood, not used}
    cocos2d::Vec4 _lights[DDConfig::NUM_LIGHT];

};

// 单元高层 （普通扩展的，上层建筑，受攻击，受治疗，扩展-方便可以延伸一些光圈等效果）
class BattleHighNode:public BattleNode
{
public:
    CREATE_FUNC(BattleHighNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;

    void configBuildingAnimation(const std::string& buildingFile);
    void configBuildingNormalMap(const std::string& buildingNormalMap);
    void configBuildingAnimationIndex(int index);
    void configBeAttacked(float opacity);
    void configBeCured(float opacity);
    void configAmbientColor(const cocos2d::Vec4& color);
    void configAnimationTexture(cocos2d::Texture2D* texture) {_textureBuildingAnimation = texture; _textureBuildingNormalMap = texture;}
    void updateLights(LightNode* lights);

protected:

    static void prepareVertexData(); //初始化VAO/VBO
    static GLuint _vao; //共享的VAO
    static GLuint _vbo; //共享的VBO
    static void prepareShaders(); //初始化shader program
    static cocos2d::GLProgramState* _programState;//共享的PROGRAM
    cocos2d::BlendFunc _blendFunc;
    cocos2d::CustomCommand _command;
    
    cocos2d::Texture2D* _textureBuildingAnimation = nullptr;//以帧动画形式出现的上层建筑
    cocos2d::Texture2D* _textureBuildingNormalMap = nullptr;//上层建筑的法向贴图，法向贴图永远只有一个，并不具有动画。
    cocos2d::Texture2D* _textureBeAttacked = nullptr;//被攻击贴图
    cocos2d::Texture2D* _textureBeCured = nullptr;//被治疗贴图

    int _buildingAnimationIndex = 0;//建筑的帧动画索引
    cocos2d::Vec4 _paraAmbientColor = {1,1,1,0.5};
    cocos2d::Vec4 _paraAttackedCured = {0,0,0,0};
    cocos2d::Vec4 _lights[DDConfig::NUM_LIGHT];// TODO 这个数据变成全局的，而不是对每个node的，因为都一样!
    cocos2d::Vec4 _lightsColor[DDConfig::NUM_LIGHT];//光源颜色
};

// 光源
class LightNode
{
public:
    float quality = 0;//以battle cube为单位的距离表示强度
    cocos2d::Vec2 agentFloatPos = {0,0};//以battleField中心为原点，以battle cube为单位的位置。传给shader时，需要计算从光源到对方点的相对坐标！
    float height;//以battle cube为单位的高度  降低复杂度而不考虑高度，所有光源在一个统一的高度。为了更一致的阴影计算，考虑高度，高度将与quality混合传输。
    cocos2d::Vec4 color = {1,1,1,1};//颜色
    // 因为const mediump int gl_MaxFragmentUniformVectors    >= 16;
    // 所以要节约uniform，一个vec4对应一个光源Vec4{pos.x, pos.y, qualityHeight, encodedColor}
    // qualityHeight, xxx.yyy，xxx表示xx.x的quantity .yyy表示yy.y的高度。
    // 暂时设成10个，GLSL内用uniform array!!
};

class LightNodeManager
{
public:
    void init();
    LightNode* getLights();
    LightNode* brrow();
    void giveback(LightNode* node);

protected:
    LightNode _lights[DDConfig::NUM_LIGHT];
    std::vector<LightNode*> _lightSet;
};

class AgentNode
{
public:
    void init(cocos2d::Layer* battleLayer);
//    void config(const std::string& buildingFile);
    void step(float dt);
    void configAgentFloatPos(const cocos2d::Vec2& pos); //会实际改变Node的位置
    void configAgentPos(const AgentPos& pos); //并不会实际改变位置
    void configAnimation(cocos2d::Texture2D* texture);
    void configAnimationIndex(int index);


    void updateLights(LightNode* lights);
    void clear();//清除所有
    void build(int aid, int agentType, int agentLevel, bool boss, bool lowBlood);
    int aid;
    int agentType;
    int agentLevel;
    bool boss;
    bool lowBlood;
    void configVisible(bool enbale) {
        _lowNode->setVisible(enbale);
        _middleNode->setVisible(enbale);
        _highNode->setVisible(enbale);
    }


    BattleHighNode* _highNode;
    BattleMiddleNode* _middleNode;
    BattleLowNode* _lowNode;
protected:
    cocos2d::Layer* _battleLayer;
    AgentPos _agentPos;
    cocos2d::Vec2 _agentFloatPos;
    float _agentHeight;
};


class BattleField:public DDBattleFieldProtocal
{
public:
    enum ZORDER{
        //先画
        Z_TEST_BG,
        Z_LOW_NODE,
        Z_REDGREEN_COVER,
        Z_MIDDLE_NODE,
        Z_HIGH_NODE,
        Z_SHADOW_COVER,
        Z_DISTANCE_BOX,
        Z_TEST_LIGHT,
        Z_MOVING_COVER,
        Z_MESSAGE_BACKGROUND,
        Z_MESSAGE_LABEL,

        //后画
    };
    void init(cocos2d::Layer* battleLayer);
    void step(float dt);


    //转场到新的地图，有过场动画。
    virtual void op_switch2minmap() override;// bigmap 来通知battle field切换minmap!

    virtual void op_mapdata_message(DDBattleObserveMessage message, int aid_src, int aid_des); // mapdata 来通知battle field各种消息！


    void configAnimationTexture(const std::string& fn);
    void configAnimationIndex(int index);
    void configShadowVisible();



    static cocos2d::Vec2 help_battleLayerPos2agentFloatPos(const cocos2d::Vec2& srcPos);
    static cocos2d::Vec2 help_agentPos2agentFloatPos(const AgentPos& agentPos);

protected:
    cocos2d::Layer* _battleLayer;
    LightNodeManager _lightManager;

    constexpr static int NUM_TEST_LIGHT = 2;
    BattleMiddleNode* _testMiddleNode;
    BattleHighNode* _testHighNode;
    cocos2d::Sprite* _testLightIcon[NUM_TEST_LIGHT];
    LightNode* _testLight[NUM_TEST_LIGHT];
    int _testMovingIndex = -1;
    void addTestLights();

    AgentNode _rawAgentNodes[DDConfig::BATTLE_NUM * DDConfig::BATTLE_NUM];
    std::unordered_map<AgentPos, AgentNode*> _agentNodeMap;
    ShadowCover* _shadowCover;

    bool _isMovingCovered = true;
    cocos2d::Sprite* _leftMovingCover[3];
    cocos2d::Sprite* _rightMovingCover[3];
    void initMovingCover();
    void movingCoverIn();
    void movingCoverOut();

    void op_switch2minmap_continue();
    void switchMinMap();//将新的选中的minmap渲染出来


    // 激活对话
    cocos2d::Sprite* _messageBackgroundImage;
    cocos2d::Label* _messageLabel;
    bool _waitingToTapping = false;
    void initTapMessgeThings();
    void showTapMessage(bool isActivable);
    void dismissTapMessage();

    // 移入新建筑
    int help_getWhichAgentTypeMoveIn(int buildingIndex); // 返回building field的序号对应的friend agent的类型。
    std::pair<bool, AgentPos> help_touchPoint2agentPos(const cocos2d::Vec2& point); // 计算触摸到的battle field内部的坐标
    void initMovingNewBuildThings();

    // 视野图
    std::unordered_map<AgentPos, bool> _visionMap; //视野地图，当有新建筑，或者建筑毁灭，或者切换到新的minmap，都需要重新计算它，然后据此设置cover。
    void calcVisionMapAndRefreshCover(); //计算视野地图，同时更新cover层。
    void calcVisionMap_ast(const AgentPos& agentpos, int visionDistance);

    // 红绿图
    // 每次要显示这个红绿图的时候会重新计算一边。
    // 如果minmap内agent移动或者生灭的时候，如果红绿图的visible的，那么也要更新。
    std::unordered_map<AgentPos, int> _redGreenMap;
    bool help_isDoorPosition(const AgentPos& agentpos);
    RedGreenCover* _redGreenCover;
    void calcRedGreenCoverAndShow();


    // Distance Box
    DistanceBox* _distanceBox;

};

#endif /* BattleField_hpp */
