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
            case EventKeyboard::KeyCode::KEY_F:
                _ks_addLine = true;
                break;
            case EventKeyboard::KeyCode::KEY_G:
                _ks_deleteLine = true;
                break;
            case EventKeyboard::KeyCode::KEY_A:
                _ks_selection = true;
                break;
            case EventKeyboard::KeyCode::KEY_D:
                clearSelection();
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
            case EventKeyboard::KeyCode::KEY_F:
                _ks_addLine = false;
                break;
            case EventKeyboard::KeyCode::KEY_G:
                _ks_deleteLine = false;
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
        } else if (_ks_addLine) {
            addLine(rawpos);
        } else if (_ks_deleteLine) {
            deleteLine(rawpos);
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
    _points.push_back(point);
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

    for (auto & point : _points) {
        auto distance = point->sprite->getPosition().distance(pos);
        if (distance < point->sprite->getContentSize().width * point->sprite->getScale() /2){
            return point;
        }
    }
    return nullptr;
}

void EditorScene::deletePoint(const cocos2d::Vec2 &rawpos)
{
    auto pos = help_touchPoint2editPosition(rawpos);

    for (auto & point : _points) {
        auto distance = point->sprite->getPosition().distance(pos);
        if (distance < point->sprite->getContentSize().width * point->sprite->getScale() /2){
            CCLOG("delete point");
            deleteLineWithPoint(point);
            _pointLayer->removeChild(point->sprite);
            _points.remove(point);
            _selectedPoints.remove(point);
            refreshLines();
            return;
        }
    }
}

void EditorScene::addLine(const cocos2d::Vec2& rawpos)
{
    auto selectedPoint = findSelectedPoint(rawpos);
    if (selectedPoint) {
        if (_firstSelectedPoint && _firstSelectedPoint != selectedPoint) {

            for (auto& line : _lines) {
                if ((line->a == _firstSelectedPoint && line->b == selectedPoint) || (line->b == _firstSelectedPoint && line->a == selectedPoint)) {
                    _firstSelectedPoint = selectedPoint;
                    return;
                }
            }
            CCLOG("add line");
            auto line = std::make_shared<EELine>();
            line->a = _firstSelectedPoint;
            line->b = selectedPoint;
            _lines.push_back(line);
            refreshLines();
            _firstSelectedPoint = nullptr;
        } else {
            _firstSelectedPoint = selectedPoint;
        }
    }
}

void EditorScene::deleteLine(const cocos2d::Vec2& rawpos)
{
    auto selectedPoint = findSelectedPoint(rawpos);
    if (selectedPoint) {
        if (_firstSelectedPoint && _firstSelectedPoint != selectedPoint) {

            for (auto& line : _lines) {
                if ((line->a == _firstSelectedPoint && line->b == selectedPoint) || (line->b == _firstSelectedPoint && line->a == selectedPoint)) {
                    CCLOG("remove line");
                    _lines.remove(line);
                    break;
                }
            }

            refreshLines();
            _firstSelectedPoint = nullptr;
        } else {
            _firstSelectedPoint = selectedPoint;
        }
    }
}

void EditorScene::deleteLineWithPoint(const std::shared_ptr<EEPoint>& point)
{
    for (auto iter = _lines.begin(); iter != _lines.end(); ) {
        if ((*iter)->a == point || (*iter)->b == point) {
            iter = _lines.erase(iter);
        } else {
            iter++;
        }
    }
}

void EditorScene::refreshLines()
{
    _linesNode->configLines(_lines);
    refreshTriangles();
}

std::shared_ptr<EEPoint> EditorScene::help_checkTwoLineConnection(const std::shared_ptr<EELine>& a, const std::shared_ptr<EELine>& b)
{
    return  a->a == b->a ? a->a :
    a->a == b->b ? a->a :
    a->b == b->a ? a->b :
    a->b == b->b ? a->b : nullptr;
}

void EditorScene::refreshTriangles()
{
    _triangles.clear();
    for (auto ia = _lines.begin(); ia != _lines.end(); ia++) {
        auto ib = ia;ib++; if (ib == _lines.end()) continue;
        for (; ib != _lines.end(); ib++) {
            auto ic = ib; ic++; if(ic == _lines.end()) continue;
            for (; ic != _lines.end(); ic++) {
                auto la = help_checkTwoLineConnection(*ia, *ib);
                auto lb = help_checkTwoLineConnection(*ia, *ic);
                auto lc = help_checkTwoLineConnection(*ib, *ic);
                if (la && lb && lc){
                    auto t = std::make_shared<EETriangle>();
                    t->a = la;
                    t->b = lb;
                    t->c = lc;
                    _triangles.push_back(t);
                }
            }
        }
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