// (C) 2015 Turnro.com

#ifndef __Turnroll__DynamicGlass4__
#define __Turnroll__DynamicGlass4__

struct AcceEffectVertexFormat
{
    cocos2d::Vec3 position;
    cocos2d::Tex2F texcoord;
};

class AcceEffectSpace: public cocos2d::Node
{
public:
    enum ATTRIBUTE_INDEX{
        VERTEX_ATTRIB_POSITION,
        VERTEX_ATTRIB_TEXCOORD
    };

    static AcceEffectSpace* create();

    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);


    void step(float dt);//外部调用来驱动动画。
    void config(bool onOff, const cocos2d::Vec3& speed);

    void ani2color(const cocos2d::Color4F& color, float duration=1.f);//渐变到色彩
    void setColor(const cocos2d::Color4F& color);//突变色彩

    // 设置线条显示百分比
    // 在texcoord的.x来标记(0-1)渐变，在fsh中，根据percent来选择性discard。
    void setPercent(float per);

protected:

    virtual bool init();

    AcceEffectSpace();
    virtual ~AcceEffectSpace();

    void prepare(); //初始化shared vao/vbo...
    GLuint _vao;
    GLuint _vbo;
    float _width = 1;
    float _height = 1;
    float _length = 5;
    float _percent = 1.f;
    const float _BONE_LENGTH = 0.5f;
    const float _BONE_THIN = 0.0005f;
    const int _N = 200;
    GLsizei _count;
    AcceEffectVertexFormat* _data;
    void fillVertexData();

    static void initShaders(); //初始化shared _programState...
    static cocos2d::GLProgramState* _programState;

    cocos2d::CustomCommand _command;
    cocos2d::Color4F _color = cocos2d::Color4F::WHITE;
    float _time = 0;
    bool _aniColoring = false;
    cocos2d::Color4F _aniStartColor;
    cocos2d::Color4F _aniTargetColor;
    float _aniColorTimed = 0;
    float _aniColorDuration = 0;
    cocos2d::Quaternion _zRandomRotateQua;
    bool _lastState = false;
};



#endif /* defined(__Turnroll__DynamicGlass__) */
