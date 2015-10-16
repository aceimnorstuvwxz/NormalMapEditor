// (C)2015 Turnro Game

#include "PixelPlane.h"
#include "intersection.h"

USING_NS_CC;

PixelPlane* PixelPlane::create(const std::string& fsh, const std::string& soundTextureFile, const std::string& imageTextureFile)
{
    auto p = new PixelPlane();
    if (p && p->init(fsh, soundTextureFile, imageTextureFile)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;

}


bool PixelPlane::init(const std::string& fsh, const std::string& soundTextureFile, const std::string& imageTextureFile)
{
    assert(Node::init());
    _pixelProgram =  GLProgramCache::getInstance()->getGLProgram(fsh);
    if (!_pixelProgram) {
        auto fileUtils = FileUtils::getInstance();
        auto vertSource = fileUtils->getStringFromFile("3d/pixel_plane.vsh");//所有pixelplane都使用它，而fsh可能不同。
        auto fragSource = fileUtils->getStringFromFile(fsh);

        auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
        glprogram->bindAttribLocation("a_position", 0);
        glprogram->bindAttribLocation("a_texCoord", 1);
        glprogram->link();
        glprogram->updateUniforms();
        GLProgramCache::getInstance()->addGLProgram(glprogram, fsh);

        _pixelProgram = glprogram;
    }
    _pixelProgram->retain();
    if (soundTextureFile.length() > 0) {
        _soundTexture = Director::getInstance()->getTextureCache()->addImage(soundTextureFile);
        _soundTexture->retain();
    }
    if (imageTextureFile.length() > 0) {
        _imageTexture = Director::getInstance()->getTextureCache()->addImage(imageTextureFile);
        _imageTexture->retain();
    }

    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    ensureCapacity(512);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);
        // vertex
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, vertices));
        // color
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, colors));
        // texcood
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, texCoords));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    CHECK_GL_ERROR_DEBUG();
    _dirty = true;

    return true;
}
void PixelPlane::ensureCapacity(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");

    if(_bufferCount + count > _bufferCapacity)
    {
        _bufferCapacity += MAX(_bufferCapacity, count);
        _buffer = (V3F_C4B_T2F*)realloc(_buffer, _bufferCapacity*sizeof(V3F_C4B_T2F));
    }
}

PixelPlane::PixelPlane():
_opacity(255)
, _vao(0)
, _vbo(0)
, _buffer(nullptr)
, _bufferCapacity(0)
, _bufferCount(0)
, _dirty(false)
, _roadColor(Color4F::WHITE)
{
}


void PixelPlane::setOpacity(GLubyte opacity){
    _opacity = opacity;
}

GLubyte PixelPlane::getOpacity() const
{
    return _opacity;
}


void PixelPlane::drawRoad(const cocos2d::Vec3& bl, const cocos2d::Vec3& br, const cocos2d::Vec3& tl, const cocos2d::Vec3& tr)
{

    static const cocos2d::Tex2F ctvbl = {0,0};
    static const cocos2d::Tex2F ctvbr = {1,0};
    static const cocos2d::Tex2F ctvtl = {0,1};
    static const cocos2d::Tex2F ctvtr = {1,1};

    drawTriangle(tr,tl,bl, ctvtr, ctvtl, ctvbl);
    drawTriangle(tr,bl,br, ctvtr, ctvbl, ctvbr);
}

void PixelPlane::drawTriangle(const cocos2d::Vec3& v1, const cocos2d::Vec3& v2, const cocos2d::Vec3& v3, const cocos2d::Tex2F& tv1, const cocos2d::Tex2F& tv2, const cocos2d::Tex2F& tv3)
{
    ensureCapacity(3);
    V3F_C4B_T2F *point = (V3F_C4B_T2F*)(_buffer + _bufferCount);
    point[0] = {v1, Color4B::WHITE, tv1};
    point[1] = {v2, Color4B::WHITE, tv2};
    point[2] = {v3, Color4B::WHITE, tv3};
    _bufferCount += 3;
    _dirty = true;
}

void PixelPlane::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgram = _pixelProgram;
    glProgram->use();
    Color4F color = _roadColor;
    color.a = color.a *_opacity /255.f;
    auto loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &color.r, 1);
    if (_soundTexture) {
        loc = glProgram->getUniformLocation("u_soundTexture");
        glProgram->setUniformLocationWith1i(loc, 0);
        GL::bindTexture2DN(0, _soundTexture->getName());
    }
    if (_imageTexture) {
        loc = glProgram->getUniformLocation("u_imageTexture");
        glProgram->setUniformLocationWith1i(loc, 1);
        GL::bindTexture2DN(1, _imageTexture->getName());
    }
    loc = glProgram->getUniformLocation("iGlobalTime");
    glProgram->setUniformLocationWith1f(loc, _shaderTime);

    if (_power) {
        loc = glProgram->getUniformLocation("u_power");
        glProgram->setUniformLocationWith1f(loc, _power);
    }

    glProgram->setUniformsForBuiltins(transform);

    GL::blendFunc(_blendFunc.src, _blendFunc.dst);

    if (_dirty)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)*_bufferCapacity, _buffer, GL_STREAM_DRAW);

        _dirty = false;
    }
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vao);
    }
    else
    {
        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        // vertex
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, vertices));
        // color
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, colors));
        // texcood
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, texCoords));
    }


    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    glDrawArrays(GL_TRIANGLES, 0, _bufferCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _bufferCount);
    CHECK_GL_ERROR_DEBUG();
}

void PixelPlane::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if(_bufferCount)
    {
        _customCommand.init(_globalZOrder, transform, flags);
        _customCommand.func = CC_CALLBACK_0(PixelPlane::onDraw, this, transform, flags);
        renderer->addCommand(&_customCommand);
    }
}

PixelPlane::~PixelPlane()
{
    free(_buffer);
    _buffer = nullptr;

    glDeleteBuffers(1, &_vbo);
    _vbo = 0;

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
    
    CC_SAFE_RELEASE(_pixelProgram);
    CC_SAFE_RELEASE(_soundTexture);
    CC_SAFE_RELEASE(_imageTexture);
}

void PixelPlane::setRoadColor(const cocos2d::Color4F& color)
{
    _roadColor = color;
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
void PixelPlane::step(float dt)
{
    _shaderTime = dt;

    if (_aniColoring) {
        _aniColorTimed += dt;
        if (_aniColorTimed >= _aniColorDuration) {
            _aniColoring = false;
            _roadColor = _aniTargetColor;
        } else {
            _roadColor = _aniStartColor + (_aniTargetColor - _aniStartColor) * (_aniColorTimed/_aniColorDuration);
        }
    }
}

void PixelPlane::reset()
{
    _hitted = false;
}
void PixelPlane::setGlassLength(float len)
{
    _glassLength = len;
}

bool PixelPlane::checkHit(const cocos2d::Vec3& pointA, const cocos2d::Vec3& pointB)
{
    if (_hitted) return false;

    Vec3 bl = this->getPosition3D() + this->getRotationQuat() * Vec3{-0.5f*_glassLength, -0.5f*_glassLength, 0};
    Vec3 br = this->getPosition3D() + this->getRotationQuat() * Vec3{0.5f*_glassLength, -0.5f*_glassLength, 0};
    Vec3 tl = this->getPosition3D() + this->getRotationQuat() * Vec3{-0.5f*_glassLength, 0.5f*_glassLength, 0};
    Vec3 tr = this->getPosition3D() + this->getRotationQuat() * Vec3{0.5f*_glassLength, 0.5f*_glassLength, 0};

    if (segment_trangle_intersection(tr, tl, bl, pointA, pointB) ||
        segment_trangle_intersection(tr, bl, br, pointA, pointB))
    {
        _hitted = true;
        return  true;
    } else {
        return  false;
    }
}

void PixelPlane::aniRoadColor(const cocos2d::Color4F& color, float duration)
{
    if (duration == 0) {
        setRoadColor(color);
    } else {
        _aniColoring = true;
        _aniStartColor = _roadColor;
        _aniTargetColor = color;
        _aniColorTimed = 0;
        _aniColorDuration = duration;
    }
}

void PixelPlane::setPower(float pow)
{
    _power = pow;
}

