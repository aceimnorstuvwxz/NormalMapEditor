// (C)2015 Turnro Game

#include "TRDrawNode3D.h"

USING_NS_CC;

bool TRDrawNode3D::init()
{
    assert(Node::init());

    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_3D_POSITION));
    ensureCapacity(512);
    ensureCapacityGLLine(256);

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



        glGenVertexArrays(1, &_vaoGLLine);
        GL::bindVAO(_vaoGLLine);
        glGenBuffers(1, &_vboGLLine);
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*_bufferCapacityGLLine, _bufferGLLine, GL_STREAM_DRAW);
        // vertex
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));

        // texcood
        glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);

        glGenBuffers(1, &_vboGLLine);
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*_bufferCapacityGLLine, _bufferGLLine, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    CHECK_GL_ERROR_DEBUG();
    _dirty = true;
    _dirtyGLLine = true;

    return true;
}
void TRDrawNode3D::ensureCapacity(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");

    if(_bufferCount + count > _bufferCapacity)
    {
        _bufferCapacity += MAX(_bufferCapacity, count);
        _buffer = (V3F_C4B_T2F*)realloc(_buffer, _bufferCapacity*sizeof(V3F_C4B_T2F));
    }
}
void TRDrawNode3D::ensureCapacityGLLine(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");

    if(_bufferCountGLLine + count > _bufferCapacityGLLine)
    {
        _bufferCapacityGLLine += MAX(_bufferCapacityGLLine, count);
        _bufferGLLine = (V3F_T2F*)realloc(_bufferGLLine, _bufferCapacityGLLine*sizeof(V3F_T2F));
    }
}
TRDrawNode3D::TRDrawNode3D()
: _vaoGLLine(0)
, _vboGLLine(0)
, _bufferCapacityGLLine(0)
, _bufferCountGLLine(0)
, _bufferGLLine(nullptr)
, _dirtyGLLine(false)
, _lineColor(Color4F::WHITE)
, _lineWidth(1.f)
, _opacity(255)
, _vao(0)
, _vbo(0)
, _buffer(nullptr)
, _bufferCapacity(0)
, _bufferCount(0)
, _dirty(false)
, _triangleColor(Color4F::WHITE)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}

void TRDrawNode3D::setLineColor(const cocos2d::Color4F& color)
{
    _lineColor = color;
}

void TRDrawNode3D::setTriangleColor(const cocos2d::Color4F& color)
{
    _triangleColor = color;
}


void TRDrawNode3D::setOpacity(GLubyte opacity){
    _opacity = opacity;
}

GLubyte TRDrawNode3D::getOpacity() const
{
    return _opacity;
}


void TRDrawNode3D::setLineWidth(float width)
{
    _lineWidth = width;
}

void TRDrawNode3D::drawTriangle(const cocos2d::Vec3& v1, const cocos2d::Vec3& v2, const cocos2d::Vec3& v3)
{
    ensureCapacity(3);
    V3F_C4B_T2F *point = (V3F_C4B_T2F*)(_buffer + _bufferCount);
    point[0] = {v1, Color4B::WHITE, Tex2F{0,0}};
    point[1] = {v2, Color4B::WHITE, Tex2F{0,0}};
    point[2] = {v3, Color4B::WHITE, Tex2F{0,0}};
    _bufferCount += 3;
    _dirty = true;
}

void TRDrawNode3D::drawLine(const cocos2d::Vec3& src, const cocos2d::Vec3& dest)
{
    ensureCapacityGLLine(2);
    V3F_T2F *point = (V3F_T2F*)(_bufferGLLine + _bufferCountGLLine);

    V3F_T2F a = {src,Tex2F(0.0, 0.0)};
    V3F_T2F b = {dest,Tex2F(0.0, 0.0)};

    *point = a;
    *(point+1) = b;

    _bufferCountGLLine += 2;
    _dirtyGLLine = true;
}

void TRDrawNode3D::onDrawGLLine(const cocos2d::Mat4 &transform, uint32_t flags)
{
    auto glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_POSITION);
    glProgram->use();
    Color4F color = _lineColor;
    color.a = color.a *_opacity /255.f;
    auto loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &color.r, 1);
    glProgram->setUniformsForBuiltins(transform);

    if (_dirtyGLLine)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*_bufferCapacityGLLine, _bufferGLLine, GL_STREAM_DRAW);
        _dirtyGLLine = false;
    }
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vaoGLLine);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
        // vertex
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));
        // texcood
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));
    }

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(_lineWidth);
    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDrawArrays(GL_LINES, 0, _bufferCountGLLine);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_bufferCountGLLine);
    CHECK_GL_ERROR_DEBUG();
}

void TRDrawNode3D::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_POSITION);
    glProgram->use();
    Color4F color = _triangleColor;
    color.a = color.a *_opacity /255.f;
    auto loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &color.r, 1);
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

    glDrawArrays(GL_TRIANGLES, 0, _bufferCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _bufferCount);
    CHECK_GL_ERROR_DEBUG();
}

void TRDrawNode3D::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if(_bufferCount)
    {
        _customCommand.init(_globalZOrder, transform, flags);
        _customCommand.func = CC_CALLBACK_0(TRDrawNode3D::onDraw, this, transform, flags);
        renderer->addCommand(&_customCommand);
    }
    if(_bufferCountGLLine)
    {
        _customCommandGLLine.init(_globalZOrder, transform, flags);
        _customCommandGLLine.func = CC_CALLBACK_0(TRDrawNode3D::onDrawGLLine, this, transform, flags);
        renderer->addCommand(&_customCommandGLLine);
    }
}

TRDrawNode3D::~TRDrawNode3D()
{
    free(_bufferGLLine);
    _bufferGLLine = nullptr;

    glDeleteBuffers(1, &_vboGLLine);
    _vboGLLine = 0;

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
        glDeleteVertexArrays(1, &_vaoGLLine);
        _vaoGLLine = 0;
    }
}


