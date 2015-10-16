// (C) 2015 Turnro.com

#include "DynamicGlass.h"
#include <cstdlib>
#include <cmath>
#include "intersection.h"

USING_NS_CC;

#define NUM_PER_EDGE_W_MIN 50
#define NUM_PER_EDGE_W_MAX 60

#define NUM_PER_EDGE_H_MIN 50
#define NUM_PER_EDGE_H_MAX 60


GLProgramState* DynamicGlass::_programState = nullptr;
Texture2D* DynamicGlass::_textureNormal = nullptr;
Texture2D* DynamicGlass::_textureShadow = nullptr;
bool DynamicGlass::_prepared = false;
GLuint DynamicGlass::_vao = 0;
GLuint DynamicGlass::_vbo = 0;
float DynamicGlass::_width = 0;
float DynamicGlass::_height = 0;
GLsizei DynamicGlass::_wNumber = 0;
GLsizei DynamicGlass::_hNumber = 0;
GLsizei DynamicGlass::_count = 0;
DynamicGlassVertexFormat* DynamicGlass::_data = nullptr;

void DynamicGlass::prepare()
{
    _prepared = true;
    _width = _height = 1.2f;
    _wNumber = NUM_PER_EDGE_W_MIN + rand()%(NUM_PER_EDGE_W_MAX - NUM_PER_EDGE_W_MIN);
    _hNumber = NUM_PER_EDGE_H_MIN + rand()%(NUM_PER_EDGE_H_MAX - NUM_PER_EDGE_H_MIN);

    _count = 3*2*(_wNumber * _hNumber);
    CCLOG("number = %d", _count);

    _data = static_cast<DynamicGlassVertexFormat*>(malloc((_count) * sizeof(DynamicGlassVertexFormat)));
    assert(_data);

    fillVertexData();

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DynamicGlassVertexFormat)*_count, _data, GL_STREAM_DRAW);
        // origin position
        glEnableVertexAttribArray(DynamicGlass::VERTEX_ATTRIB_ORIGIN_POS);
        glVertexAttribPointer(DynamicGlass::VERTEX_ATTRIB_ORIGIN_POS, 3, GL_FLOAT, GL_FALSE, sizeof(DynamicGlassVertexFormat), (GLvoid *)offsetof(DynamicGlassVertexFormat, origin_pos));

        // posture
        glEnableVertexAttribArray(DynamicGlass::VERTEX_ATTRIB_POSTURE);
        glVertexAttribPointer(DynamicGlass::VERTEX_ATTRIB_POSTURE, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicGlassVertexFormat), (GLvoid *)offsetof(DynamicGlassVertexFormat, posture));

        // target position
        glEnableVertexAttribArray(DynamicGlass::VERTEX_ATTRIB_TARGET_POS);
        glVertexAttribPointer(DynamicGlass::VERTEX_ATTRIB_TARGET_POS, 3, GL_FLOAT, GL_FALSE, sizeof(DynamicGlassVertexFormat), (GLvoid *)offsetof(DynamicGlassVertexFormat, target_pos));

        // texcoord
        glEnableVertexAttribArray(DynamicGlass::VERTEX_ATTRIB_TEXCOORD);
        glVertexAttribPointer(DynamicGlass::VERTEX_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicGlassVertexFormat), (GLvoid *)offsetof(DynamicGlassVertexFormat, texcoord));

        // center position
        glEnableVertexAttribArray(DynamicGlass::VERTEX_ATTRIB_CENTER_POS);
        glVertexAttribPointer(DynamicGlass::VERTEX_ATTRIB_CENTER_POS, 3, GL_FLOAT, GL_FALSE, sizeof(DynamicGlassVertexFormat), (GLvoid *)offsetof(DynamicGlassVertexFormat, center_pos));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DynamicGlassVertexFormat)*_count, _data, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();

    free(_data);
}

DynamicGlass* DynamicGlass::create( const cocos2d::Color4F& color)
{
    if (_programState == nullptr) {
        initShaders();
    }
    if (!_prepared) {
        prepare();
    }

    auto p = new DynamicGlass();
    if (p && p->init(color)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

void DynamicGlass::initShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/glass.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/glass.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_origin_pos", DynamicGlass::VERTEX_ATTRIB_ORIGIN_POS);
    glprogram->bindAttribLocation("a_posture", DynamicGlass::VERTEX_ATTRIB_POSTURE);
    glprogram->bindAttribLocation("a_target_pos", DynamicGlass::VERTEX_ATTRIB_TARGET_POS);
    glprogram->bindAttribLocation("a_texcoord", DynamicGlass::VERTEX_ATTRIB_TEXCOORD);
    glprogram->bindAttribLocation("a_center_pos", DynamicGlass::VERTEX_ATTRIB_CENTER_POS);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();

    _textureNormal = Director::getInstance()->getTextureCache()->addImage("3d/glass2.jpg");
    _textureShadow = Director::getInstance()->getTextureCache()->addImage("3d/glass_shadow.png");
}

bool DynamicGlass::init(const cocos2d::Color4F& color)
{
    assert(Node::init());

    _color = color;

    _lightColor = Color4F::WHITE;
    _lightDirection = Vec3{0,-1,0};

    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

    setGLProgramState(_programState);

    return true;
}

DynamicGlass::DynamicGlass():_factor(0){};


void DynamicGlass::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();
    auto loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &_color.r, 1);
    loc = glProgram->getUniformLocation("u_factor");
    glProgram->setUniformLocationWith1f(loc, _factor);
    loc = glProgram->getUniformLocation("u_texture_normal");
    glProgram->setUniformLocationWith1i(loc, 0);
    loc = glProgram->getUniformLocation("u_texture_shadow");
    glProgram->setUniformLocationWith1i(loc, 1);
    loc = glProgram->getUniformLocation("u_light_color");
    glProgram->setUniformLocationWith3fv(loc, &_lightColor.r, 1);
    loc = glProgram->getUniformLocation("u_light_direction");
    glProgram->setUniformLocationWith3fv(loc, &_lightDirection.x, 1);
    loc = glProgram->getUniformLocation("u_speed");
    glProgram->setUniformLocationWith1f(loc, _speed*25);
    GL::bindTexture2DN(0, _textureNormal->getName());
    GL::bindTexture2DN(1, _textureShadow->getName());
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

void DynamicGlass::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if(_count)
    {
        _command.init(_globalZOrder, transform, flags);
        _command.func = CC_CALLBACK_0(DynamicGlass::onDraw, this, transform, flags);
        renderer->addCommand(&_command);
    }
}

DynamicGlass::~DynamicGlass()
{
}

Quaternion genRandomQua()
{
    float s = rand_0_1();
    float σ1 = sqrt(1 - s);
    float σ2 = sqrt(s);
    float θ1 = 2*M_PI * rand_0_1();
    float θ2 = 2*M_PI * rand_0_1();
    float w = cos(θ2) * σ2 ;
    float x = sin(θ1) * σ1 ;
    float y = cos(θ1) * σ1 ;
    float z = sin(θ2) * σ2 ;
    return Quaternion(w, x, y, z);
//    Quaternion qua = {rand_0_1()*2 -1.f,rand_0_1()*2 -1.f,rand_0_1()*2 -1.f,
//        rand_0_1()*2 -1.f};
//    qua.normalize();
//    return  qua;
}
Vec3 genRandomDiffPos()
{
    const float xy_factor = 5.f;
    return {(rand_0_1()-0.5f)*2.0f*xy_factor, (rand_0_1()-0.5f)*2.0f*xy_factor ,-(0.f +30* rand_0_1())};
}

void DynamicGlass::fillVertexData()
{
    float x_step = _width / _wNumber;
    float y_step = _height / _hNumber;
    Vec3 origin = Vec3{-_width/2, -_height/2, 0};
    int dataIndex = 0;

    for (int i = 0; i < _wNumber; i++) {
        for (int j = 0; j < _hNumber; j++) {
            Vec3 leftBottom = {i*x_step, j*y_step, 0};
            Vec3 rightBottom = leftBottom + Vec3{x_step, 0, 0};
            Vec3 leftTop = leftBottom + Vec3{0, y_step, 0};
            Vec3 rightTop = leftBottom + Vec3{x_step, y_step, 0};
            Quaternion qua = genRandomQua();
            Vec3 diff = genRandomDiffPos();
            _data[dataIndex++] = {origin + rightTop, qua, diff, Tex2F{0,0}, origin + 0.33333 *(rightTop + leftTop + leftBottom) };
            _data[dataIndex++] = {origin + leftTop, qua, diff, Tex2F{0,0}, origin + 0.33333 *(rightTop + leftTop + leftBottom) };
            _data[dataIndex++] = {origin + leftBottom, qua, diff, Tex2F{0,0}, origin + 0.33333 *(rightTop + leftTop + leftBottom) };

            qua = genRandomQua();
            diff = genRandomDiffPos();
            _data[dataIndex++] = {origin + rightTop, qua, diff, Tex2F{0,0}, origin + 0.33333 *(rightTop + leftTop + leftBottom) };
            _data[dataIndex++] = {origin + leftBottom, qua, diff, Tex2F{0,0}, origin + 0.33333 *(rightTop  + leftBottom + rightBottom) };
            _data[dataIndex++] = {origin + rightBottom, qua, diff, Tex2F{0,0}, origin + 0.33333 *(rightTop  + leftBottom + rightBottom) };
        }
    }

    // 重算texcoord
    float f_x = 1.f/_width;
    float f_y = 1.f/_height;
    for (int i = 0; i < _count; i++){
        _data[i].texcoord.u = (_data[i].origin_pos.x - origin.x) * f_x;
        _data[i].texcoord.v = (_data[i].origin_pos.y - origin.y) * f_y;
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

void  DynamicGlass::step(float dt)
{
    if (_hiting) {
        _factor += 0.1*dt/_speed;
        if (_factor > 1) {
            _hiting = false;
            _factor = 1;
        }
    }

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

void DynamicGlass::reset()
{
    _factor = 0;
    _hiting = false;
    _hitted = false;
    _aniColoring = false;
}

void DynamicGlass::hit(float speed)
{
    _hiting = true;
    _speed = speed > 1 ? speed : 1;
}

void DynamicGlass::beat()
{
    const float t = 0.11f;
    this->runAction(Sequence::create(ScaleTo::create(t, 1.25), ScaleTo::create(t, 1), nullptr));
}

void DynamicGlass::ani2color(const cocos2d::Color4F& color, float duration)
{
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

void DynamicGlass::setColor(const cocos2d::Color4F &color)
{
    _color = color;
}

bool DynamicGlass::checkHit(const cocos2d::Vec3& pointA, const cocos2d::Vec3& pointB, float speed)
{
    if (_hitted) return false;

    Vec3 bl = this->getPosition3D() + this->getRotationQuat() * Vec3{-0.5f*_width, -0.5f*_height, 0};
    Vec3 br = this->getPosition3D() + this->getRotationQuat() * Vec3{0.5f*_width, -0.5f*_height, 0};
    Vec3 tl = this->getPosition3D() + this->getRotationQuat() * Vec3{-0.5f*_width, 0.5f*_height, 0};
    Vec3 tr = this->getPosition3D() + this->getRotationQuat() * Vec3{0.5f*_width, 0.5f*_height, 0};

    if (segment_trangle_intersection(tr, tl, bl, pointA, pointB) ||
        segment_trangle_intersection(tr, bl, br, pointA, pointB))
    {
        _hitted = true;
        hit(speed);
        return  true;
    } else {
        return  false;
    }
}

