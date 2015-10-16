// (C) 2015 Turnro.com

#include "BattleField.h"
#include "DDConfig.h"

USING_NS_CC;

bool ShadowCover::init()
{
    assert(Node::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    _textureShadow = Director::getInstance()->getTextureCache()->addImage("images/shadow_cover.png");

    return true;
}

inline int help_agentPos2arrawIndex(const AgentPos& pos)
{
    int y = pos.y + DDConfig::BATTLE_NUM/2;
    int x = pos.x + DDConfig::BATTLE_NUM/2;
    return (y * DDConfig::BATTLE_NUM + x );
}

void ShadowCover::prepareVertexData()
{

    for (int i = 0; i < DDConfig::BATTLE_NUM; i++) {
        for (int j = 0; j < DDConfig::BATTLE_NUM;j++){
            ShadowCoverVertexFormat* pdata = &_vertexData[(i*DDConfig::BATTLE_NUM+j)*6];
            AgentPos agentPos = {j-DDConfig::BATTLE_NUM/2, i-DDConfig::BATTLE_NUM/2};
            Vec2 pos = BattleField::help_agentPos2agentFloatPos(agentPos);
            float hw = DDConfig::battleCubeWidth() * COVER_SCALER * 0.5f;
            Vec2 tl = {pos.x - hw, pos.y + hw};
            Vec2 tr = {pos.x + hw, pos.y + hw};
            Vec2 bl = {pos.x - hw, pos.y - hw};
            Vec2 br = {pos.x + hw, pos.y - hw};
            pdata[0].position = tr;
            pdata[0].texcoord = {1,0};
            pdata[1].position = tl;
            pdata[1].texcoord = {0,0};
            pdata[2].position = bl;
            pdata[2].texcoord = {0,1};
            pdata[3].position = tr;
            pdata[3].texcoord = {1,0};
            pdata[4].position = bl;
            pdata[4].texcoord = {0,1};
            pdata[5].position = br;
            pdata[5].texcoord = {1,1};
            for (int k = 0; k < 6; k++) {
                pdata[k].radio = 1;
            }
        }
    }
    for (int i = 0; i < DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM; i++) {
        _cubeAniDir[i] = 0.f;
    }

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowCoverVertexFormat)*DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6, _vertexData, GL_STREAM_DRAW);
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ShadowCoverVertexFormat), (GLvoid *)offsetof(ShadowCoverVertexFormat, position));

        // texcoord
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ShadowCoverVertexFormat), (GLvoid *)offsetof(ShadowCoverVertexFormat, texcoord));

        // radio
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ShadowCoverVertexFormat), (GLvoid *)offsetof(ShadowCoverVertexFormat, radio));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowCoverVertexFormat)*DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6, _vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();
}

void ShadowCover::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/shadow_cover.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/shadow_cover.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);
    glprogram->bindAttribLocation("a_texcoord", 1);
    glprogram->bindAttribLocation("a_radio", 2);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}



void ShadowCover::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(ShadowCover::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void ShadowCover::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
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
/*
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
*/
    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (_dirty) {
        _dirty = false;
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShadowCoverVertexFormat)*DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6, _vertexData, GL_STREAM_DRAW);
    }
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
    // shadow cover打开depth test同时在fsh中对a为0的进行discard，以保证重合交叠处不会交叠而加深。
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);


    glDrawArrays(GL_TRIANGLES, 0, DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6);
    CHECK_GL_ERROR_DEBUG();
}

void ShadowCover::coverAll()
{
    _dirty = true;
    for (int i = 0; i < DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM*6; i++) {
        _vertexData[i].radio = 1;
    }
}

void ShadowCover::setRadio(const AgentPos& pos, float radio)
{
    _dirty = true;
    int baseIndex = help_agentPos2arrawIndex(pos);
    for (int i = 0; i < 6; i++) {
        _vertexData[6*baseIndex+i].radio = radio;
    }
}

float ShadowCover::getRadio(const AgentPos& pos)
{
    int baseIndex = help_agentPos2arrawIndex(pos);
    return _vertexData[6*baseIndex].radio;
}

void ShadowCover::reveal(const AgentPos& pos)
{
    setRadio(pos, 0);
}

void ShadowCover::cover(const AgentPos& pos)
{
    setRadio(pos, 1);
}

void ShadowCover::aniReveal(const AgentPos& pos)
{
    _cubeAniDir[help_agentPos2arrawIndex(pos)] = -1;

}
void ShadowCover::aniCover(const AgentPos& pos)
{
    _cubeAniDir[help_agentPos2arrawIndex(pos)] = 1;
}
void ShadowCover::step(float dt)
{
    for (int i = 0; i < DDConfig::BATTLE_NUM; i++) {
        for (int j = 0; j < DDConfig::BATTLE_NUM; j++) {
            float dir =  _cubeAniDir[i*DDConfig::BATTLE_NUM+j];
            if (dir == 0.f) continue;
            AgentPos pos = {j-DDConfig::BATTLE_NUM/2, i-DDConfig::BATTLE_NUM/2};
            float oldRadio = getRadio(pos);
            if (dir < 0 && oldRadio > 0.f){
                float newRadio = std::max(0.f, oldRadio - ANI_STEP_PER_SEC*dt);
                if (newRadio == 0.f) {
                    _cubeAniDir[i*DDConfig::BATTLE_NUM+j] = 0;
                }
                setRadio(pos, newRadio);
            } else if (dir >0 && oldRadio < 1.f) {
                float newRadio = std::min(1.f, oldRadio + ANI_STEP_PER_SEC*dt);
                if (newRadio == 1.f) {
                    _cubeAniDir[i*DDConfig::BATTLE_NUM+j] = 0;
                }
                setRadio(pos, newRadio);
            }
        }
    }
}