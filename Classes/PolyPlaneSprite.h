// (C) 2015 Turnro.com

#ifndef __Turnroll__PolyPlaneSprite__
#define __Turnroll__PolyPlaneSprite__

#include "cocos2d.h"

class PolyPlaneSprite:public cocos2d::Node
{
public:
    virtual bool init() override;
    CREATE_FUNC(PolyPlaneSprite);
    void setColor(const cocos2d::Vec4& color);

    void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags);
private:
    cocos2d::GLProgramState* _programState = nullptr;
    cocos2d::Mesh* _renderMesh = nullptr;
    cocos2d::Texture2D* _texture = nullptr;
    cocos2d::MeshCommand* _meshCommand = nullptr;
    cocos2d::BlendFunc _blendFunc;
    cocos2d::Vec4 _color;
    void initShader();
    void initRenderData();

};

#endif /* defined(__Turnroll__PolyPlaneSprite__) */
