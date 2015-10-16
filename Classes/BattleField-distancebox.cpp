// (C) 2015 Turnro.com

#include "BattleField.h"
#include "DDConfig.h"

USING_NS_CC;

bool DistanceBox::init()
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

void DistanceBox::prepareVertexData()
{

    _count = 0;
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DistanceBoxVertexPormat)*_count, _vertexData, GL_STREAM_DRAW);
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DistanceBoxVertexPormat), (GLvoid *)offsetof(DistanceBoxVertexPormat, position));

        // edge
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(DistanceBoxVertexPormat), (GLvoid *)offsetof(DistanceBoxVertexPormat, edge));

        // corner
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(DistanceBoxVertexPormat), (GLvoid *)offsetof(DistanceBoxVertexPormat, corner));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(DistanceBoxVertexPormat)*_count, _vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();
}

void DistanceBox::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/distance_box.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/distance_box.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);
    glprogram->bindAttribLocation("a_edge", 1);
    glprogram->bindAttribLocation("a_corner", 2);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}



void DistanceBox::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(DistanceBox::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void DistanceBox::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();

    auto loc = glProgram->getUniformLocation("u_alpha");
    glProgram->setUniformLocationWith1f(loc, this->getOpacity() * (1.f/255));
    loc = glProgram->getUniformLocation("u_width");
    glProgram->setUniformLocationWith1f(loc, DDConfig::relativeScaler(1.0) * (750/DDConfig::BATTLE_NUM));
    loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &(_color.x), 1);
    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (_dirty) {
        _dirty = false;
        glBufferData(GL_ARRAY_BUFFER, sizeof(DistanceBoxVertexPormat)*_count, _vertexData, GL_STREAM_DRAW);
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


    glDrawArrays(GL_POINTS, 0, _count);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_count);
    CHECK_GL_ERROR_DEBUG();
}

void DistanceBox::configDistance(int distance)
{
    // check
    if (_distance == distance) return;


    // 计算和填充
    _dirty = true;
    _count = 0;
    _distance = distance;
    for (int x = -distance; x <= distance; x++) {
        for (int y = -distance; y <= distance; y++) {
            AgentPos agentpos = {x,y};
            int mydistance = agentpos.distance({0,0});
            if (mydistance == distance) {
                _vertexData[_count].position = BattleField::help_agentPos2agentFloatPos(agentpos);
                _vertexData[_count].edge = {0,0,0,0};
                _vertexData[_count].corner = {0,0,0,0};
                // 外围
                if (y >= 0) {
                    _vertexData[_count].edge.x = 1;
                }
                if (y <= 0) {
                    _vertexData[_count].edge.z = 1;
                }
                if (x >= 0) {
                    _vertexData[_count].edge.y = 1;
                }
                if (x <= 0) {
                    _vertexData[_count].edge.w = 1;
                }
                _count++;
            } else if (mydistance == distance-1) {
                // 内圈
                _vertexData[_count].position = BattleField::help_agentPos2agentFloatPos(agentpos);
                _vertexData[_count].edge = {0,0,0,0};
                _vertexData[_count].corner = {0,0,0,0};
                if (x >= 0 && y >= 0) {
                    _vertexData[_count].corner.x = 1;
                }
                if (x >= 0 && y <= 0) {
                    _vertexData[_count].corner.y = 1;
                }
                if (x <= 0 && y <= 0) {
                    _vertexData[_count].corner.z = 1;
                }
                if (x <= 0 && y >= 0) {
                    _vertexData[_count].corner.w = 1;
                }
                _count++;
            }
        }
    }
}

void DistanceBox::configCenter(const AgentPos& center, bool isok)
{
    this->setPosition(BattleField::help_agentPos2agentFloatPos(center));
    _color = isok ? COLOR_GOOD:COLOR_BAD;
}

void DistanceBox::show()
{
    this->setVisible(true);
    this->setOpacity(0);
    this->runAction(FadeIn::create(0.25));
}

void DistanceBox::dismiss()
{
    this->setOpacity(0);
    this->runAction(Sequence::create(FadeOut::create(0.25), Hide::create(), nullptr));
}

void DistanceBox::step(float dt)
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