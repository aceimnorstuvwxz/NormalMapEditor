// (C) 2015 ScatOrc

#include "EditorScene.h"

USING_NS_CC;
bool EELinesNode::init()
{
    assert(Node::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    return true;
}

void EELinesNode::prepareVertexData()
{

    _count = 0;
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(EELinesNodeVertexFormat)*_count, _vertexData, GL_STREAM_DRAW);
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(EELinesNodeVertexFormat), (GLvoid *)offsetof(EELinesNodeVertexFormat, position));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(EELinesNodeVertexFormat)*_count, _vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    CHECK_GL_ERROR_DEBUG();
}

void EELinesNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/lines.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/lines.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}



void EELinesNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(EELinesNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void EELinesNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();
/*
    auto loc = glProgram->getUniformLocation("u_alpha");
    glProgram->setUniformLocationWith1f(loc, this->getOpacity() * (1.f/255));
    loc = glProgram->getUniformLocation("u_width");
    glProgram->setUniformLocationWith1f(loc, DDConfig::relativeScaler(1.0) * (750/DDConfig::BATTLE_NUM));
    loc = glProgram->getUniformLocation("u_color");
    glProgram->setUniformLocationWith4fv(loc, &(_color.x), 1);
 */
    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (_dirty) {
        _dirty = false;
        glBufferData(GL_ARRAY_BUFFER, sizeof(EELinesNodeVertexFormat)*_count, _vertexData, GL_STREAM_DRAW);
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


    glDrawArrays(GL_LINES, 0, _count);

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

void EELinesNode::configLines(const std::list<std::shared_ptr<EELine>>& lines)
{
    _count = 0;
    _dirty = true;
    for (auto line : lines) {
        auto src = EditorScene::help_relativePosition2editPosition(line->a->position);
        auto des = EditorScene::help_relativePosition2editPosition(line->b->position);
        _vertexData[_count++].position = src;
        _vertexData[_count++].position = des;
    }
}
