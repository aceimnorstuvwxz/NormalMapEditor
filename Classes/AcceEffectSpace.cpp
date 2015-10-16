// (C) 2015 Turnro.com

#include "AcceEffectSpace.h"
#include <cstdlib>
#include <cmath>
#include "LocalSpaceRotation.h"

USING_NS_CC;


GLProgramState* AcceEffectSpace::_programState = nullptr;

void AcceEffectSpace::prepare()
{
    _width = _height = 1.2f;
    _count = _N *4*2*3;

    _data = static_cast<AcceEffectVertexFormat*>(malloc((_count) * sizeof(AcceEffectVertexFormat)));
    assert(_data);

    fillVertexData();

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AcceEffectVertexFormat)*_count, _data, GL_STREAM_DRAW);
        // origin position
        glEnableVertexAttribArray(AcceEffectSpace::VERTEX_ATTRIB_POSITION);
        glVertexAttribPointer(AcceEffectSpace::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(AcceEffectVertexFormat), (GLvoid *)offsetof(AcceEffectVertexFormat, position));

        // texcoord
        glEnableVertexAttribArray(AcceEffectSpace::VERTEX_ATTRIB_TEXCOORD);
        glVertexAttribPointer(AcceEffectSpace::VERTEX_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(AcceEffectVertexFormat), (GLvoid *)offsetof(AcceEffectVertexFormat, texcoord));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(AcceEffectVertexFormat)*_count, _data, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();

    free(_data);
}

AcceEffectSpace* AcceEffectSpace::create()
{
    if (_programState == nullptr) {
        initShaders();
    }

    auto p = new AcceEffectSpace();
    if (p && p->init()) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

void AcceEffectSpace::initShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/acce.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/acce.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_position", AcceEffectSpace::VERTEX_ATTRIB_POSITION);
    glprogram->bindAttribLocation("a_texcoord", AcceEffectSpace::VERTEX_ATTRIB_TEXCOORD);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}

bool AcceEffectSpace::init()
{
    assert(Node::init());

    prepare();
    setGLProgramState(_programState);

    return true;
}

AcceEffectSpace::AcceEffectSpace(){};
AcceEffectSpace::~AcceEffectSpace(){}

void AcceEffectSpace::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();
    auto loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &_color.r, 1);
    loc = glProgram->getUniformLocation("u_time");
    glProgram->setUniformLocationWith1f(loc, _time);
    loc = glProgram->getUniformLocation("u_percent");
    glProgram->setUniformLocationWith1f(loc, _percent);
    loc = glProgram->getUniformLocation("u_length");
    glProgram->setUniformLocationWith1f(loc, _length);
    glProgram->setUniformsForBuiltins(transform);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vao);
    }
    else
    {
        // TODO
//        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
//        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
//        // vertex
//        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));
//        // texcood
//        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));
    }
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(true);

    glDrawArrays(GL_TRIANGLES, 0, _count);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_count);
    CHECK_GL_ERROR_DEBUG();
}

void AcceEffectSpace::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if(_count)
    {
        _command.init(_globalZOrder, transform, flags);
        _command.func = CC_CALLBACK_0(AcceEffectSpace::onDraw, this, transform, flags);
        renderer->addCommand(&_command);
    }
}

void AcceEffectSpace::fillVertexData()
{
    const Vec3 bbl = {-_BONE_THIN, -_BONE_THIN, -0.5f * _BONE_LENGTH};
    const Vec3 bbr = {_BONE_THIN, -_BONE_THIN, -0.5f * _BONE_LENGTH};
    const Vec3 btl = {-_BONE_THIN, _BONE_THIN, -0.5f * _BONE_LENGTH};
    const Vec3 btr = {_BONE_THIN, _BONE_THIN, -0.5f * _BONE_LENGTH};

    const Vec3 fbl = {-_BONE_THIN, -_BONE_THIN, 0.5f * _BONE_LENGTH};
    const Vec3 fbr = {_BONE_THIN, -_BONE_THIN, 0.5f * _BONE_LENGTH};
    const Vec3 ftl = {-_BONE_THIN, _BONE_THIN, 0.5f * _BONE_LENGTH};
    const Vec3 ftr = {_BONE_THIN, _BONE_THIN, 0.5f * _BONE_LENGTH};

    int dataIndex = 0;
    for (int i = 0; i < _N; i++) {
        Vec3 center = {(rand_0_1()-0.5f)*_width, (rand_0_1()-0.5f)*_height, (rand_0_1()-0.5f)*_length};
        Tex2F tex = {i*1.0f/_N, rand_0_1()};

        _data[dataIndex++] = {center+btr, tex};
        _data[dataIndex++] = {center+btl, tex};
        _data[dataIndex++] = {center+ftl, tex};

        _data[dataIndex++] = {center+btr, tex};
        _data[dataIndex++] = {center+ftl, tex};
        _data[dataIndex++] = {center+ftr, tex};

        _data[dataIndex++] = {center+btl, tex};
        _data[dataIndex++] = {center+bbl, tex};
        _data[dataIndex++] = {center+fbl, tex};

        _data[dataIndex++] = {center+btl, tex};
        _data[dataIndex++] = {center+fbl, tex};
        _data[dataIndex++] = {center+ftl, tex};

        _data[dataIndex++] = {center+bbr, tex};
        _data[dataIndex++] = {center+btr, tex};
        _data[dataIndex++] = {center+ftr, tex};

        _data[dataIndex++] = {center+bbr, tex};
        _data[dataIndex++] = {center+ftr, tex};
        _data[dataIndex++] = {center+fbr, tex};

        _data[dataIndex++] = {center+bbl, tex};
        _data[dataIndex++] = {center+bbr, tex};
        _data[dataIndex++] = {center+fbr, tex};

        _data[dataIndex++] = {center+bbl, tex};
        _data[dataIndex++] = {center+fbr, tex};
        _data[dataIndex++] = {center+fbl, tex};
    }
}

inline Color4F operator+(const Color4F& l, const Color4F& r)
{
    return {l.r+r.r, l.g+r.g, l.b+r.b, l.a+r.a};
}
inline Color4F operator-(const Color4F& l, const Color4F& r)
{
    return {l.r-r.r, l.g-r.g, l.b-r.b, l.a-r.a};
}

inline Color4F operator*(const Color4F& color, const float f)
{
    return {color.r*f, color.g*f, color.b*f, color.a*f};
}

void  AcceEffectSpace::step(float dt)
{
    _time += dt;
    if (_aniColoring) {
        _aniColorTimed += dt;
        if (_aniColorTimed >= _aniColorDuration) {
            _aniColoring = false;
            _color = _aniTargetColor;
        } else {
            _color = _aniStartColor + (_aniTargetColor - _aniStartColor) * (_aniColorTimed/_aniColorDuration);
        }
    }
}

void AcceEffectSpace::ani2color(const cocos2d::Color4F& color, float duration)
{
    if (_color == color) {setColor(color);return;}
    if (duration == 0) {
        setColor(color);
    } else {
        _aniColoring = true;
        _aniStartColor = _color;
        _aniTargetColor = color;
        _aniColorTimed = 0;
        _aniColorDuration = duration;
    }
}

void AcceEffectSpace::setColor(const cocos2d::Color4F &color)
{
    _color = color;
}

void AcceEffectSpace::config(bool onOff, const cocos2d::Vec3& speed)
{
    constexpr float PI = 3.1415926;
    if (_lastState != onOff) {
        _zRandomRotateQua = rotation2quaternion({0,0, rand_0_1()*360});
    }
    if (this->isVisible() != onOff) this->setVisible(onOff);
    if (onOff) {
        float xSin = speed.x / speed.length();
        float xAngel = std::asin(xSin);
        float ySin = speed.y /speed.length();
        float yAngel = std::asin(ySin);

        auto speedRot = rotation2quaternion(Vec3{yAngel/PI * 180, xAngel/PI * 180, 0});
        speedRot.multiply(_zRandomRotateQua);
        this->setRotationQuat(speedRot);
    }
    setPercent(speed.length()/67);
    _lastState = onOff;
}


void AcceEffectSpace::setPercent(float per)
{
    _percent = per;
}
