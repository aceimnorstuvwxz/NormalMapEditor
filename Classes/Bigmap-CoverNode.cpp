// (C) 2015 Turnro.com

#include "Bigmap.h"
#include "DDMapData.h"


USING_NS_CC;

bool BigmapCoverNode::init()
{
    assert(Node::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }
    return true;
}


void BigmapCoverNode::prepareVertexData()
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

void BigmapCoverNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/bigmap_cover.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/bigmap_cover.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);
    glprogram->bindAttribLocation("a_color", 1);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}


void BigmapCoverNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(BigmapCoverNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void BigmapCoverNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
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
    glDrawArrays(GL_POINTS, 0, _count); //描边层


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

cocos2d::Vec2 help_calcMinmapPixelPosInBigmap(const MapPos& mappos)
{
    float x = (mappos.x)*((DDConfig::MINMAP_POS_EXPAND*2+1)*(DDConfig::MINMAP_EXPAND*2+1)+2*DDConfig::MINMAP_POS_OFFSET);
    float y = (mappos.y)*((DDConfig::MINMAP_POS_EXPAND*2+1)*(DDConfig::MINMAP_EXPAND*2+1)+2*DDConfig::MINMAP_POS_OFFSET);
    return {x,y};
}

cocos2d::Vec4 help_getClorOfCover(int minmapState)
{
    Vec4 c;
    switch (minmapState) {

        case DDMinMap::T_ACTIVE:
            c = {0,0,0,0};
            break;
        case DDMinMap::T_ACTIVABLE:
            c =  {0,0,0,30};
            break;
        case DDMinMap::T_NON_ACTIVE:
            c = {0,0,0,128};
            break;

        default:
            assert(false);
            break;
    }
    return c * (1.0/255);
}

void BigmapCoverNode::refreshVertexData()
{
    _count = 0;
    for (int x = -DDConfig::BIGMAP_X_EXPAND; x < DDConfig::BIGMAP_X_EXPAND+1; x++) {
        for (int y = -DDConfig::BIGMAP_Y_EXPAND; y < DDConfig::BIGMAP_Y_EXPAND+1; y++) {
            int state = DDMinMap::T_NON_ACTIVE;
            if (DDMapData::s()->getMinMaps().find({x,y}) != DDMapData::s()->getMinMaps().end()) {
                state = DDMapData::s()->getMinMaps().find({x,y})->second->state;
            }
            _vertexData[_count] = {help_calcMinmapPixelPosInBigmap({x,y}), help_getClorOfCover(state)};
            _count++;
        }
    }
}

