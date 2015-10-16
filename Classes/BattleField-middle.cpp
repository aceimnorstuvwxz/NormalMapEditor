// (C) 2015 Turnro.com

#include "BattleField.h"
#include "DDConfig.h"

USING_NS_CC;
GLuint BattleMiddleNode::_vao;
GLuint BattleMiddleNode::_vbo;
cocos2d::GLProgramState* BattleMiddleNode::_programState = nullptr;

bool BattleMiddleNode::init()
{
    assert(BattleNode::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    _textureActionScopeLine = Director::getInstance()->getTextureCache()->addImage("images/middle_scope_line.png");
    _texturePlacement = Director::getInstance()->getTextureCache()->addImage("images/middle_placement.png");
    _textureSelection = Director::getInstance()->getTextureCache()->addImage("images/middle_selection.png");
    _textureBOSS = Director::getInstance()->getTextureCache()->addImage("images/middle_boss.png");
    _textureLowBlood = Director::getInstance()->getTextureCache()->addImage("images/middle_lowblood.png");

    for (int i = 0; i < DDConfig::NUM_LIGHT; i++) {
        _lights[i] = {0,0,0,0};
    }
    /*
    _lights[3] = {1,1,1,1};
    _lights[4] = {-1,0,1,1};

    _lights[5] = {0,-1,1,1};
*/
    return true;
}

void BattleMiddleNode::prepareVertexData()
{
    V3F_T2F vertexData[6];
    float halfWidth = DDConfig::battleCubeWidth()*0.5f*(1 + 2* DDConfig::EXPAND_SHADOW); //middle层是拓展的，因为部分元素会渲染到其它cube的空间内。
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

void BattleMiddleNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/battle_middle.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/battle_middle.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", MY_VERTEX_ATTRIB_POSITION);
    glprogram->bindAttribLocation("a_texcoord", MY_VERTEX_ATTRIB_TEXCOORD);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}



void BattleMiddleNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(BattleMiddleNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void BattleMiddleNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
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
    auto loc = glProgram->getUniformLocation("u_texture_shadow");
    glProgram->setUniformLocationWith1i(loc, 0);
    GL::bindTexture2DN(0, _textureShadow->getName());

    loc = glProgram->getUniformLocation("u_texture_action_scope_line");
    glProgram->setUniformLocationWith1i(loc, 1);
    GL::bindTexture2DN(1, _textureActionScopeLine->getName());

    loc = glProgram->getUniformLocation("u_texture_placement");
    glProgram->setUniformLocationWith1i(loc, 2);
    GL::bindTexture2DN(2, _texturePlacement->getName());

    loc = glProgram->getUniformLocation("u_texture_selection");
    glProgram->setUniformLocationWith1i(loc, 3);
    GL::bindTexture2DN(3, _textureSelection->getName());

    loc = glProgram->getUniformLocation("u_texture_boss");
    glProgram->setUniformLocationWith1i(loc, 4);
    GL::bindTexture2DN(4, _textureBOSS->getName());

    loc = glProgram->getUniformLocation("u_texture_low_blood");
    glProgram->setUniformLocationWith1i(loc, 5);
    GL::bindTexture2DN(5, _textureLowBlood->getName());

    loc = glProgram->getUniformLocation("u_para_action_scope_line");
    glProgram->setUniformLocationWith4fv(loc, &_paraActionScopeLine.x, 1);
    loc = glProgram->getUniformLocation("u_para_placement");
    glProgram->setUniformLocationWith4fv(loc, &_paraPlacement.x, 1);
    loc = glProgram->getUniformLocation("u_para_selectioin_boss_lowblood");
    glProgram->setUniformLocationWith4fv(loc, &_paraSelectionBossLowBlood.x, 1);
//    loc = glProgram->getUniformLocation("u_height");
//    glProgram->setUniformLocationWith1f(loc, _height);

    loc = glProgram->getUniformLocation("u_lights");
    glProgram->setUniformLocationWith4fv(loc, &_lights[0].x, DDConfig::NUM_LIGHT);

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

void BattleMiddleNode::configScopeLine(const cocos2d::Vec4& opacitys)
{
    _paraActionScopeLine = opacitys;

}

void BattleMiddleNode::configPlacement(float opacity, bool isOk)
{
    static Vec4 colorNo = {1.f,0.f,0.f,1.f};
    static Vec4 colorYes = {0.f,1.f,0.f,1.f};
    _paraPlacement = isOk?colorYes:colorNo;
    _paraPlacement.w = opacity;
}
void BattleMiddleNode::configSelection(float opacity)
{
    _paraSelectionBossLowBlood.x = opacity;
}
void BattleMiddleNode::configBoss(float opacity)
{
    _paraSelectionBossLowBlood.y = opacity;
}
void BattleMiddleNode::configLowBlood(float opacity)
{
    _paraSelectionBossLowBlood.z = opacity;
}

void BattleMiddleNode::configShadowFile(const std::string& shadowFile)
{
    _textureShadow = Director::getInstance()->getTextureCache()->addImage(shadowFile);
}

void BattleMiddleNode::updateLights(LightNode* lights)
{
    auto myAgentFloatPos = BattleField::help_battleLayerPos2agentFloatPos(this->getPosition());
    for (int i = 0; i < DDConfig::NUM_LIGHT; i++) {
        Vec2 relativeAgentFloatPos = myAgentFloatPos - lights[i].agentFloatPos;;
        this->_lights[i] = {relativeAgentFloatPos.x, relativeAgentFloatPos.y, lights[i].quality, lights[i].height};
    }
}