// (C) 2015 Turnro.com

#include "BattleField.h"
#include "DDConfig.h"

USING_NS_CC;
GLuint BattleLowNode::_vao;
GLuint BattleLowNode::_vbo;
cocos2d::GLProgramState* BattleLowNode::_programState = nullptr;

bool BattleLowNode::init()
{
    assert(BattleNode::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    return true;
}

void BattleLowNode::prepareVertexData()
{
    V3F_T2F vertexData[6];
    float halfWidth = DDConfig::battleCubeWidth()*0.5f;
    vertexData[0] = {Vec3{halfWidth,halfWidth,0}, Tex2F{1,0}};
    vertexData[1] = {Vec3{-halfWidth,halfWidth,0}, Tex2F{0,0}};
    vertexData[2] = {Vec3{-halfWidth,-halfWidth,0}, Tex2F{0,1}};

    vertexData[3] = {Vec3{halfWidth,halfWidth,0}, Tex2F{1,0}};
    vertexData[4] = {Vec3{-halfWidth,-halfWidth,0}, Tex2F{0,1}};
    vertexData[5] = {Vec3{halfWidth,-halfWidth,0}, Tex2F{1,1}};

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*6, vertexData, GL_STREAM_DRAW);
        // origin position
        glEnableVertexAttribArray(MY_VERTEX_ATTRIB_POSITION);
        glVertexAttribPointer(MY_VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, vertices));

        // texcoord
        glEnableVertexAttribArray(MY_VERTEX_ATTRIB_TEXCOORD);
        glVertexAttribPointer(MY_VERTEX_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_T2F), (GLvoid *)offsetof(V3F_T2F, texCoords));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_T2F)*6, vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();
}

void BattleLowNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/battle_low.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/battle_low.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", MY_VERTEX_ATTRIB_POSITION);
    glprogram->bindAttribLocation("a_texcoord", MY_VERTEX_ATTRIB_TEXCOORD);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}


void BattleLowNode::setFieldBg(const std::string& fieldBgFile)
{
    _textureFieldBg = Director::getInstance()->getTextureCache()->addImage(fieldBgFile);

}
void BattleLowNode::setBuildingBg(const std::string& buildingBgFile)
{
    _textureBuildingBg = Director::getInstance()->getTextureCache()->addImage(buildingBgFile);
}

void BattleLowNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(BattleLowNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void BattleLowNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();

    //    auto loc = glProgram->getUniformLocation("u_color");
    //    glProgram->setUniformLocationWith4fv(loc, &_color.r, 1);
    //    loc = glProgram->getUniformLocation("u_factor");
    //    glProgram->setUniformLocationWith1f(loc, _factor);

    //    loc = glProgram->getUniformLocation("u_texture_shadow");
    //    glProgram->setUniformLocationWith1i(loc, 1);
    //    loc = glProgram->getUniformLocation("u_light_color");
    //    glProgram->setUniformLocationWith3fv(loc, &_lightColor.r, 1);
    //    loc = glProgram->getUniformLocation("u_light_direction");
    //    glProgram->setUniformLocationWith3fv(loc, &_lightDirection.x, 1);
    //    loc = glProgram->getUniformLocation("u_speed");
    //    glProgram->setUniformLocationWith1f(loc, _speed*25);
    //    GL::bindTexture2DN(1, _textureShadow->getName());
    auto loc = glProgram->getUniformLocation("u_texture_fieldbg");
    glProgram->setUniformLocationWith1i(loc, 0);
    GL::bindTexture2DN(0, _textureFieldBg->getName());

    loc = glProgram->getUniformLocation("u_texture_buildingbg");
    glProgram->setUniformLocationWith1i(loc, 1);
    GL::bindTexture2DN(1, _textureBuildingBg->getName());

    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vao);
    }
    else
    {
        // TODO
    }
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*2D的，都在同一Z位置，开启depth test会导致后画的不再绘制。
     glEnable(GL_DEPTH_TEST);
     glDepthMask(true);
     */

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,6);
    CHECK_GL_ERROR_DEBUG();
}