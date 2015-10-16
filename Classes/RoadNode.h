// (C)2015 Turnro Game

#ifndef __gmap_editor__TRDrawNode3D2__
#define __gmap_editor__TRDrawNode3D2__

// DrawNode's 3d version.
#include "cocos2d.h"

class RoadNode: public cocos2d::Node
{
public:
    CREATE_FUNC(RoadNode);
    static void initShaders(); //初始化shared _programState...
    void setLineWidth(float width);
    void setLineColor(const cocos2d::Color4F& color);
    void setRoadColor(const cocos2d::Color4F& color);
    // TODO color ani!!!
    void drawLine(const cocos2d::Vec3& src, const cocos2d::Vec3& dest);
    void onDrawGLLine(const cocos2d::Mat4 &transform, uint32_t flags);
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t     flags) override;
    virtual void setOpacity(GLubyte opacity) override;
    GLubyte getOpacity() const;
    void drawRoad(const cocos2d::Vec3& bl, const cocos2d::Vec3& br, const cocos2d::Vec3& tl, const cocos2d::Vec3& tr);
    void drawTriangle(const cocos2d::Vec3& v1, const cocos2d::Vec3& v2, const cocos2d::Vec3& v3,     const cocos2d::Tex2F& tv1, const cocos2d::Tex2F& tv2, const cocos2d::Tex2F& tv3);
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);

protected:
    RoadNode();
    virtual ~RoadNode();

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
    cocos2d::Color4F _roadColor;
    float _lineWidth;
    GLubyte _opacity;

    // Road的渲染program与texture
    static cocos2d::GLProgram* _roadProgram;
    static cocos2d::Texture2D* _roadTexture;
    static cocos2d::BlendFunc _roadBlendFunc;

    static cocos2d::GLProgram* _lineProgram;
    static cocos2d::BlendFunc _lineBlendFunc;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(RoadNode);
};

#endif /* defined(__gmap_editor__TRDrawNode3D__) */
