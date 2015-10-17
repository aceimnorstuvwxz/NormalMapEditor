// (C) 2015 ScatOrc

#include "EditorScene.h"

USING_NS_CC;
bool EETrianglesNode::init()
{
    assert(Node::init());

    if (_programState == nullptr) {
        prepareVertexData();
        prepareShaders();
    }

    return true;
}

void EETrianglesNode::prepareVertexData()
{
    _count = 0;
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(EETrianglesNodeVertexFormat)*_count, _vertexData, GL_STREAM_DRAW);
        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(EETrianglesNodeVertexFormat), (GLvoid *)offsetof(EETrianglesNodeVertexFormat, position));

        // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(EETrianglesNodeVertexFormat), (GLvoid *)offsetof(EETrianglesNodeVertexFormat, normal));

        // color
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(EETrianglesNodeVertexFormat), (GLvoid *)offsetof(EETrianglesNodeVertexFormat, color));

        GL::bindVAO(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(EETrianglesNodeVertexFormat)*_count, _vertexData, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    CHECK_GL_ERROR_DEBUG();
}

void EETrianglesNode::prepareShaders()
{
    auto fileUtils = FileUtils::getInstance();
    auto vertSource = fileUtils->getStringFromFile("3d/triangles.vsh");
    auto fragSource = fileUtils->getStringFromFile("3d/triangles.fsh");

    auto glprogram = GLProgram::createWithByteArrays(vertSource.c_str(), fragSource.c_str());
    glprogram->bindAttribLocation("a_positioin", 0);
    glprogram->bindAttribLocation("a_normal2", 1);
    glprogram->bindAttribLocation("a_color2", 2);

    glprogram->link();
    glprogram->updateUniforms();

    _programState = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _programState->retain();
}

void EETrianglesNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    _command.init(_globalZOrder, transform, flags);
    _command.func = CC_CALLBACK_0(EETrianglesNode::onDraw, this, transform, flags);
    renderer->addCommand(&_command);
}

void EETrianglesNode::onDraw(const cocos2d::Mat4 &transform, uint32_t flags)
{

    auto glProgram = _programState->getGLProgram();
    glProgram->use();
    auto loc = glProgram->getUniformLocation("u_show_state");
    glProgram->setUniformLocationWith1i(loc, _showState);
    loc = glProgram->getUniformLocation("u_light_position");
    glProgram->setUniformLocationWith2fv(loc, &(_lightPos.x), 1);
//    CCLOG("%f %f", _lightPos.x, _lightPos.y);

    glProgram->setUniformsForBuiltins(transform);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    if (_dirty) {
        _dirty = false;
        glBufferData(GL_ARRAY_BUFFER, sizeof(EETrianglesNodeVertexFormat)*_count, _vertexData, GL_STREAM_DRAW);
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

    glDrawArrays(GL_TRIANGLES, 0, _count);

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

Vec3 calcTriangleNormal(Vec3 a, Vec3 b, Vec3 c)
{
    /*
    Begin Function CalculateSurfaceNormal (Input Triangle) Returns Vector

    Set Vector U to (Triangle.p2 minus Triangle.p1)
    Set Vector V to (Triangle.p3 minus Triangle.p1)

    Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
    Set Normal.y to (multiply U.z by V.x) minus (multiply U.x by V.z)
    Set Normal.z to (multiply U.x by V.y) minus (multiply U.y by V.x)

    Returning Normal
    
    End Function
     */
    Vec3 U = b - a;
    Vec3 V = c - a;
    Vec3 N;
    N.x = U.y * V.z - U.z * V.y;
    N.y = U.z * V.x - U.x * V.z;
    N.z = U.x * V.y - U.y * V.x;

    if (N.dot({0,0,1}) < 0) {
        N = -N;
    }
    return N;
}

void EETrianglesNode::configTriangles(const std::list<std::shared_ptr<EETriangle>>& triangles)
{
    _count = 0;
    _dirty = true;
    for (auto triangle : triangles) {
        // normal
        auto normal = calcTriangleNormal(triangle->a->pos3d(), triangle->b->pos3d(), triangle->c->pos3d());
        CCLOG("normal %f %f %f", normal.x, normal.y, normal.z);


        _vertexData[_count].position = EditorScene::help_relativePosition2editPosition(triangle->a->position);
        _vertexData[_count].color = triangle->color;
        _vertexData[_count].normal = normal;
        _count++;
        _vertexData[_count].position = EditorScene::help_relativePosition2editPosition(triangle->b->position);
        _vertexData[_count].color = triangle->color;
        _vertexData[_count].normal = normal;
        _count++;
        _vertexData[_count].position = EditorScene::help_relativePosition2editPosition(triangle->c->position);
        _vertexData[_count].color = triangle->color;
        _vertexData[_count].normal = normal;
        _count++;
    }
}

/*
void EETrianglesNode::configTriangles2(EETriangle2* tirangles, int num)
{
    _count = 0;
    _dirty = true;
    for (int i = 0; i < num; i++) {
        // normal
        auto normal = Vec3{0,0,1};

        float cc = rand_0_1();
        Vec4 col = {cc, cc,cc, 1.0};

        _vertexData[_count].position = EditorScene::help_relativePosition2editPosition(tirangles[i].a);
        _vertexData[_count].color = col;//{0.5,0.5,0.5,1.0};
        _vertexData[_count].normal = normal;
        _count++;
        _vertexData[_count].position = EditorScene::help_relativePosition2editPosition(tirangles[i].b);
        _vertexData[_count].color = col;//{0.5,0.5,0.5,1.0};
        _vertexData[_count].normal = normal;
        _count++;
        _vertexData[_count].position = EditorScene::help_relativePosition2editPosition(tirangles[i].c);
        _vertexData[_count].color = col;//{0.5,0.5,0.5,1.0};
        _vertexData[_count].normal = normal;
        _count++;
    }
}*/

void EETrianglesNode::updateLightPos(cocos2d::Vec2 pos)
{
    _lightPos = pos;
}
