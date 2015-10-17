// (C) 2015 Turnro.com

#include "EditorScene.h"
#include "TRLocale.h"
#include "format.h"
#include "intersection.h"

USING_NS_CC;

bool EditorScene::init()
{
    assert(TRBaseScene::init());

    _layer = Layer::create();
    this->addChild(_layer);

    _presentingLayer = Layer::create();
    _presentingLayer->setPosition({512,256+512});
    this->addChild(_presentingLayer);
    
    _pointLayer = Layer::create();
    _pointLayer->setPosition({512,256+512});
    this->addChild(_pointLayer);

    _rulerBg = Sprite::create("images/ruler.png");
    _rulerBg->setOpacity(128);
    _presentingLayer->addChild(_rulerBg);



    _diggColorPanel = Sprite::create("images/dfdg.png");
    _diggColorPanel->setPosition({512,256/2});
    _layer->addChild(_diggColorPanel);

    _diggColorLabel = Label::createWithTTF("color", "fonts/fz.ttf", 25);
    _diggColorLabel->setPosition({512, 256*0.77});
    _layer->addChild(_diggColorLabel);

    initLinesThings();
    initTrianglesThings();
    initKeyboardMouse();
    addTestLights();

    addCommonBtn({0.1,0.1}, "test tri", [this](){

        delaunay();

    });

    return true;
}

cocos2d::Vec2 EditorScene::help_touchPoint2editPosition(const cocos2d::Vec2& touchpoint)
{
    auto size = Director::getInstance()->getWinSize();
    return {touchpoint.x - size.width/2, touchpoint.y - 1024/2 - 256};
}

void EditorScene::initLinesThings()
{
    _linesNode = EELinesNode::create();
    _linesNode->setPosition({0,0});
    _pointLayer->addChild(_linesNode);
    _linesNode->setZOrder(Z_LINES);
}

void EditorScene::initTrianglesThings()
{
    _trianglesNode = EETrianglesNode::create();
    _trianglesNode->setPosition({0,0});
    _presentingLayer->addChild(_trianglesNode);
    _trianglesNode->setZOrder(Z_TRIANGLES);
}

void EditorScene::initKeyboardMouse()
{
    auto _keyboardListener = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed = [&](EventKeyboard::KeyCode code, Event* event){
        switch (code) {
            case EventKeyboard::KeyCode::KEY_SPACE:
                break;

            case EventKeyboard::KeyCode::KEY_R:
                _ks_addPoint = true;
                break;
            case EventKeyboard::KeyCode::KEY_T:
                _ks_deletePoint = true;
                break;
            case EventKeyboard::KeyCode::KEY_A:
                _ks_selection = true;
                break;
            case EventKeyboard::KeyCode::KEY_D:
                clearSelection();
                break;
            case EventKeyboard::KeyCode::KEY_W:
                moveUp(true);
                break;
            case EventKeyboard::KeyCode::KEY_S:
                moveUp(false);
                break;
            case EventKeyboard::KeyCode::KEY_F:
                _ks_shading = true;
                break;
            case EventKeyboard::KeyCode::KEY_G:
                _ks_digging = true;
                break;
            case EventKeyboard::KeyCode::KEY_P:
                _pointLayer->setVisible(!_pointLayer->isVisible());
                _rulerBg->setVisible(!_rulerBg->isVisible());
                break;
            case EventKeyboard::KeyCode::KEY_Y:
                this->delaunay();
                this->refreshLines();
                this->refreshTriangles();
                break;

            default:
                break;
        }
    };
    _keyboardListener->onKeyReleased = [&](EventKeyboard::KeyCode code, Event* event){
        switch (code) {
            case EventKeyboard::KeyCode::KEY_SPACE:
                break;

            case EventKeyboard::KeyCode::KEY_R:
                _ks_addPoint = false;
                break;
            case EventKeyboard::KeyCode::KEY_T:
                _ks_deletePoint = false;
                break;
            case EventKeyboard::KeyCode::KEY_A:
                _ks_selection = false;
                break;
            case EventKeyboard::KeyCode::KEY_F:
                _ks_shading = false;
                break;
            case EventKeyboard::KeyCode::KEY_G:
                _ks_digging = false;
                break;
            default:
                break;
        }
        
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        auto rawpos = touch->getLocation();
        CCLOG("click in at %f %f", rawpos.x, rawpos.y);

        if (rawpos.y < 256) {
            diggColor(rawpos);

            return false;
        } else {

            if (_ks_addPoint) {
                addPoint(rawpos);
            } else if (_ks_deletePoint) {
                deletePoint(rawpos);
            } else if (_ks_selection) {
                selectPoint(rawpos, true);
            } else if (_ks_shading) {
                shadingTriangle(rawpos);
            } else if (_ks_digging) {
                diggColor(rawpos);
            } else {
                selectPoint(rawpos, false);
            }
        }

        return _selectedPoints.size() > 0;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){

        auto move = touch->getDelta() ;//* 0.25;
        for (auto point : _selectedPoints) {
            point->sprite->setPosition(point->sprite->getPosition() + move);
            point->position = help_editPosition2relativePosition(point->sprite->getPosition());
        }
        this->delaunay();
        this->refreshLines();
        this->refreshTriangles();

    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _pointLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _pointLayer);

}

cocos2d::Vec2 EditorScene::help_editPosition2relativePosition(const cocos2d::Vec2& editposition)
{
    auto size = Director::getInstance()->getWinSize();
    return {editposition.x / (size.width/2), editposition.y / (size.width/2)};
}
cocos2d::Vec2 EditorScene::help_relativePosition2editPosition(const cocos2d::Vec2& relativePosition)
{
    auto size = Director::getInstance()->getWinSize();
    return {relativePosition.x * size.width/2, relativePosition.y * size.width/2};
}

void EditorScene::addPoint(const cocos2d::Vec2 &rawpos)
{
    clearSelection();

    auto pos = help_touchPoint2editPosition(rawpos);
    auto point = std::make_shared<EEPoint>();
    point->position = help_editPosition2relativePosition(pos);
    point->height = 0;
    point->sprite = Sprite::create("images/point_normal.png");
    point->sprite->setPosition(help_relativePosition2editPosition(point->position));
    point->sprite->setZOrder(Z_POINTS);
    point->sprite->setScale(0.05);
    _pointLayer->addChild(point->sprite);
    point->pid = nextPid();
    _points[point->pid] = point;
    delaunay();
    refreshLines();
    refreshTriangles();
}

void EditorScene:: selectPoint(const cocos2d::Vec2 rawpos, bool multi)
{
    auto point = findSelectedPoint(rawpos);
    if (point) {
        if (!multi) {
            for (auto p : _selectedPoints) {
                p->sprite->setTexture("images/point_normal.png");
            }
            _selectedPoints.clear();
        }
        point->sprite->setTexture("images/point_selected.png");
        bool needToUnselect = false;
        for (auto p : _selectedPoints) {
            if (p == point) {
                needToUnselect = true;
            }
        }
        if (needToUnselect) {
            point->sprite->setTexture("images/point_normal.png");
            _selectedPoints.remove(point);
        } else {
            _selectedPoints.push_back(point);
        }
    }
}


std::shared_ptr<EEPoint> EditorScene::findSelectedPoint(const cocos2d::Vec2& rawpos)
{
    auto pos = help_touchPoint2editPosition(rawpos);

    for (auto & pair : _points) {
        auto distance = pair.second->sprite->getPosition().distance(pos);
        if (distance < pair.second->sprite->getContentSize().width * pair.second->sprite->getScale() /2){
            return pair.second;
        }
    }
    return nullptr;
}

void EditorScene::deletePoint(const cocos2d::Vec2 &rawpos)
{
    clearSelection();

    auto pos = help_touchPoint2editPosition(rawpos);

    for (auto & pair : _points) {
        auto distance = pair.second->sprite->getPosition().distance(pos);
        if (distance < pair.second->sprite->getContentSize().width * pair.second->sprite->getScale() /2){
            CCLOG("delete point");
            _pointLayer->removeChild(pair.second->sprite);
            _points.erase(pair.first);
            _selectedPoints.remove(pair.second);

            delaunay();
            refreshLines();
            refreshTriangles();

            return;
        }
    }
}

void EditorScene::refreshLines()
{
    _linesNode->configLines(_triangles);
}

void EditorScene::refreshTriangles()
{
    // refill color
    for (auto tri : _triangles) {
        int key = tri->calcKey();
        if (_triangleColorMap.count(key) == 0) {
            _triangleColorMap[key] = Vec4{0.5,0.5,0.5, 1.0};
        }
        tri->color = _triangleColorMap[key];
    }
    _trianglesNode->configTriangles(_triangles);
}

void EditorScene::addTestLights()
{
        auto sp = Sprite::create("images/test_light.png");
        sp->setScale(DDConfig::battleCubeWidth()/sp->getContentSize().width);
        sp->setPosition({0,0});
        sp->setZOrder(Z_LIGHT);
        sp->setScale(0.25);
        _pointLayer->addChild(sp);

        _testLightIcon = sp;


    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        bool res = false;
            auto point = touch->getLocation();
            auto size = _testLightIcon->getContentSize();
            auto pos = _testLightIcon->getParent()->getPosition()+ _testLightIcon->getPosition();
            Rect rect = {pos.x - 0.5f*size.width, pos.y - 0.5f*size.height, size.width, size.height};

            if (rect.containsPoint(point)) {
                res = true;
            }

        return res;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        this->clearSelection();
        _testLightIcon->setPosition(_testLightIcon->getPosition() + touch->getDelta());
        _trianglesNode->updateLightPos(_testLightIcon->getPosition()*0.25);
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _pointLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _pointLayer);
}

void EditorScene::clearSelection()
{
    for (auto point : _selectedPoints) {
        point->sprite->setTexture("images/point_normal.png");
    }
    _selectedPoints.clear();
}

void EditorScene::moveUp(bool isup)
{
    const float move_step = 0.015;
    for (auto point : _selectedPoints) {
        point->height += isup ? move_step : -move_step;
    }
    refreshTriangles();
}


void  EditorScene::delaunay()
{

    /*
     typedef struct {
     /** input points count
     unsigned int	num_points;

     /** input points
     del_point2d_t*	points;

     /** number of triangles
     unsigned int	num_triangles;

     the triangles indices v0,v1,v2, v0,v1,v2 ....
     unsigned int*	tris;
     } tri_delaunay2d_t;
     */
    if (_points.size() < 3) {
        return;
    }
    CCLOG("delaunay");
    _delPointsCount = 0;
    for (auto pair : _points) {
        _delPoints[_delPointsCount].x = pair.second->position.x;
        _delPoints[_delPointsCount].y = pair.second->position.y;
        _delPoints[_delPointsCount].pid = pair.first;
        _delPointsCount++;
    }

    auto res_poly = delaunay2d_from(_delPoints, _delPointsCount);
    auto res_tri = tri_delaunay2d_from(res_poly);


    _triangles.clear();
    for (int i = 0; i < res_tri->num_triangles; i++) {
        auto tri = std::make_shared<EETriangle>();
        tri->a = _points[res_tri->points[res_tri->tris[i*3]].pid];
        tri->b = _points[res_tri->points[res_tri->tris[i*3+1]].pid];
        tri->c = _points[res_tri->points[res_tri->tris[i*3+2]].pid];
        _triangles.push_back(tri);
    }

    delaunay2d_release(res_poly);
    tri_delaunay2d_release(res_tri);
}



void EditorScene::diggColor(cocos2d::Vec2 rawpos)
{
    clearSelection();
    float radio = 1.0*rawpos.x/1024.0;
    if (rawpos.y < 44) {
        _diggingColor.w = radio;
    } else if (rawpos.y < 44*2) {
        _diggingColor.z = radio;
    } else if (rawpos.y < 44*3) {
        _diggingColor.y = radio;
    } else if (rawpos.y < 44*4) {
        _diggingColor.x = radio;
    } else if (rawpos.y > 256) {
        auto tri = findTriangle(rawpos);
        if (tri) {
            _diggingColor = _triangleColorMap[tri->calcKey()];
        }
    }
    refreshDiggColor();
}

void EditorScene::refreshDiggColor()
{
    _diggColorLabel->setString(fmt::sprintf(" |||||||||||||||||||||||||R=%.2f, G=%.2f, B=%.2f, A=%.2f |||||||||||||||||||||||||", _diggingColor.x, _diggingColor.y, _diggingColor.z, _diggingColor.w));
    _diggColorLabel->setTextColor({static_cast<GLubyte>(_diggingColor.x*255),static_cast<GLubyte>(_diggingColor.y*255), static_cast<GLubyte>(_diggingColor.z*255), static_cast<GLubyte>(_diggingColor.w*255)});
}

void EditorScene::shadingTriangle(cocos2d::Vec2 rawpos)
{
    clearSelection();
    auto tri = findTriangle(rawpos);
    if (tri) {
        _triangleColorMap[tri->calcKey()] = _diggingColor;
        tri->color = _diggingColor;
        refreshTriangles();
    }
}

std::shared_ptr<EETriangle> EditorScene::findTriangle(cocos2d::Vec2 rawpos)
{
    float tmpOut;
    auto ori = help_touchPoint2editPosition(rawpos);
    ori = help_editPosition2relativePosition(ori);
    for (auto tri : _triangles) {

        if (triangle_intersection(tri->a->pos3d(),  // Triangle vertices
                                  tri->b->pos3d(),
                                  tri->c->pos3d(),
                                  Vec3{ori.x, ori.y, 5.0},  //Ray origin
                                  Vec3{0.f,0.f,-1.f},  //Ray direction
                                  &tmpOut)) {
            return tri;
            
        }
    }
    return nullptr;
}