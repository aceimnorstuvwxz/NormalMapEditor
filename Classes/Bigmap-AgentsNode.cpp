// (C) 2015 Turnro.com

#include "Bigmap.h"
#include "DDMapData.h"


USING_NS_CC;

bool BigmapAgentsNode::init()
{
    assert(Node::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }
    return true;
}


void BigmapAgentsNode::prepareVertexData()
{
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BigmapNodeFormat)*_count, _vertexData, GL_STREAM_DRAW);
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(BigmapNodeFormat), (GLvoid *)offsetof(BigmapNodeFormat, position));

        // color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(BigmapNodeFormat), (GLvoid *)offsetof(BigmapNodeFormat, color));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(BigmapNodeFormat)*_count, _vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    CHECK_GL_ERROR_DEBUG();
}

void BigmapAgentsNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/bigmap_agents.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/bigmap_agents.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);
    glprogram->bindAttribLocation("a_color", 1);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}


void BigmapAgentsNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(BigmapAgentsNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void BigmapAgentsNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();

    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (_dirty) {
        _dirty = false;
        refreshVertexData();
        glBufferData(GL_ARRAY_BUFFER, sizeof(BigmapNodeFormat)*_count, _vertexData, GL_STREAM_DRAW);
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
//    glEnable(GL_DEPTH_TEST);
//    glDepthMask(true);


    auto loc = glProgram->getUniformLocation("u_scaler");
    glProgram->setUniformLocationWith1f(loc, DDConfig::bigmapCubePixelScaler());
    loc = glProgram->getUniformLocation("u_is_stroke");
    glProgram->setUniformLocationWith1i(loc, 1);
    glDrawArrays(GL_POINTS, 0, _count); //描边层
    glProgram->setUniformLocationWith1i(loc, 0);
    glDrawArrays(GL_POINTS, 0, _count); //内容层


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

cocos2d::Vec2 help_calcAgentPixelPosInBigmap(const MapPos& mappos, const AgentPos& agentpos)
{
    float x = (mappos.x)*((DDConfig::MINMAP_POS_EXPAND*2+1)*(DDConfig::MINMAP_EXPAND*2+1)+2*DDConfig::MINMAP_POS_OFFSET) + (agentpos.x)*(DDConfig::MINMAP_POS_EXPAND*2+1);
    float y = (mappos.y)*((DDConfig::MINMAP_POS_EXPAND*2+1)*(DDConfig::MINMAP_EXPAND*2+1)+2*DDConfig::MINMAP_POS_OFFSET) + (agentpos.y)*(DDConfig::MINMAP_POS_EXPAND*2+1);
    return {x,y};
}

cocos2d::Vec4 help_getClorOfAgent(int agentType)
{
    Vec4 c;
    switch (agentType) {
        case DDAgent::AT_3RD_MINE:
            c =  {135,213,226,255};
            break;
        case DDAgent::AT_3RD_STONE:
            c =  {102,102,102,255};
            break;
        case DDAgent::AT_3RD_TREE:
            c =  {000,102,000,255};
            break;
        case DDAgent::AT_3RD_VOLCANO:
            c =  {102,000,000,255};
            break;
        case DDAgent::AT_3RD_WATER:
            c =  {000,051,153,255};
            break;
        case DDAgent::AT_FRIEND_CORE:
            c =  {100,255,100,255};
            break;
        case DDAgent::AT_FRIEND_ARROW_TOWER:
        case DDAgent::AT_FRIEND_CONNON_TOWER:
        case DDAgent::AT_FRIEND_CURE_TOWER:
        case DDAgent::AT_FRIEND_LIGHT_TOWER:
        case DDAgent::AT_FRIEND_MAGIC_TOWER:
        case DDAgent::AT_FRIEND_MINER:
        case DDAgent::AT_FRIEND_WALL:
            c =  {000,255,000,255};
            break;

        case DDAgent::AT_ENEMY_NEST:
        case DDAgent::AT_ENEMY_FAR:
        case DDAgent::AT_ENEMY_NEAR:
            c =  {255,000,000,255};
            break;

        default:
            assert(false);
            break;
    }
    return c * (1.0/255);
}

void BigmapAgentsNode::refreshVertexData()
{
    _count = 0;
    for (auto mapposMinmap : DDMapData::s()->getMinMaps()) {
        if (mapposMinmap.second->blocked || mapposMinmap.second->state != DDMinMap::T_ACTIVE) {
            continue;
        }
        // TODO 仅渲染可见的部分agents
        for (auto agentposMinmap : mapposMinmap.second->posAgentMap) {
            _vertexData[_count] = {help_calcAgentPixelPosInBigmap(mapposMinmap.first, agentposMinmap.first), help_getClorOfAgent(agentposMinmap.second->type)};
            _count++;
        }
    }
}

