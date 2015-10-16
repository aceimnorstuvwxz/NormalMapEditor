// (C)2015 Turnro Game

#ifndef __gmap_editor__TRDrawNode3D__
#define __gmap_editor__TRDrawNode3D__

// DrawNode's 3d version.
#include "cocos2d.h"

class TRDrawNode3D: public cocos2d::Node
{
public:
    CREATE_FUNC(TRDrawNode3D);
    void setLineWidth(float width);
    void setLineColor(const cocos2d::Color4F& color);
    void drawLine(const cocos2d::Vec3& src, const cocos2d::Vec3& dest);
    void onDrawGLLine(const cocos2d::Mat4 &transform, uint32_t flags);
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    virtual void setOpacity(GLubyte opacity) override;
    GLubyte getOpacity() const;
    void drawTriangle(const cocos2d::Vec3& v1, const cocos2d::Vec3& v2, const cocos2d::Vec3& v3);
    void setTriangleColor(const cocos2d::Color4F& color);
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);

protected:
    TRDrawNode3D();
    virtual ~TRDrawNode3D();

    virtual bool init() override;
    void ensureCapacity(int count);
    void ensureCapacityGLLine(int count);
    GLuint      _vao;
    GLuint      _vbo;
    GLuint      _vaoGLLine;
    GLuint      _vboGLLine;
    int         _bufferCapacity;
    GLsizei     _bufferCount;
    cocos2d::V3F_C4B_T2F *_buffer;
    int         _bufferCapacityGLLine;
    GLsizei     _bufferCountGLLine;
    cocos2d::V3F_T2F *_bufferGLLine;
    cocos2d::BlendFunc   _blendFunc;
    cocos2d::CustomCommand _customCommand;

    cocos2d::CustomCommand _customCommandGLLine;
    bool        _dirty;
    bool        _dirtyGLLine;
    cocos2d::Color4F _lineColor;
    cocos2d::Color4F _triangleColor;
    float _lineWidth;
    GLubyte _opacity;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(TRDrawNode3D);
};

#endif /* defined(__gmap_editor__TRDrawNode3D__) */
