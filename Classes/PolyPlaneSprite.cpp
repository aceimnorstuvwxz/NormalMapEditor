// (C) 2015 Turnro.com

#include "PolyPlaneSprite.h"

USING_NS_CC;

bool PolyPlaneSprite::init()
{
    assert(Node::init());

    initShader();
    initRenderData();

    return  true;
}

void PolyPlaneSprite::initShader()
{
    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/shadow.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/shadow.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_position", GLProgram::VERTEX_ATTRIB_POSITION);
    glprogram->bindAttribLocation("a_color", GLProgram::VERTEX_ATTRIB_COLOR);
    glprogram->bindAttribLocation("a_normal", GLProgram::VERTEX_ATTRIB_NORMAL);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    assert(_programState);
    _programState->retain();
    this->setGLProgramState(_programState);
}

void  PolyPlaneSprite::initRenderData()
{
    // init the flat plane mesh
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<float> texs;

    float vertices[4*5] = { // pos, tex
        0.5, 0.5, 0.0, 1.0, 0.0,
        0.5, -0.5,0.0,  1.0, 1.0,
        -0.5,-0.5, 0.0,  0.0, 1.0,
        -0.5, 0.5, 0.0,  0.0, 0.0
    };
    for (int i = 0; i < 4; i++) {
        positions.push_back(vertices[5*i]);
        positions.push_back(vertices[5*i+1]);
        positions.push_back(vertices[5*i+2]);
    }
    std::vector<unsigned short> triangleIndex = {
        2,1,0,
        0,3,2
    };

    _renderMesh = Mesh::create(positions, normals, texs, triangleIndex);
    assert(_renderMesh != nullptr);
    _renderMesh->retain();

    long offset = 0;
    auto attributeCount = _renderMesh->getMeshVertexAttribCount();
    for (auto k = 0; k < attributeCount; k++) {
        auto meshattribute = _renderMesh->getMeshVertexAttribute(k);
        _programState->setVertexAttribPointer(s_attributeNames[meshattribute.vertexAttrib],
                                              meshattribute.size,
                                              meshattribute.type,
                                              GL_FALSE,
                                              _renderMesh->getVertexSizeInBytes(),
                                              (GLvoid*)offset);
        offset += meshattribute.attribSizeBytes;
    }
    
}

void PolyPlaneSprite::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{

    CC_SAFE_DELETE(_meshCommand);
    _meshCommand = new MeshCommand();
    //    _meshCommand->genMaterialID(0, getGLProgramState(), _renderMesh->getVertexBuffer(), _renderMesh->getIndexBuffer(), _blendFunc);
    _meshCommand->init(_globalZOrder, 0, getGLProgramState(), _blendFunc, _renderMesh->getVertexBuffer(), _renderMesh->getIndexBuffer(), (GLenum)_renderMesh->getPrimitiveType(), (GLenum)_renderMesh->getIndexFormat(), _renderMesh->getIndexCount(), transform, flags);

    _meshCommand->setCullFaceEnabled(false);
    _meshCommand->setDepthTestEnabled(false);
    _meshCommand->setTransparent(true);
    _meshCommand->setDepthWriteEnabled(true);

    _meshCommand->setDisplayColor(_color);
    renderer->addCommand(_meshCommand);

    Node::draw(renderer, transform, flags);
}
void PolyPlaneSprite::setColor(const cocos2d::Vec4& color)
{
    _color = color;
}

