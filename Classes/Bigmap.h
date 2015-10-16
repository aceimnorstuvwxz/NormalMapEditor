
#ifndef Bigmap_hpp
#define Bigmap_hpp


#include "cocos2d.h"
#include "DDConfig.h"
#include "DDMapData.h"
#include "DDProtocals.h"
//大地图的渲染

struct BigmapNodeFormat
{
    cocos2d::Vec2 position;
    cocos2d::Vec4 color;
};

class BigmapAgentsNode: public cocos2d::Node
{
public:
    CREATE_FUNC(BigmapAgentsNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;

    // 设置后，在下次绘制时，会先根据DDMapData重算vertexData，然后更新VBO，再画。
    void configAsDirty() { _dirty = true;}


protected:
    constexpr static int MAX_NUM = 10000; // 最多支持多少个元素
    void prepareVertexData();

    void refreshVertexData(); // 根据DDMapData重算vertexData内容

    GLuint _vao;
    GLuint _vbo;
    void prepareShaders();
    cocos2d::GLProgramState* _programState = nullptr;
    cocos2d::CustomCommand _command;
    BigmapNodeFormat _vertexData[MAX_NUM];
    int _count = 0;
    bool _dirty = true;
};

class BigmapCoverNode: public cocos2d::Node
{
public:
    CREATE_FUNC(BigmapCoverNode);
    virtual bool init()override;
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)override;

    // 设置后，在下次绘制时，会先根据DDMapData重算vertexData，然后更新VBO，再画。
    void configAsDirty() { _dirty = true;}


protected:
    constexpr static int MAX_NUM = (DDConfig::BIGMAP_X_EXPAND*2+1)*(DDConfig::BIGMAP_Y_EXPAND*2+1); // 最多支持多少个CUBE
    void prepareVertexData();

    void refreshVertexData(); // 根据DDMapData重算vertexData内容

    GLuint _vao;
    GLuint _vbo;
    void prepareShaders();
    cocos2d::GLProgramState* _programState = nullptr;
    cocos2d::CustomCommand _command;
    BigmapNodeFormat _vertexData[MAX_NUM];
    int _count = 0;
    bool _dirty = true;
};

class Bigmap
{
public:
    enum ZORDER{
        //先画
        Z_BASE_IMAGE,
        Z_BIGMAP_NODE,
        Z_BIGMAP_COVER,
        Z_SELECTION_IMAGE
        //后画
    };
    void init(cocos2d::Layer* bigmapLayer, cocos2d::Layer* uiLayer);
    void configProtocals(DDBattleFieldProtocal* battleProtocal);
    void step(float dt);


protected:

    DDBattleFieldProtocal* _battleProtocal;

    cocos2d::Layer* _bigmapLayer;
    BigmapAgentsNode* _agentsNode;
    BigmapCoverNode* _coverNode;
    cocos2d::Sprite* _bigmapBaseImage;
    cocos2d::Sprite* _selectionImage;

    cocos2d::Layer* _uiLayer;
    constexpr static int NUM_MESSAGE_LABEL = 8;
    cocos2d::Label* _lbMessages[NUM_MESSAGE_LABEL];
    cocos2d::Label* _lbMessagesShadow[NUM_MESSAGE_LABEL];
    float _messageYStart;
    float _messageYStep;
    int _messageHeadIndex;
    void initMessageThings();
    void message(const std::string& text, const cocos2d::Color4B& color);

    cocos2d::Sprite* _selectionIcon;
    void initTouchThings();
    MapPos _centerMinMapPos;
    MapPos _selectedMapPos;
    void help_bigmapMoveTo(const MapPos& mappos);
    void help_tryToSwitchTo(const MapPos& mappos);//尝试选择某个MinMap

};

#endif /* Bigmap_hpp */
