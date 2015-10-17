// (C) 2015 Turnro.com

#include "EditorScene.h"
#include "TRLocale.h"

USING_NS_CC;

bool EditorScene::init()
{
    assert(TRBaseScene::init());

    _layer = Layer::create();
    this->addChild(_layer);

    _pointLayer = Layer::create();
    _pointLayer->setPosition(genPos({0.5,0.5}));
    this->addChild(_pointLayer);

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
    return {touchpoint.x - size.width/2, touchpoint.y - size.height/2};
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
    _pointLayer->addChild(_trianglesNode);
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

            default:
                break;
        }
        
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        auto rawpos = touch->getLocation();
        CCLOG("click in at %f %f", rawpos.x, rawpos.y);

        if (_ks_addPoint) {
            addPoint(rawpos);
        } else if (_ks_deletePoint) {
            deletePoint(rawpos);
        } else if (_ks_selection) {
            selectPoint(rawpos);
        }

        return _selectedPoints.size() > 0;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){

        auto move = touch->getDelta() * 0.25;
        for (auto point : _selectedPoints) {
            point->sprite->setPosition(point->sprite->getPosition() + move);
            point->position = help_editPosition2relativePosition(point->sprite->getPosition());
        }
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
    auto pos = help_touchPoint2editPosition(rawpos);
    auto point = std::make_shared<EEPoint>();
    point->position = help_editPosition2relativePosition(pos);
    point->height = 0;
    point->sprite = Sprite::create("images/point_normal.png");
    point->sprite->setPosition(help_relativePosition2editPosition(point->position));
    point->sprite->setZOrder(Z_POINTS);
    point->sprite->setScale(0.1);
    _pointLayer->addChild(point->sprite);
    point->pid = nextPid();
    _points[point->pid] = point;
    delaunay();
    refreshLines();
    refreshTriangles();
}

void EditorScene:: selectPoint(const cocos2d::Vec2 rawpos)
{
    auto point = findSelectedPoint(rawpos);
    if (point) {
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
        _testLightIcon->setPosition(_testLightIcon->getPosition() + touch->getDelta());
        _trianglesNode->updateLightPos(_testLightIcon->getPosition());
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
    const float move_step = 0.05;
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