// (C) 2015 Turnro.com

#include "BattleField.h"
#include "DDConfig.h"

USING_NS_CC;
GLuint BattleHighNode::_vao;
GLuint BattleHighNode::_vbo;
cocos2d::GLProgramState* BattleHighNode::_programState = nullptr;

bool BattleHighNode::init()
{
    assert(BattleNode::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    _textureBeAttacked = Director::getInstance()->getTextureCache()->addImage("images/high_be_attacked.png");
    _textureBeCured = Director::getInstance()->getTextureCache()->addImage("images/high_be_cured.png");

    for (int i = 0; i < DDConfig::NUM_LIGHT; i++) {
        _lights[i] = {0,0,0,0};
        _lightsColor[i] = {1,1,1,1};
    }

    return true;
}

void BattleHighNode::prepareVertexData()
{
    V3F_T2F vertexData[6];
    float halfWidth = DDConfig::battleCubeWidth()*(1 + 2.f * DDConfig::EXPAND_NORMAL); //high层是拓展的，因为部分元素(比如一些光圈)会渲染到其它cube的空间内。//TODO 这个HalfWidth莫名奇妙必须是正确值的2倍！！
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

void BattleHighNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/battle_high.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/battle_high.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", MY_VERTEX_ATTRIB_POSITION);
    glprogram->bindAttribLocation("a_texcoord", MY_VERTEX_ATTRIB_TEXCOORD);
    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}



void BattleHighNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(BattleHighNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void BattleHighNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
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
    auto loc = glProgram->getUniformLocation("u_texture_building_animation");
    glProgram->setUniformLocationWith1i(loc, 0);
    GL::bindTexture2DN(0, _textureBuildingAnimation->getName());

    loc = glProgram->getUniformLocation("u_texture_building_normalmap");
    glProgram->setUniformLocationWith1i(loc, 1);
    GL::bindTexture2DN(1, _textureBuildingNormalMap->getName());

    loc = glProgram->getUniformLocation("u_texture_be_attacked");
    glProgram->setUniformLocationWith1i(loc, 2);
    GL::bindTexture2DN(2, _textureBeAttacked->getName());

    loc = glProgram->getUniformLocation("u_texture_be_cured");
    glProgram->setUniformLocationWith1i(loc, 3);
    GL::bindTexture2DN(3, _textureBeCured->getName());

    loc = glProgram->getUniformLocation("u_para_ambient_color");
    glProgram->setUniformLocationWith4fv(loc, &_paraAmbientColor.x, 1);
    loc = glProgram->getUniformLocation("u_para_attacked_cured");
    glProgram->setUniformLocationWith4fv(loc, &_paraAttackedCured.x, 1);

    loc = glProgram->getUniformLocation("u_lights");
    glProgram->setUniformLocationWith4fv(loc, &_lights[0].x, DDConfig::NUM_LIGHT);
    loc = glProgram->getUniformLocation("u_lights_color");
    glProgram->setUniformLocationWith4fv(loc, &_lightsColor[0].x, DDConfig::NUM_LIGHT);

    loc = glProgram->getUniformLocation("u_building_animation_index");
    glProgram->setUniformLocationWith1i(loc, _buildingAnimationIndex);

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

void BattleHighNode::updateLights(LightNode* lights)
{
    auto myAgentFloatPos = BattleField::help_battleLayerPos2agentFloatPos(this->getPosition());
    for (int i = 0; i < DDConfig::NUM_LIGHT; i++) {
        Vec2 relativeAgentFloatPos = myAgentFloatPos - lights[i].agentFloatPos;;
        this->_lights[i] = {relativeAgentFloatPos.x, relativeAgentFloatPos.y, lights[i].quality, lights[i].height};
        this->_lightsColor[i] = lights[i].color;
    }
}

void BattleHighNode::configBuildingAnimation(const std::string& buildingFile)
{
    _textureBuildingAnimation = Director::getInstance()->getTextureCache()->addImage(buildingFile);
}
void BattleHighNode::configBuildingNormalMap(const std::string& buildingNormalMap)
{
    _textureBuildingNormalMap = Director::getInstance()->getTextureCache()->addImage(buildingNormalMap);
}
void BattleHighNode::configBuildingAnimationIndex(int index)
{
    _buildingAnimationIndex = index;
}
void BattleHighNode::configBeAttacked(float opacity)
{
    _paraAttackedCured.x = opacity;
}
void BattleHighNode::configBeCured(float opacity)
{
    _paraAttackedCured.y = opacity;
}
void BattleHighNode::configAmbientColor(const cocos2d::Vec4& color)
{
    _paraAmbientColor = color;
}