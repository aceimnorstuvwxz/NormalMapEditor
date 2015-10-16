// (C) 2015 Turnro.com

#include "BattleField.h"
#include "DDConfig.h"

USING_NS_CC;

bool RedGreenCover::init()
{
    assert(Node::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    return true;
}

inline int help_agentPos2arrawIndex(const AgentPos& pos)
{
    int y = pos.y + DDConfig::BATTLE_NUM/2;
    int x = pos.x + DDConfig::BATTLE_NUM/2;
    return (y * DDConfig::BATTLE_NUM + x );
}

void RedGreenCover::prepareVertexData()
{

    for (int y = -DDConfig::MINMAP_EXPAND; y <= DDConfig::MINMAP_EXPAND; y++) {
        for (int x = -DDConfig::MINMAP_EXPAND; x <= DDConfig::MINMAP_EXPAND; x++){
            AgentPos agentPos = {x, y};
            RedGreenCoverVertexFormat* pdata = &_vertexData[help_agentPos2arrawIndex(agentPos)];
            Vec2 pos = BattleField::help_agentPos2agentFloatPos(agentPos);
            pdata->position = pos;
            pdata->state = 0;
        }
    }
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(RedGreenCoverVertexFormat)*DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM, _vertexData, GL_STREAM_DRAW);
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RedGreenCoverVertexFormat), (GLvoid *)offsetof(RedGreenCoverVertexFormat, position));

        // state
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(RedGreenCoverVertexFormat), (GLvoid *)offsetof(RedGreenCoverVertexFormat, state));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(RedGreenCoverVertexFormat)*DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM, _vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();
}

void RedGreenCover::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/redgreen_cover.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/redgreen_cover.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);
    glprogram->bindAttribLocation("a_state", 1);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}



void RedGreenCover::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(RedGreenCover::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void RedGreenCover::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();

    auto loc = glProgram->getUniformLocation("u_alpha");
    glProgram->setUniformLocationWith1f(loc, this->getOpacity() * (1.f/255));
    loc = glProgram->getUniformLocation("u_width");
    glProgram->setUniformLocationWith1f(loc, DDConfig::relativeScaler(1.0) * (750/DDConfig::BATTLE_NUM));
    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (_dirty) {
        _dirty = false;
        glBufferData(GL_ARRAY_BUFFER, sizeof(RedGreenCoverVertexFormat)*DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM, _vertexData, GL_STREAM_DRAW);
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
//    glEnable(GL_DEPTH_TEST);
//    glDepthMask(true);


    glDrawArrays(GL_POINTS, 0, DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,DDConfig::BATTLE_NUM*DDConfig::BATTLE_NUM);
    CHECK_GL_ERROR_DEBUG();
}

void RedGreenCover::configCover(const AgentPos& pos, int state)
{
    _vertexData[help_agentPos2arrawIndex(pos)].state = state;
    _dirty = true;
}


void RedGreenCover::show()
{
    this->setVisible(true);
    this->setOpacity(0);
    this->runAction(FadeIn::create(0.25));
}

void RedGreenCover::dismiss()
{
    this->setOpacity(0);
    this->runAction(Sequence::create(FadeOut::create(0.25), Hide::create(), nullptr));
}

void RedGreenCover::step(float dt)
{
    /*
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
    }*/
}