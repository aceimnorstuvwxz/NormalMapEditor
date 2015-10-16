// (C)2015 Turnro Game

#ifndef __gmap_editor__TRDrawNode3D3__
#define __gmap_editor__TRDrawNode3D3__

#include "cocos2d.h"
#include <unordered_map>

// 与TRDrawNode3D 类似的，但是结合了shaderToy...
// GLProgram是每个实例可以指定的，而不再通用...
// GLProgram使用定制的cache机制...更新，cache用c2dx提供的.

class PixelPlane: public cocos2d::Node
{
public:
    static PixelPlane* create(const std::string& fsh, const std::string& soundTextureFile = "", const std::string& imageTextureFile = "");
    void setRoadColor(const cocos2d::Color4F& color);
    void aniRoadColor(const cocos2d::Color4F& color, float duration = 3.f);
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t     flags) override;
    virtual void setOpacity(GLubyte opacity) override;
    GLubyte getOpacity() const;
    void drawRoad(const cocos2d::Vec3& bl, const cocos2d::Vec3& br, const cocos2d::Vec3& tl, const cocos2d::Vec3& tr);
    void drawTriangle(const cocos2d::Vec3& v1, const cocos2d::Vec3& v2, const cocos2d::Vec3& v3,     const cocos2d::Tex2F& tv1, const cocos2d::Tex2F& tv2, const cocos2d::Tex2F& tv3);
    void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
    void step(float time);
    void setGlassLength(float len);
    bool checkHit(const cocos2d::Vec3& pointA, const cocos2d::Vec3& pointB);//线段与glass的碰撞，与_hitted配合，当碰到时会触发hit()。
    void reset();
    void setPower(float pow);
protected:
    PixelPlane();
    virtual ~PixelPlane();

    virtual bool init(const std::string& fsh, const std::string& soundTextureFile, const std::string& imageTextureFile);
    void ensureCapacity(int count);
    GLuint      _vao;
    GLuint      _vbo;
    int         _bufferCapacity;
    GLsizei     _bufferCount;
    cocos2d::V3F_C4B_T2F *_buffer;
    cocos2d::BlendFunc   _blendFunc;
    cocos2d::CustomCommand _customCommand;
    bool        _dirty;
    cocos2d::Color4F _roadColor;
    GLubyte _opacity;

    // PixelPlane的渲染program与texture
    cocos2d::GLProgram* _pixelProgram = nullptr;
    cocos2d::Texture2D* _soundTexture = nullptr;
    cocos2d::Texture2D* _imageTexture = nullptr;

    float _shaderTime = 0;

    bool _hitted = false;
    float _glassLength;

    bool _aniColoring = false;
    cocos2d::Color4F _aniStartColor;
    cocos2d::Color4F _aniTargetColor;
    float _aniColorTimed = 0;
    float _aniColorDuration = 0;

    float _power = 0;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(PixelPlane);
};

#endif /* defined(__gmap_editor__TRDrawNode3D__) */
