// (C) 2015 Turnro.com

#ifndef __Turnroll__DynamicGlass__
#define __Turnroll__DynamicGlass__

struct DynamicGlassVertexFormat
{
    cocos2d::Vec3 origin_pos;
    cocos2d::Quaternion posture;
    cocos2d::Vec3 target_pos;
    cocos2d::Tex2F texcoord;
    cocos2d::Vec3 center_pos;
};

class DynamicGlass: public cocos2d::Node
{
public:
    enum ATTRIBUTE_INDEX{
        VERTEX_ATTRIB_ORIGIN_POS,
        VERTEX_ATTRIB_POSTURE,
        VERTEX_ATTRIB_TARGET_POS,
        VERTEX_ATTRIB_TEXCOORD,
        VERTEX_ATTRIB_CENTER_POS
    };

    static DynamicGlass* create(float width, float height, int nWidth, int nHeight, const cocos2d::Color4F& color = cocos2d::Color4F::MAGENTA);

    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);


    void step(float dt);//外部调用来驱动动画。
    void reset();//恢复到完整玻璃状态。
    void hit(float speed);//开始打碎玻璃的动画(和音效TODO), hit的效果与当前运动的speed正相关。
    void beat();//稍微放大又复原，类似一个心跳。
    void ani2color(const cocos2d::Color4F& color, float duration=2.f);//渐变到色彩
    void setColor(const cocos2d::Color4F& color);//突变色彩
    bool checkHit(const cocos2d::Vec3& pointA, const cocos2d::Vec3& pointB, float speed);//线段与glass的碰撞，与_hitted配合，当碰到时会触发hit()。


protected:

    virtual bool init(float width, float height, int nWidth, int nHeight, const cocos2d::Color4F& color);

    DynamicGlass();
    virtual ~DynamicGlass();

    void prepare(); //初始化shared vao/vbo...
    GLuint _vao;
    GLuint _vbo;
    float _width;
    float _height;
    GLsizei _wNumber;
    GLsizei _hNumber;
    GLsizei _count;
    DynamicGlassVertexFormat* _data;
    void fillVertexData();

    static void initShaders(); //初始化shared _programState...
    static cocos2d::GLProgramState* _programState;
    static cocos2d::Texture2D* _textureNormal;
    static cocos2d::Texture2D* _textureShadow;


    cocos2d::BlendFunc _blendFunc;

    cocos2d::CustomCommand _command;
    cocos2d::Color4F _color;
    cocos2d::Color4F _lightColor;
    cocos2d::Vec3 _lightDirection;
    float _speed; // 碰撞的速度，与碎片效果的距离正相关。
    float _factor; // [0-1] 动作节点

    bool _hitted = false;
    bool _hiting = false;
    bool _aniColoring = false;
    cocos2d::Color4F _aniStartColor;
    cocos2d::Color4F _aniTargetColor;
    float _aniColorTimed = 0;
    float _aniColorDuration = 0;

};



#endif /* defined(__Turnroll__DynamicGlass__) */
