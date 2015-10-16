// (C) 2015 Turnro.com

#include "SuperHero.h"
#include "RollNode.h"
#include "intersection.h"
#include "LocalSpaceRotation.h"
#include "format.h"
#include "SimpleAudioEngine.h"
#include "TRLocale.h"
#include "TurnScene.h"
#include "MenuScene.h"
#include "TRStories.h"
#include "RolkConfig.h"


USING_NS_CC;
#define STATIC_GLASS
//#define SKYBOX

inline cocos2d::Vec3 genPos3D(const cocos2d::Vec2& pos, float z){
    auto size = Director::getInstance()->getVisibleSize();
    return {size.width * pos.x, size.height * pos.y, z};
}

inline float endIndex2RoadHaldWidth(int endIndex) {
    return RoadHalfWidth + RoadHalfWidth/100.0*endIndex*endIndex;
}
// 注意，为了安全和编码的简便，maxmap的终点minimap后必须还有几个多余的minimap。
std::pair<PixelPlane*, RoadNode*> SuperHero::buildRoad(const std::unique_ptr<RollMinMap>& mapdata, const RollNode& lastNodeOfLastMinmap, bool hasLastNode, std::vector<PixelPlane*>& glassHolder, std::vector<PixelPlane*>& gateHolder, bool afterEnd)
{
    glassHolder.clear();
    gateHolder.clear();

    auto _drawNodeStaticGlass = RoadNode::create();
    _drawNodeStaticGlass->setPosition3D({0,0,0});
    _drawNodeStaticGlass->setCameraMask(_camera->getCameraMask());
    _drawNodeStaticGlass->setLineColor(Color4F::WHITE);
    _mainLayer->addChild(_drawNodeStaticGlass);

    auto _drawNodeRoad = PixelPlane::create("3d/pp_road.fsh",EditState::s()->getBgmTextureFile());
    _drawNodeRoad->setPosition3D({0,0,0});
    _drawNodeRoad->setRoadColor(Color4F::WHITE);
    _drawNodeRoad->setCameraMask(_camera->getCameraMask());
    _mainLayer->addChild(_drawNodeRoad);

    // lastNodeOfLastMinmap是上一个片段的最后一个node，它用来和新绘制片段的第一个连接在一起。
    // 否则会出现一个小块的断裂。hasLastNode标志：是否存在这个node。先画这一块。
    if (hasLastNode) {
        //【注意】 这段代码和下面for-loop下的drawing code是冗余的。
        float roadHalfWidth = afterEnd ? 100.0:RoadHalfWidth;
        auto firstPos = lastNodeOfLastMinmap._position;
        Vec3 firstLeft = lastNodeOfLastMinmap._posture * Vec3{-roadHalfWidth, 0, 0} + firstPos;
        Vec3 firstRight = lastNodeOfLastMinmap._posture * Vec3{roadHalfWidth, 0, 0} + firstPos;
        auto secondPos = mapdata->_data.begin()->_position;
        Vec3 secondLeft = mapdata->_data.begin()->_posture * Vec3{-roadHalfWidth, 0, 0} + secondPos;
        Vec3 secondRight = mapdata->_data.begin()->_posture * Vec3{roadHalfWidth, 0, 0} + secondPos;
        _drawNodeRoad->drawRoad(firstLeft, firstRight, secondLeft, secondRight);
//        _drawNodeRoad->drawLine(firstLeft, firstRight);
//        _drawNodeRoad->drawLine(firstLeft, secondLeft);
//        _drawNodeRoad->drawLine(firstRight, secondRight);

#ifdef STATIC_GLASS
        if (!lastNodeOfLastMinmap._uncover) {
            Vec3 topLeft = lastNodeOfLastMinmap._posture * Vec3{-roadHalfWidth, StaticGlassHeight, 0} + firstPos;
            Vec3 topRight = lastNodeOfLastMinmap._posture * Vec3{roadHalfWidth, StaticGlassHeight, 0} + firstPos;
            _drawNodeStaticGlass->drawLine(firstLeft, topLeft);
            _drawNodeStaticGlass->drawLine(firstRight, topRight);
            _drawNodeStaticGlass->drawLine(topLeft, topRight);
        }
#endif
    }

    //其余的块。
    assert(mapdata->_data.size() >= 2);
    auto iterPrev = mapdata->_data.end();
    auto iterFirst = mapdata->_data.begin();
    auto iterSecond = iterFirst; iterSecond++;
    bool hasEnd = false;
    int endIndex = afterEnd ? 100:0;
    for (; iterSecond != mapdata->_data.end(); iterFirst++, iterSecond++) {

        if (!hasEnd && iterFirst->_end) {
            hasEnd = true;
        }

        auto firstPos = iterFirst->_position;
        Vec3 firstLeft = iterFirst->_posture * Vec3{-endIndex2RoadHaldWidth(endIndex), 0, 0} + firstPos;
        iterFirst->_leftPoint = firstLeft;
        Vec3 firstRight = iterFirst->_posture * Vec3{endIndex2RoadHaldWidth(endIndex), 0, 0} + firstPos;
        iterFirst->_rightPoint = firstRight;
        auto secondPos = iterSecond->_position;
        Vec3 secondLeft = iterSecond->_posture * Vec3{-endIndex2RoadHaldWidth(endIndex), 0, 0} + secondPos;
        iterSecond->_leftPoint = secondLeft;
        Vec3 secondRight = iterSecond->_posture * Vec3{endIndex2RoadHaldWidth(endIndex), 0, 0} + secondPos;
        iterSecond->_rightPoint = secondRight;



        if (iterFirst->_type == RollNode::RT_UNREAL) {
            if (iterPrev == mapdata->_data.end() || iterPrev->_type == RollNode::RT_REAL) {
                // PREV不是VIRTUAL，所以画横线。
//                _drawNodeRoad->drawLine(firstLeft, firstRight);
            }
        } else {
            _drawNodeRoad->drawRoad(firstLeft, firstRight, secondLeft, secondRight);
//            _drawNodeRoad->drawLine(firstLeft, firstRight);
//            _drawNodeRoad->drawLine(firstLeft, secondLeft);
//            _drawNodeRoad->drawLine(firstRight, secondRight);
#ifdef STATIC_GLASS
            if (!iterFirst->_uncover) {
                Vec3 topLeft = iterFirst->_posture * Vec3{-endIndex2RoadHaldWidth(endIndex), StaticGlassHeight, 0} + firstPos;
                Vec3 topRight = iterFirst->_posture * Vec3{endIndex2RoadHaldWidth(endIndex), StaticGlassHeight, 0} + firstPos;

                _drawNodeStaticGlass->drawLine(firstLeft, topLeft);
                _drawNodeStaticGlass->drawLine(firstRight, topRight);
                _drawNodeStaticGlass->drawLine(topLeft, topRight);
            }
#endif
        }


        if (iterPrev == mapdata->_data.end()) {
            iterPrev = mapdata->_data.begin();
        } else {
            iterPrev++;
        }

        if (iterFirst->_glass) {
            // 动态Glass
            auto glass = _dynamicGlassPool.brrow();
            glass->setPosition3D(iterFirst->_position + iterFirst->_posture * Vec3{0, StaticGlassHeight*0.25, 0});
            glass->setScale(1);
            glass->setRotationQuat(iterFirst->_posture);
            glass->reset();
            glassHolder.push_back(glass);
        }

        if (iterFirst->_gate) {
            // 星门
            auto gate = _dynamicGatePool.brrow();
            gate->setPosition3D(iterFirst->_position + iterFirst->_posture * Vec3{0, StaticGlassHeight*0.5, 0});
            gate->setRotationQuat(iterFirst->_posture);
            // 原生的旋转在World Space旋转，不适用。
            // gate->runAction(RepeatForever::create(RotateBy::create(20.0, Vec3{0,0,360})));
            // 定制的在Object Local Space旋转的Action。
            gate->runAction(RepeatForever::create(LocalSpaceRotateBy::create(5.0, Vec3{0.f,0.f, rand_0_1() > 0.5 ? 360.f:-360.f})));
            gate->reset();
            gateHolder.push_back(gate);
        }

        if (hasEnd) {
            endIndex++;
        }
    }

    return {_drawNodeRoad, _drawNodeStaticGlass};
}

std::tuple<PixelPlane*, PixelPlane*, PixelPlane*> SuperHero::buildAsset(const std::unique_ptr<RollMinMap>& minmap)
{
    auto pp_float = PixelPlane::create("3d/pp_asset.fsh", EditState::s()->getBgmTextureFile());
    auto pp_sky = PixelPlane::create("3d/pp_asset2.fsh", EditState::s()->getBgmTextureFile());
    auto pp_abyss = PixelPlane::create("3d/pp_asset2.fsh", EditState::s()->getBgmTextureFile());

    float len = 0.5;
    float hhh = 5.;

    {//Float
        auto pp = pp_float;
        pp->setPosition3D(minmap->_data.begin()->_position);
        pp->setScale(1);
        pp->setCameraMask(_camera->getCameraMask());
        _mainLayer->addChild(pp);
        pp->setVisible(minmap->_floatAssets.size() > 0);

        Vec3 bl = {-len,-len,len};
        Vec3 br = {len,-len,len};
        Vec3 tl = {-len,len,len};
        Vec3 tr = {len,len,len};
        Vec3 bl2 = {-len,-len,-len};
        Vec3 br2 = {len,-len,-len};
        Vec3 tl2 = {-len,len,-len};
        Vec3 tr2 = {len,len,-len};
        for (auto iter = minmap->_floatAssets.begin(); iter != minmap->_floatAssets.end(); iter++) {
            // todo 一个形状而不是面?? 是个井?
            pp->drawRoad(iter->_position+ bl, iter->_position+br, iter->_position+tl, iter->_position+tr);
            pp->drawRoad(iter->_position+ bl2, iter->_position+br2, iter->_position+tl2, iter->_position+tr2);
            pp->drawRoad(iter->_position+ bl2, iter->_position+bl, iter->_position+tl2, iter->_position+tl);
            pp->drawRoad(iter->_position+ br, iter->_position+br2, iter->_position+tr, iter->_position+tr2);        }
    }

    {//Sky
        auto pp = pp_sky;
        pp->setPosition3D(minmap->_data.begin()->_position);
        pp->setScale(1);
        pp->setCameraMask(_camera->getCameraMask());
        _mainLayer->addChild(pp);
        pp->setVisible(minmap->_skyAssets.size() > 0);

        Vec3 bl = {-len,-len,len};
        Vec3 br = {len,-len,len};
        Vec3 tl = {-len,len+hhh,len};
        Vec3 tr = {len,len+hhh,len};
        Vec3 bl2 = {-len,-len,-len};
        Vec3 br2 = {len,-len,-len};
        Vec3 tl2 = {-len,len+hhh,-len};
        Vec3 tr2 = {len,len+hhh,-len};
        for (auto iter = minmap->_skyAssets.begin(); iter != minmap->_skyAssets.end(); iter++) {
            // todo 一个形状而不是面?? 是个井?
            pp->drawRoad(iter->_position+ bl, iter->_position+br, iter->_position+tl, iter->_position+tr);
            pp->drawRoad(iter->_position+ bl2, iter->_position+br2, iter->_position+tl2, iter->_position+tr2);
            pp->drawRoad(iter->_position+ bl2, iter->_position+bl, iter->_position+tl2, iter->_position+tl);
            pp->drawRoad(iter->_position+ br, iter->_position+br2, iter->_position+tr, iter->_position+tr2);        }
    }

    {//abyss
        auto pp = pp_abyss;
        pp->setPosition3D(minmap->_data.begin()->_position);
        pp->setScale(1);
        pp->setCameraMask(_camera->getCameraMask());
        _mainLayer->addChild(pp);
        pp->setVisible(minmap->_abyssAssets.size() > 0);

        Vec3 bl = {-len,-len-hhh,len};
        Vec3 br = {len,-len-hhh,len};
        Vec3 bl2 = {-len,-len-hhh,-len};
        Vec3 br2 = {len,-len-hhh,-len};
        Vec3 tl = {-len,len,len};
        Vec3 tr = {len,len,len};
        Vec3 tl2 = {-len,len,-len};
        Vec3 tr2 = {len,len,-len};
        for (auto iter = minmap->_abyssAssets.begin(); iter != minmap->_abyssAssets.end(); iter++) {
            // todo 一个形状而不是面?? 是个井?
            pp->drawRoad(iter->_position+ bl, iter->_position+br, iter->_position+tl, iter->_position+tr);
            pp->drawRoad(iter->_position+ bl2, iter->_position+br2, iter->_position+tl2, iter->_position+tr2);
            pp->drawRoad(iter->_position+ bl2, iter->_position+bl, iter->_position+tl2, iter->_position+tl);
            pp->drawRoad(iter->_position+ br, iter->_position+br2, iter->_position+tr, iter->_position+tr2);
        }
    }

    return {pp_float, pp_sky, pp_abyss};
}

void SuperHero::initRoad()
{
    _minmapIndex = 0;
    _drawNodeRoadCurrent = _drawNodeRoadPrev = _drawNodeRoadNext = nullptr;
    _drawNodeStaticGlassNext = _drawNodeStaticGlassCurrent = _drawNodeStaticGlassPrev = nullptr;

    assert(_maxmap->getSize() > 2);
    RollNode nullNode;
    auto resCurrent = buildRoad(_maxmap->getCookedMinMap(0), nullNode, false, _dynamicGlassesCurrent, _dynamicGatesCurrent, false);
    auto resNext = buildRoad(_maxmap->getCookedMinMap(1), _maxmap->getCookedMinMap(0)->_data.back(), true, _dynamicGlassesNext, _dynamicGatesNext, false);

    _drawNodeRoadCurrent = resCurrent.first;
    _drawNodeStaticGlassCurrent = resCurrent.second;
    _drawNodeRoadNext = resNext.first;
    _drawNodeStaticGlassNext = resNext.second;

    // Asset
    _assetNodeCurrent = buildAsset(_maxmap->getCookedMinMap(0));
    _assetNodeNext = buildAsset(_maxmap->getCookedMinMap(1));
}

void SuperHero::initSkybox()
{
    float len = 500.0;
    Vec3 bl = {-len,0,-len};
    Vec3 br = {len, 0,-len};
    Vec3 tl = {-len, 0,len};
    Vec3 tr = {len, 0,len};

    {
        auto _upNode = PixelPlane::create("3d/pp_galaxy.fsh", EditState::s()->getBgmTextureFile());
        _upNode->setPosition3D({0,30,0});
        _upNode->setScale(1);
        _upNode->setCameraMask(_camera->getCameraMask());
        _mainLayer->addChild(_upNode);
        _upNode->drawRoad(bl, br, tl, tr);
        _pixelPlaneSky = _upNode;
    }

    {
        auto _upNode = PixelPlane::create("3d/pp_galaxy.fsh", EditState::s()->getBgmTextureFile());
        _upNode->setPosition3D({0,-30,0});
        _upNode->setScale(1);
        _upNode->setCameraMask(_camera->getCameraMask());
        _mainLayer->addChild(_upNode);
        _upNode->drawRoad(bl, br, tl, tr);
        _pixelPlaneLand = _upNode;
    }

#ifndef  SKYBOX
    _pixelPlaneSky->setVisible(false);
    _pixelPlaneLand->setVisible(false);
#endif

}

void SuperHero::init(cocos2d::Camera* camera, GameUIState* uistate, Maxmap* maxmap, cocos2d::Layer* mainLayer, cocos2d::Layer* uiLayer)
{
    _camera = camera;
    _uistate = uistate;
    _maxmap = maxmap;
    _mainLayer = mainLayer;
    _uiLayer = uiLayer;

    _musicEnable = TRLocale::s().isMusicEnable();
    _sfxEnable = TRLocale::s().isSfxEnable();
    _isInfinite = EditState::s()->isInfinite();
    if (_isInfinite)
        _historyBestPts = TRLocale::s().getSectionStatic(EditState::s()->getSectionId(), true).second;

    RoadNode::initShaders();
    _dynamicGlassPool.init(_mainLayer, _camera);
    _dynamicGatePool.init(_mainLayer, _camera);
    initRoad();
    initSkybox();

    auto firstNode =_maxmap->getCookedMinMap(_minmapIndex)->_data.begin();
    _position = firstNode->_position + firstNode->_posture * Vec3{0,2,-.25};
    _posture = firstNode->_posture;

    _headPosition = _position + _posture * Vec3{0, HEIGHT, 0};
    _cameraRelativePosture = Quaternion::identity();
    _cameraMicroMovePosture = Quaternion::identity();
    _cameraCurrentPosture = _cameraRelativePosture*_posture;
    _heightCurrent = HEIGHT;

    camera->setPosition3D(_headPosition);
    camera->setRotationQuat(_cameraCurrentPosture*_cameraMicroMovePosture);

    _roadSpeed = 0.f;
    _airSpeed = {0,0,0};
    _roadIter = firstNode;
    initCameraSpace();
    initUIViews();
    initSettlementViews();
}

// 除了Init时之外，其它的时候进入到新的minmap时，需要调用此函数来，如果没有Build的road出来。
void SuperHero::setMinmapIndex(int index)
{
    if (index == _minmapIndex) return;

    auto rmassets = [this](const std::tuple<PixelPlane*, PixelPlane*, PixelPlane*>& tpassets) {
        if (std::get<0>(tpassets)) {
            _mainLayer->removeChild(std::get<0>(tpassets));
            _mainLayer->removeChild(std::get<1>(tpassets));
            _mainLayer->removeChild(std::get<2>(tpassets));
        }
    };

    if (index == _minmapIndex+1) {
        if (_drawNodeRoadPrev) _mainLayer->removeChild(_drawNodeRoadPrev);
        if (_drawNodeStaticGlassPrev) _mainLayer->removeChild(_drawNodeStaticGlassPrev);
        rmassets(_assetNodePrev);
        for (auto g :_dynamicGlassesPrev) {
            _dynamicGlassPool.giveback(g);
        }
        for (auto g : _dynamicGatesPrev) {
            _dynamicGatePool.giveback(g);
        }

        _drawNodeRoadPrev = _drawNodeRoadCurrent;
        _drawNodeStaticGlassPrev = _drawNodeStaticGlassCurrent;
        _dynamicGlassesPrev = _dynamicGlassesCurrent;
        _dynamicGatesPrev = _dynamicGatesCurrent;
        _assetNodePrev = _assetNodeCurrent;

        _drawNodeRoadCurrent = _drawNodeRoadNext;
        _drawNodeStaticGlassCurrent = _drawNodeStaticGlassNext;
        _dynamicGlassesCurrent = _dynamicGlassesNext;
        _dynamicGatesCurrent = _dynamicGatesNext;
        _assetNodeCurrent = _assetNodeNext;

        auto retNext = buildRoad(_maxmap->getCookedMinMap(index+1), _maxmap->getCookedMinMap(index)->_data.back(), true, _dynamicGlassesNext, _dynamicGatesNext, _maxmap->getEndIndex() < index+1);
        _drawNodeRoadNext = retNext.first;
        _drawNodeStaticGlassNext = retNext.second;
        _assetNodeNext = buildAsset(_maxmap->getCookedMinMap(index+1));
    } else {

        if (_drawNodeRoadPrev)_mainLayer->removeChild(_drawNodeRoadPrev);
        if (_drawNodeStaticGlassPrev)_mainLayer->removeChild(_drawNodeStaticGlassPrev);
        for (auto g : _dynamicGlassesPrev) {
            _dynamicGlassPool.giveback(g);
        }
        for (auto g : _dynamicGatesPrev) {
            _dynamicGatePool.giveback(g);
        }

        if (_drawNodeRoadCurrent)_mainLayer->removeChild(_drawNodeRoadCurrent);
        if (_drawNodeStaticGlassCurrent)_mainLayer->removeChild(_drawNodeStaticGlassCurrent);
        for (auto g : _dynamicGlassesCurrent) {
            _dynamicGlassPool.giveback(g);
        }
        for (auto g : _dynamicGatesCurrent) {
            _dynamicGatePool.giveback(g);
        }
        if (_drawNodeRoadNext)_mainLayer->removeChild(_drawNodeRoadNext);
        if (_drawNodeStaticGlassNext)_mainLayer->removeChild(_drawNodeStaticGlassNext);
        for (auto g : _dynamicGlassesNext) {
            _dynamicGlassPool.giveback(g);
        }
        for (auto g : _dynamicGatesNext) {
            _dynamicGatePool.giveback(g);
        }
        rmassets(_assetNodePrev);
        rmassets(_assetNodeCurrent);
        rmassets(_assetNodeNext);

        if (index == 0) {
            initRoad();
        } else {
            RollNode nullNode;
            RollNode& last = index-1 == 0 ? nullNode : _maxmap->getCookedMinMap(index-2)->_data.back();
            auto retPrev = buildRoad(_maxmap->getCookedMinMap(index-1), last, index-1 != 0, _dynamicGlassesPrev, _dynamicGatesPrev, _maxmap->getEndIndex() < index-1);
            _drawNodeRoadPrev = retPrev.first;
            _drawNodeStaticGlassPrev = retPrev.second;
            auto retCurrent = buildRoad(_maxmap->getCookedMinMap(index), _maxmap->getCookedMinMap(index-1)->_data.back(), true, _dynamicGlassesCurrent, _dynamicGatesPrev, _maxmap->getEndIndex() < index);
            _drawNodeRoadCurrent = retCurrent.first;
            _drawNodeStaticGlassCurrent = retCurrent.second;
            auto retNext = buildRoad(_maxmap->getCookedMinMap(index+1), _maxmap->getCookedMinMap(index)->_data.back(), true, _dynamicGlassesNext,  _dynamicGatesNext, _maxmap->getEndIndex() < index+1);
            _drawNodeRoadNext = retNext.first;
            _drawNodeStaticGlassNext = retNext.second;

            _assetNodePrev = buildAsset(_maxmap->getCookedMinMap(index-1));
            _assetNodeCurrent = buildAsset(_maxmap->getCookedMinMap(index));
            _assetNodeNext = buildAsset(_maxmap->getCookedMinMap(index+1));
        }
    }
    _minmapIndex = index;
}

void SuperHero::step(float dt)
{

    if (EditState::s()->_needPause) {
        EditState::s()->_needPause = false;
        if (!_paused) aniPause();
    }
    if (_paused) return;
    if (_state == SH_AIR) {
        // 位移
        _lastPosition = _position;
        Vec3 posDiff = _airSpeed*dt;
        _position += posDiff;

        // 距离最近的Road
        float minFar = std::numeric_limits<float>::max();
        auto iterNearest = _maxmap->getCookedMinMap(_minmapIndex)->_data.end();
        auto indexNearest = _minmapIndex;
        for (auto iter = _maxmap->getCookedMinMap(_minmapIndex)->_data.begin(); iter != _maxmap->getCookedMinMap(_minmapIndex)->_data.end(); iter++) {
            if (iter->_type == RollNode::RT_UNREAL) continue;
            auto diff = iter->_position - _position;
            if (diff.length() < minFar) {
                minFar = diff.length();
                iterNearest = iter;
                indexNearest = _minmapIndex;
            }
        }
        if (_maxmap->checkIndex(_minmapIndex+1) ){
        for (auto iter = _maxmap->getCookedMinMap(_minmapIndex+1)->_data.begin(); iter != _maxmap->getCookedMinMap(_minmapIndex+1)->_data.end(); iter++) {
            if (iter->_type == RollNode::RT_UNREAL) continue;
            auto diff = iter->_position - _position;
            if (diff.length() < minFar) {
                minFar = diff.length();
                iterNearest = iter;
                indexNearest = _minmapIndex +1;
            }
        }
        }

        // 重力加速度
        auto graDir = iterNearest->_posture * Vec3{0,-1,0};
        _airSpeed += graDir * (_uistate->_isTouch ? MULTI_GRAVITY_FACTOR*GRAVITY : GRAVITY);

        // 姿态变更
        setPosture(iterNearest->_posture);

        // 找到与最近的相邻的第二近的Node
        auto iterFirst = iterNearest;
        auto iterSecond = iterNearest;
        auto iterPrev = iterNearest;
        auto iterPrev2 = iterNearest;
        bool hasPrev = true;
        bool hasPrev2 = true;
        if (iterNearest == _maxmap->getCookedMinMap(0)->_data.begin()) {
            iterSecond++;
            hasPrev = false;
        } else {
            iterSecond = iterNearest; iterSecond++;
            iterPrev = iterNearest; iterPrev--;
            iterPrev2 = iterPrev; iterPrev2--;
            if (iterSecond == _maxmap->getCookedMinMap(indexNearest)->_data.end()) {
                assert(_maxmap->checkIndex(indexNearest+1));
                iterSecond = _maxmap->getCookedMinMap(indexNearest+1)->_data.begin();
            }
            if (iterNearest == _maxmap->getCookedMinMap(indexNearest)->_data.begin()) {
                iterPrev = _maxmap->getCookedMinMap(indexNearest-1)->_data.end();
                iterPrev--;
                iterPrev2 = iterPrev;iterPrev2--;
            }
            if (iterPrev == _maxmap->getCookedMinMap(indexNearest)->_data.begin()) {
                if (indexNearest > 0) {
                    iterPrev2 = _maxmap->getCookedMinMap(indexNearest-1)->_data.end();
                    iterPrev2--;
                } else {
                    hasPrev2 = false;
                }
            }
            if (iterPrev->_type == RollNode::RT_UNREAL) {
                hasPrev = false;
            }
            if (hasPrev2 == false || iterPrev == _maxmap->getCookedMinMap(0)->_data.begin() || iterPrev2->_type == RollNode::RT_UNREAL) {
                hasPrev2 = false;
            }
        }

        // 以最近和第二近组成的平面进行碰撞
        // 四个3角形，防止在特定扭转时出现缝隙。
        // 以头顶-脚线段A/上一个脚-现在的脚线段B
        // 线段与三角形碰撞。
        // 如果B碰到，是落下。如果B没有碰到，而A碰到，是撞击。
        // 全部没碰到，是没碰到。
        bool footCollision = false;
        bool bodyCollision = false;
        float yDiff = (iterNearest->_posture* posDiff).y;
        auto _footHeadPos = _position + iterNearest->_posture * Vec3{0,-yDiff*1.5f,0};

        if (segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterSecond->_leftPoint, _position, _headPosition) ||
            segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterSecond->_rightPoint, _position, _headPosition) ||
            segment_trangle_intersection(iterSecond->_rightPoint, iterSecond->_leftPoint, iterFirst->_leftPoint, _position, _headPosition) ||
            segment_trangle_intersection(iterSecond->_rightPoint, iterSecond->_leftPoint, iterFirst->_rightPoint, _position, _headPosition) ){
            CCLOG("body clision %f %f %f %d %d", _airSpeed.x, _airSpeed.y, _airSpeed.z, hasPrev ?1:0, hasPrev2?1:0);
            bodyCollision = true;
        }

        if (graDir.dot(_airSpeed) > 0) {
            if (segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterSecond->_leftPoint, _position, _footHeadPos) ||
                segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterSecond->_rightPoint, _position, _footHeadPos)||
                segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterSecond->_position, _position, _footHeadPos) ||
                segment_trangle_intersection(iterSecond->_rightPoint, iterSecond->_leftPoint, iterFirst->_leftPoint, _position, _footHeadPos) ||
                segment_trangle_intersection(iterSecond->_rightPoint, iterSecond->_leftPoint, iterFirst->_rightPoint, _position, _footHeadPos)) {
                CCLOG("foot clision 1\n");
                footCollision = true;
            }
            if (hasPrev) {
                if (segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterPrev->_leftPoint, _position, _footHeadPos) ||
                    segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterPrev->_rightPoint, _position, _footHeadPos) ||
                    segment_trangle_intersection(iterFirst->_rightPoint, iterFirst->_leftPoint, iterPrev->_position, _position, _footHeadPos)||/*
                    segment_trangle_intersection(iterSecond->_rightPoint, iterSecond->_leftPoint, iterPrev->_position, secPos, _lastPosition) ||*/
                    segment_trangle_intersection(iterPrev->_rightPoint, iterPrev->_leftPoint, iterFirst->_leftPoint, _position, _footHeadPos) ||
                    segment_trangle_intersection(iterPrev->_rightPoint, iterPrev->_leftPoint, iterFirst->_rightPoint, _position, _footHeadPos)) {
                    CCLOG("foot clision 2\n");
                    footCollision = true;
                }
            }
            if (hasPrev && hasPrev2) {
                if (segment_trangle_intersection(iterPrev->_rightPoint, iterPrev->_leftPoint, iterPrev2->_leftPoint, _position, _footHeadPos) ||
                    segment_trangle_intersection(iterPrev->_rightPoint, iterPrev->_leftPoint, iterPrev2->_rightPoint, _position, _footHeadPos) ||/*
                    segment_trangle_intersection(iterPrev2->_rightPoint, iterPrev2->_leftPoint, iterSecond->_position, _position, _footHeadPos) ||*/
                     segment_trangle_intersection(iterPrev2->_rightPoint, iterPrev2->_leftPoint, iterPrev->_leftPoint, _position, _footHeadPos) ||
                     segment_trangle_intersection(iterPrev2->_rightPoint, iterPrev2->_leftPoint, iterPrev->_rightPoint, _position, _footHeadPos)) {
                         CCLOG("foot clision 3\n");
                         footCollision = true;
                     }
            }

        }

        // 碰撞动作
        if (footCollision) {
            // 落地
            _state = SH_ROAD;
            // 计算跑动offset和roaditer和xdiff和speed
            _roadIter = iterFirst;
            setMinmapIndex(indexNearest);
            _roadIterNext = _roadIter; _roadIterNext++;
            if (_roadIterNext == _maxmap->getCookedMinMap(_minmapIndex)->_data.end()) {
                _roadIterNext = _maxmap->getCookedMinMap(_minmapIndex+1)->_data.begin();
                _isRoadIterInNextMinimap = true;
            } else {
                _isRoadIterInNextMinimap = false;
            }

            _roadLength = (_roadIterNext->_position - _roadIter->_position).length();
            _roadDir = _roadIterNext->_position - _roadIter->_position;
            _roadDir.normalize();
            auto rawCollisionPos = _roadIter->_posture.getInversed() * (_position - _roadIter->_position);
            //CCLOG("%f %f", _position.z, rawCollisionPos.z);
            _roadXdiff = rawCollisionPos.x / RoadHalfWidth;
            _roadOffset = -rawCollisionPos.z;
            auto rawSPeed = _roadIter->_posture.getConjugated() * _airSpeed;
            _roadSpeed = -rawSPeed.z;
            setPosture(_roadIter->_posture);
            setupHeightAnimation(std::max(HEIGHT_HITDOWN_MAX, HEIGHT*(1 - HEIGHT_HITDOWN_FACTOR * std::abs(rawSPeed.y))) );

//            setCameraRelativePosture({-0.022,0.023,0.018,0.999});
//            _camera->scheduleOnce([this](float dt){setCameraRelativePosture({0.020,-0.017,-0.015,1.000});}, 0.1f, "sdfdf");
            float POSTURE_DEGREE = 2.f + _roadSpeed/60.f*5;
            setCameraRelativePosture(rotation2quaternion({rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE}));//{-0.260,0.000,0.000,0.958});
            _camera->scheduleOnce([this](float dt){
                float DEG = 1.f + _roadSpeed/60.f*3.f;;
                setCameraRelativePosture(rotation2quaternion(Vec3{rand_0_1()*2*DEG-DEG, rand_0_1()*2*DEG-DEG, rand_0_1()*2*DEG-DEG}));//{0.022,0.000,0.000,1.000});
            }, 0.1f, "sdfdf");

            _camera->scheduleOnce([this](float dt){setCameraRelativePosture(CAMERA_NORMAL_POSTURE);}, 0.2f, "sdfdf2");
            actionLandRoad();
        } else if (bodyCollision) {
            // 撞击
        }
    }// SH_AIR
    else if (_state == SH_ROAD) {
        // 速度
        if (_uistate->_isTouch && !_win) {
                _roadSpeed += getRoadForwardAcce() * dt;
        } else {
            _roadSpeed += getRoadStopAcce()*dt;
            if (_roadSpeed < 0) {
                _roadSpeed = 0;
            }
        }

        // 位移
        _roadOffset += _roadSpeed*dt;
        if (_roadXdiff < 0) {
            _roadXdiff += ROAD_XDIFF_SPEED*dt;
            if (_roadXdiff > 0) _roadXdiff = 0;
        } else if (_roadXdiff > 0) {
            _roadXdiff -= ROAD_XDIFF_SPEED*dt;
            if (_roadXdiff < 0) _roadXdiff = 0;
        }

        // 检查是否过头
        if (_roadLength < _roadOffset) {
                _roadIter = _roadIterNext;
            if (_isRoadIterInNextMinimap) {
                _isRoadIterInNextMinimap = false;
                setMinmapIndex(_minmapIndex+1);
            }
                _roadIterNext++;
            if (_roadIterNext == _maxmap->getCookedMinMap(_minmapIndex)->_data.end()) {
                _isRoadIterInNextMinimap = true;
                _roadIterNext =_maxmap->getCookedMinMap(_minmapIndex+1)->_data.begin();
            }
                _roadOffset = _roadOffset - _roadLength;
                _roadLength = (_roadIterNext->_position - _roadIter->_position).length();
                _roadDir = _roadIterNext->_position - _roadIter->_position;
                _roadDir.normalize();
        }
        if (!_win && (_roadIter->_end || _roadIterNext->_end)) {
            _win = true;
            actionGameWin();
        }

        // 换算
        _lastPosition = _position;
        _position = _roadIter->_position + _roadOffset * _roadDir;
        setPosture(_roadIter->_posture);

        // 跳起
        if (_isLastTouch && !_uistate->_isTouch) {
            _state = SH_AIR;
            // 速度
            float jumpStepp = std::min(JUMP_SPEED_BASE + JUMP_SPEED_FACTOR * _roadSpeed, JUMP_SPEED_MAX);
            _airSpeed = _posture * Vec3{0, jumpStepp, -_roadSpeed};
            CCLOG("posture (%f %f %f %f)", _posture.x, _posture.y, _posture.z, _posture.w);
            CCLOG("jump %f  (%f %f %f)", jumpStepp, _airSpeed.x, _airSpeed.y, _airSpeed.z);

            // 镜头姿态动画
            float POSTURE_DEGREE = 5.f + _roadSpeed/60.f*5;
            setCameraRelativePosture(rotation2quaternion({rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE}));//{-0.260,0.000,0.000,0.958});
            _camera->scheduleOnce([this](float dt){
                float DEG = 3.f + _roadSpeed/60.f*3.f;;
                setCameraRelativePosture(rotation2quaternion(Vec3{rand_0_1()*2*DEG-DEG, rand_0_1()*2*DEG-DEG, rand_0_1()*2*DEG-DEG}));//{0.022,0.000,0.000,1.000});
            }, 0.1f, "sdfdf");
            _camera->scheduleOnce([this](float dt){setCameraRelativePosture({-0.030,0.000,0.000,1.000});}, 0.2f, "sdfdf2");

            actionJumpAir();
        }
        if (_roadIter->_type == RollNode::RT_UNREAL) {
            //掉落
            _state = SH_AIR;
            _airSpeed = _posture * Vec3{0, 0, -_roadSpeed};

            // 镜头姿态动画
            float POSTURE_DEGREE = 3.f + _roadSpeed/60.f*3;
            setCameraRelativePosture(rotation2quaternion({rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE}));

            actionMissAir();
        }
    }

    if (_state == SH_ROAD && _roadSpeed == 0) {
        if (_cameraSmoothIndex >= 1.f) {
            // 静止时镜头运动起来
            if (!_scatCameraAniContinue) {
                _scatCameraAniContinue = true;
                _scatCameraAniCurrent = {0,0,0};
                const float POSTURE_DEGREE = 0.5f;
                _scatCameraAniStep = {rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, 0};
            }
            _scatCameraAniCurrent += _scatCameraAniStep;
            if (_scatCameraAniCurrent.length() > 15)
                    _scatCameraAniStep = -1*_scatCameraAniStep;
            if (_scatCameraAniCurrent.length() < _scatCameraAniStep.length()) {
                _scatCameraAniContinue = false;
            }
            setCameraRelativePosture(rotation2quaternion(_scatCameraAniCurrent));
        }
    } else {
        if (_scatCameraAniContinue) {
            setCameraRelativePosture(CAMERA_NORMAL_POSTURE);
        }
        _scatCameraAniContinue = false;
    }

    // 镜头在跑动时微动
    if (_state == SH_ROAD && _roadSpeed > 2.5f) {
        float idx =  (std::sin(_shaderTime)+1.f)/2;

        if (std::abs(idx) < 0.001) {
            float POSTURE_DEGREE = 1.f + _roadSpeed/60.f*2;
            _cameraMicroMoveAniTarget = {rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE, (rand_0_1()*2*POSTURE_DEGREE-POSTURE_DEGREE)*2};
        }

        _cameraMicroMovePosture = rotation2quaternion( (_cameraMicroMoveIndex > 0.f ? 1.f : -1.f )*_cameraMicroMoveAniUniform.lerp(_cameraMicroMoveAniTarget, idx));
    }

    // Camera 位置
    if (_heightAniIndex < 2.f) {
        if (_heightAniIndex <= 1.f) {
            _heightCurrent = _heightStart + (_heightTarget - _heightStart) * _heightAniIndex;

            _heightAniIndex += 0.2f;
        } else {
            _heightCurrent = _heightTarget + (HEIGHT - _heightTarget) * (_heightAniIndex-1.f);
            _heightAniIndex += 0.05f;
        }
    } else {
        _heightCurrent = HEIGHT;
    }
    _headPosition = _position + _posture * Vec3{0, _heightCurrent + _aniCameraHeightPlus, 0};
    _camera->setPosition3D(_headPosition);

    // Camera姿态
    if (_cameraSmoothIndex < 1.f) {
        _cameraSmoothIndex += _state == SH_AIR ? _cameraSmoothStepAir :  _cameraSmoothStep;
        if (_cameraSmoothIndex > 1.f) _cameraSmoothIndex = 1.f;

        Quaternion::slerp(_cameraStartPosture, _cameraTargetPosture, _cameraSmoothIndex, &_cameraCurrentPosture);
    }
    _camera->setRotationQuat(_cameraCurrentPosture * _cameraMicroMovePosture);



    _isLastTouch = _uistate->_isTouch;
    _shaderTime += dt;
    // Hero与dynamicGlass的碰撞 及生命驱动。
    heroDynamicGlassHitCheckAndLive(dt);

    _drawNodeRoadCurrent->step(_shaderTime);
    _drawNodeRoadNext->step(_shaderTime);
    if (_drawNodeRoadPrev) _drawNodeRoadPrev->step(_shaderTime);

    for (auto g : _dynamicGatesPrev) g->step(_shaderTime);
    for (auto g : _dynamicGatesCurrent) g->step(_shaderTime);
    for (auto g : _dynamicGatesNext) g->step(_shaderTime);
    for (int i = 0; i < NHitGlass; i++) _cameraSpaceHitGlassLevels[i]->step(dt);

    _drawNodeRoadNext->setZOrder(100);
    _drawNodeRoadCurrent->setZOrder(99);
    if (_drawNodeRoadPrev) _drawNodeRoadPrev->setZOrder(98);

    if (std::get<0>(_assetNodePrev)) {
        std::get<0>( _assetNodePrev)->setZOrder(98);
        std::get<1>( _assetNodePrev)->setZOrder(98);
        std::get<2>( _assetNodePrev)->setZOrder(98);
    }
    std::get<0>( _assetNodeNext)->setZOrder(100);
    std::get<1>( _assetNodeNext)->setZOrder(100);
    std::get<2>( _assetNodeNext)->setZOrder(100);
    std::get<0>( _assetNodeCurrent)->setZOrder(99);
    std::get<1>( _assetNodeCurrent)->setZOrder(99);
    std::get<2>( _assetNodeCurrent)->setZOrder(99);
    _camera->setZOrder(10001);
    _acceEffect->step(dt);

    // 只有在空中按下状态才有acce线条效果.
    _acceEffect->config((_state == SH_AIR && _uistate->_isTouch) || _acceLeftTime > 0.f, _state == SH_ROAD ? Vec3{0, 0, -_roadSpeed} : _cameraCurrentPosture.getInversed()*_airSpeed);

    _pixelPlaneSky->step(_shaderTime);
    _pixelPlaneSky->setPosition3D(_position+Vec3{0,30,0});
    _pixelPlaneLand->step(_shaderTime);
    _pixelPlaneLand->setPosition3D(_position+Vec3{0,-30,0});

    if (std::get<0>(_assetNodePrev)) {
        std::get<0>( _assetNodePrev)->step(_shaderTime);
        std::get<1>( _assetNodePrev)->step(_shaderTime);
        std::get<2>( _assetNodePrev)->step(_shaderTime);
    }
    if (std::get<0>(_assetNodeCurrent)) {
        std::get<0>( _assetNodeCurrent)->step(_shaderTime);
        std::get<1>( _assetNodeCurrent)->step(_shaderTime);
        std::get<2>( _assetNodeCurrent)->step(_shaderTime);
    }
    if (std::get<0>(_assetNodeNext)) {
        std::get<0>( _assetNodeNext)->step(_shaderTime);
        std::get<1>( _assetNodeNext)->step(_shaderTime);
        std::get<2>( _assetNodeNext)->step(_shaderTime);
    }

    powerStep(dt);
    updateColorPowerEffect();
    uiViewsStep(dt);
    acceStep(dt);

    // 死亡生命控制air空中时间计算
    if (!_win && !_dead) {
        if (_state == SH_AIR) {
            _airTime += dt;
            if (_airTime > AIR_TIME_DEAD) {
                _dead = true;
                actionGameOver();
            }
        } else {
            _airTime = 0;
        }
    }
}

// 四元数非严格等于判断。
// 因为实际上是必须先normalized的，但是我们这边不需要。
inline bool roughQuaEqual(const cocos2d::Quaternion& a, const cocos2d::Quaternion& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

void SuperHero::setPosture(const cocos2d::Quaternion& newPosture)
{
    if (!roughQuaEqual(_posture, newPosture)) {
        _posture = newPosture;
        setupCameraSmoothAnimation();
    }
}
void SuperHero::setCameraRelativePosture(const cocos2d::Quaternion& newPosture)
{
    if (!roughQuaEqual(_cameraRelativePosture, newPosture)) {
        _cameraRelativePosture = newPosture;
        setupCameraSmoothAnimation();
    }
}

void SuperHero::setupCameraSmoothAnimation()
{
    _cameraStartPosture = _cameraCurrentPosture;
    _cameraTargetPosture = _posture*_cameraRelativePosture;
    _cameraSmoothIndex = 0.f;
}

void SuperHero::setupHeightAnimation(float targetHeight)
{
    _heightAniIndex = 0.f;
    _heightStart = _heightCurrent;
    _heightTarget = targetHeight;
}

float SuperHero::getSpeed()
{
    return _state == SH_AIR ? std::abs((_posture.getInversed() * _airSpeed).z) : _roadSpeed;
}

float SuperHero::getPercent()
{
    return _minmapIndex*1.0/_maxmap->getEndIndex();
}

void SuperHero::heroDynamicGlassHitCheckAndLive(float dt)
{
    // Hero和DynamicGlass的碰撞检查，无论是在road还是air都会检查。
    // Hero主要以_position, posture来计算碰撞线段。
    // Hero的碰撞线段改为在身体前方斜向交叉。
    // "前部"是为了，让glass能够在眼前hit，而不是在视线外。

    // 计算Hero的碰撞线段，两个点。
    const float TEST_SEGMENT_LENGTH = 1.f;
    Vec3 heroPointNearFoot = _position + _posture * Vec3{0, HEIGHT/3, 0};
    Vec3 heroPointNearHead = _position + _posture * Vec3{0, HEIGHT, 0};

    Vec3 heroPointFarFoot = _position + _posture * Vec3{0, HEIGHT/3, -TEST_SEGMENT_LENGTH};
    Vec3 heroPointFarHead = _position + _posture * Vec3{0, HEIGHT, -TEST_SEGMENT_LENGTH};

    // 线段与所有显示的Glass碰撞测试
    // TODO hit的统计计数等内容。
//    float speed = _state == SH_AIR ? _airSpeed.length() : _roadSpeed;
    bool hit = false;
    for (auto g : _dynamicGlassesPrev) {
        hit = hit || g->checkHit(heroPointNearFoot, heroPointFarHead) || g->checkHit(heroPointNearHead, heroPointFarFoot);
        g->step(_shaderTime);
    }
    for (auto g : _dynamicGlassesCurrent) {
        hit = hit || g->checkHit(heroPointNearFoot, heroPointFarHead) || g->checkHit(heroPointNearHead, heroPointFarFoot);
        g->step(_shaderTime);
    }
    for (auto g : _dynamicGlassesNext) {
        hit = hit || g->checkHit(heroPointNearFoot, heroPointFarHead) || g->checkHit(heroPointNearHead, heroPointFarFoot);
        g->step(_shaderTime);
    }
    if (hit) {
        CCLOG("dynamic glass hit");
        actionHitGlass();
    }

}

void SuperHero::initCameraSpace()
{
    // 碰到glass时的动画sp
    for (int i = 0; i < NHitGlass; i++) {
        auto _glass = DynamicGlass::create(1.2,1.2, (i+1)*5,(i+1)*5, Color4F{1,0,1,0.5});
        _glass->setCameraMask(_camera->getCameraMask());
        _camera->addChild(_glass);
        _glass->setPosition3D({0,0,0});
        _glass->setVisible(false);
        _cameraSpaceHitGlassLevels[i] = _glass;
    }

    _acceEffect = AcceEffectSpace::create();
    _acceEffect->setCameraMask(_camera->getCameraMask());
    _acceEffect->setPosition3D({0,0,0});
    _camera->addChild(_acceEffect);

}

void SuperHero::aniHitGlass()
{
    for (int i = 0; i < NHitGlass; i++) {
        _cameraSpaceHitGlassLevels[i]->stopAllActions();
        _cameraSpaceHitGlassLevels[i]->setRotation(360*rand_0_1());
        _cameraSpaceHitGlassLevels[i]->reset();
        _cameraSpaceHitGlassLevels[i]->hit(1.0);
        _cameraSpaceHitGlassLevels[i]->setVisible(true);
        _cameraSpaceHitGlassLevels[i]->runAction(Spawn::create(Sequence::create(DelayTime::create(0.3),Blink::create(0.8, rand_0_1()*50), Hide::create(), NULL), EaseSineIn::create(MoveFromTo::create(1.15, Vec3{0.f,-0.5f,0.f}, Vec3{0, -3.5f - 2.f*rand_0_1(), 0})), NULL));
    }
}

void SuperHero::updateColorPowerEffect()
{
    static const Color4F COL_LOW = Color4F{230/255.,230/255.,230/255.,1};
    static const Color4F COL_GOOD = Color4F{0/255.,200/255.,200/255.,1};
    static const Color4F COL_EXTRA = Color4F{0/255.,240/255.,127/255.,1};
    static const Color4F COL_FINAL =  Color4F{238/255.,232/255.,170/255.,1};

    static const float POW_LOW = 0.2;
    static const float POW_GOOD = 0.25;
    static const float POW_EXTRA = 0.5;
    static const float POW_FINAL = 0.65;

    static const float SP_GOOD = 30;//80
    static const float SP_EXTRA = 45;//100
    static const float SP_FINAL = 60;//130

    static float lastBasePower = POW_LOW;

    float speed = _state == SH_AIR ? std::abs((_posture.getInversed() * _airSpeed).z) : _roadSpeed;
    _globalColor = speed < SP_GOOD ? COL_LOW :
                        speed < SP_EXTRA ? COL_GOOD :
                            speed < SP_FINAL ? COL_EXTRA : COL_FINAL;
    _globalBasePower = speed < SP_GOOD ? POW_LOW :
                            speed < SP_EXTRA ? POW_GOOD :
                                speed < SP_FINAL ? POW_EXTRA : POW_FINAL;

    if (_globalBasePower > lastBasePower) {
        actionSpeedup();
    }
    lastBasePower = _globalBasePower;

    float power = std::max(_globalBasePower, _globalInstantPower);
    // sett color
    _drawNodeRoadCurrent->aniRoadColor(_globalColor);
    _drawNodeRoadNext->aniRoadColor(_globalColor);
    if (_drawNodeRoadPrev) _drawNodeRoadPrev->aniRoadColor(_globalColor);

    _drawNodeStaticGlassCurrent->setLineColor(_globalColor);
    _drawNodeStaticGlassNext->setLineColor(_globalColor);
    if (_drawNodeStaticGlassPrev) _drawNodeStaticGlassPrev->setLineColor(_globalColor);

    std::get<0>(_assetNodeCurrent)->aniRoadColor(_globalColor);
    std::get<1>(_assetNodeCurrent)->aniRoadColor(_globalColor);
    std::get<2>(_assetNodeCurrent)->aniRoadColor(_globalColor);
    std::get<0>(_assetNodeNext)->aniRoadColor(_globalColor);
    std::get<1>(_assetNodeNext)->aniRoadColor(_globalColor);
    std::get<2>(_assetNodeNext)->aniRoadColor(_globalColor);
    if (std::get<0>(_assetNodePrev)) {
        std::get<0>(_assetNodePrev)->aniRoadColor(_globalColor);
        std::get<1>(_assetNodePrev)->aniRoadColor(_globalColor);
        std::get<2>(_assetNodePrev)->aniRoadColor(_globalColor);
    }

    for (auto g : _dynamicGatesPrev) {g->aniRoadColor(_globalColor); g->setPower(power);}
    for (auto g : _dynamicGatesCurrent) {g->aniRoadColor(_globalColor); g->setPower(power);}
    for (auto g : _dynamicGatesNext) {g->aniRoadColor(_globalColor); g->setPower(power);}

    _acceEffect->ani2color(_globalColor);

    std::get<0>(_assetNodeCurrent)->setPower(power);
    std::get<1>(_assetNodeCurrent)->setPower(power);
    std::get<2>(_assetNodeCurrent)->setPower(power);
    std::get<0>(_assetNodeNext)->setPower(power);
    std::get<1>(_assetNodeNext)->setPower(power);
    std::get<2>(_assetNodeNext)->setPower(power);
    if (std::get<0>(_assetNodePrev)) {
        std::get<0>(_assetNodeCurrent)->setPower(power);
        std::get<1>(_assetNodeCurrent)->setPower(power);
        std::get<2>(_assetNodeCurrent)->setPower(power);
    }

    _drawNodeRoadCurrent->setPower(power);
    _drawNodeRoadNext->setPower(power);
    if (_drawNodeRoadPrev) _drawNodeRoadPrev->setPower(power);

}

void SuperHero::hitInstantPower()
{
    _powerAning = true;
    _powerGrowing = true;
}

void SuperHero::powerStep(float dt)
{
    static const float power_step_grow = 10;
    static const float power_step_minus = 3;
    static const float power_max = 1.0;
    if (_powerAning) {
        if (_powerGrowing) {
            _globalInstantPower += power_step_grow * dt;
            if (_globalInstantPower > power_max) {
                _globalInstantPower = power_max;
                _powerGrowing = false;
            }
        } else {
            _globalInstantPower -= power_step_minus * dt;
            if (_globalInstantPower < 0.0) {
                _globalInstantPower = 0.0;
                _powerGrowing = true;
                _powerAning = false;
            }
        }
    }
}

void SuperHero::actionHitGlass()
{
    aniHitGlass();
    hitInstantPower();

    // speed显示1秒钟为meta色。
    _speedLbGlassEffectTimeLeft = 1.f;
    // 同时加速5码
    if (_state == SH_AIR) {
        _airSpeed += _posture * Vec3{0,0,-getHitGlassAcce()};
    } else {
        _roadSpeed += getHitGlassAcce();
    }
    if (_sfxEnable)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(fmt::sprintf("sounds/action_hitglass/c0/%d.mp3", static_cast<int>(rand_0_1()*100)%4).c_str());
    _hitDots++;
}

void SuperHero::actionLandRoad()
{
    hitInstantPower();
    if (_sfxEnable && !_win)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(fmt::sprintf("sounds/action_land/%d.mp3", static_cast<int>(rand_0_1()*100)%4).c_str());

}

void SuperHero::actionMissAir()
{
//    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SFG_GAME_ACTION_MISS.c_str());

}

void SuperHero::actionJumpAir()
{
//    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SFG_GAME_ACTION_JUMP.c_str());
}

void SuperHero::actionSpeedup()
{
    if (_sfxEnable && !_win && !_dead)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/action_speedup.wav");
}

cocos2d::Vec2 SuperHero::genPos(const cocos2d::Vec2& pos)
{
    auto size = Director::getInstance()->getVisibleSize();
    return {size.width * pos.x, size.height * pos.y};
}
void SuperHero::initUIViews()
{
    //Pause icon
    _lbPauseIcon = Label::createWithTTF("II","fonts/s/slkscr.ttf" , 40);
    _uiLayer->addChild(_lbPauseIcon);
    _lbPauseIcon->setPosition3D(genPos3D({0.5,0.025}, 0));
    _lbPauseIcon->setOpacity(128/2);

    _lbSpeed = Label::createWithTTF("", "fonts/s/superaircraft.ttf", 75);
//    _lbSpeed->setHorizontalAlignment(TextHAlignment::RIGHT);
    _lbSpeed->setPosition(genPos({0.5,0.9}));
    _uiLayer->addChild(_lbSpeed);

    _lbJindu = Label::createWithTTF("", "fonts/s/BPdotsCondensed.ttf", 40);
//    _lbJindu->setHorizontalAlignment(TextHAlignment::LEFT);
    _lbJindu->setPosition(genPos({0.5, 0.95}));
    _uiLayer->addChild(_lbJindu);

    _lbPts = Label::createWithTTF("", "fonts/s/BPdotsCondensed.ttf", 40);
    auto size = Director::getInstance()->getWinSize();
    PixelPerLine = size.width / PerPixelWidth + 1;
    _lbPts->setWidth(size.width);
    _lbPts->setHeight(size.height);
    _lbPts->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
    _lbPts->setPosition(genPos({0.5, 0.5}));
    _uiLayer->addChild(_lbPts);

    _lbPtsHistory = Label::createWithTTF("", "fonts/s/BPdotsCondensed.ttf", 40);
    _lbPtsHistory->setWidth(size.width);
    _lbPtsHistory->setHeight(size.height);
    _lbPtsHistory->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);

    _lbPtsHistory->setPosition(genPos({0.5, 0.5}));
    _uiLayer->addChild(_lbPtsHistory);
    _lbPtsHistory->setOpacity(100);

    if (_isInfinite) {
        int numOld = _historyBestPts/PerPixelPts;
        std::stringstream ss;

        for (int i = 0; i < numOld; i++) {
            if (i < numOld) {ss << "|";}
            if (i%PixelPerLine == PixelPerLine-1) {
                ss << "\n";
            }
        }
        _lbPtsHistory->setString(ss.str());
    }
    float anit = 0.25f;
    _lbSpeed->setOpacity(0);
    _lbSpeed->runAction(FadeIn::create(anit));
    _lbPauseIcon->setOpacity(0);
    _lbPauseIcon->runAction(FadeTo::create(anit, 128/2));

    if (!_isInfinite) {
        _lbJindu->setOpacity(0);
        _lbJindu->runAction(FadeIn::create(anit));
    } else {
        _lbPtsHistory->setOpacity(0);
        _lbPtsHistory->runAction(FadeTo::create(anit, 100));
    }
}

inline Color4B color4f_to_color4b(const Color4F& c4f)
{
    return  {static_cast<GLubyte>(c4f.r * 255),
                static_cast<GLubyte>(c4f.g * 255),
                static_cast<GLubyte>(c4f.b * 255),
                static_cast<GLubyte>(c4f.a * 255)};
}

void SuperHero::uiViewsStep(float dt)
{
    if (_win) {
        _aniCameraHeightPlus += dt*5.f;
    }
    if (_dead || _win) return;
    float speedNow = getSpeed()*SPEED_SHOW_RADIO;
    if (speedNow > _maxSpeed) {
        _maxSpeed = speedNow;
    }
    _runLength += dt * speedNow;
    _timeCost += dt;
    _lbSpeed->setString(fmt::sprintf("%.1fm/s", speedNow));

    if (_isInfinite == false) {
        float per = getPercent();
        const int N = 30;
        std::stringstream ss;
        ss << "[";
        for (int i = 0; i < N*per; i++) {
            ss << "|";
        }
        for (int i = 0; i < N - (int)(N*per); i++) {
            ss << " ";
        }
        ss << "]";
        _lbJindu->setString(ss.str());
        _lbJindu->setTextColor(color4f_to_color4b(_globalColor));
    }
    _lbJindu->setVisible(_isInfinite == false);
    if (_speedLbGlassEffectTimeLeft > 0.f) {
        _lbSpeed->setTextColor(Color4B{255,52,179,255});
        _speedLbGlassEffectTimeLeft -= dt;
    } else {
        _lbSpeed->setTextColor(color4f_to_color4b(_globalColor));
    }

    _lbPts->setVisible(_isInfinite);
    _lbPtsHistory->setVisible(_isInfinite);

    if (_isInfinite) {
        _lbPts->setTextColor(color4f_to_color4b(_globalColor));
        _lbPtsHistory->setTextColor(color4f_to_color4b(_globalColor));

        int pts = calcPts();
        int numNow = pts/PerPixelPts;
        int numOld = _historyBestPts/PerPixelPts;
        int target = std::max(numNow, numOld);
        std::stringstream ssNow;

        for (int i = 0; i < target; i++) {
            if (i < numNow) {ssNow << "|";}
            if (i%PixelPerLine == PixelPerLine-1) {
                ssNow << "\n";
            }
        }

        _lbPts->setString(ssNow.str());
    }

    _lbPauseIcon->setTextColor(color4f_to_color4b(_globalColor));

}

void SuperHero::acceStep(float dt)
{
    _acceLeftTime -= dt;
}

void SuperHero::aniAcce(float time)
{
    _acceLeftTime = time;
}

void SuperHero::actionGameWin()
{
    CCLOG("game win");
    // 用白色笼罩

    // 开始结算
    aniGameWinSettle();
}


void SuperHero::actionGameOver()
{
    CCLOG("game over");
    aniGameOverSettle();
}

void SuperHero::initSettlementViews()
{

    //结算的相关view的初始化
    auto gen = [this](const std::string& text, const Vec2& pos, int size = 40)->cocos2d::Label*{
        auto sp =  Label::createWithTTF(text, TRLocale::s().font(), size);
        sp->setPosition3D(genPos3D(pos, 0));
        _uiLayer->addChild(sp);
        _settleMentViews.push_back(sp);
        sp->setVisible(false);
        return sp;
    };
    float top = 0.80f;
    float step = 0.075f;
    float stepRect = 0.1f;
    float now = top;
    _lbChapterName = gen("chapter", {0.5, 0.95}, 30);

    _lbMapName = gen("name", {0.5,now}, 50);
    now -= step;
    _lbMaxSpeed = gen("max speed", {0.5, now});
    now -= step;
    _lbTimeCost = gen("time", {0.5, now});
    now -= step;
    _lbRunLength = gen("run lenght", {0.5, now});
    now -= step;
    _lbDynamicGlass = gen("dynamic dots", {0.5, now});
    now -= stepRect;
    _lbTotalScore = gen("total score", {0.5, now});
    now -= stepRect;
    _lbBtnFirst = gen("btn first", {0.5, now});
    now -= stepRect;
    _lbBtnSecond = gen("btn second", {0.5, now});

    auto makeRect = [this](const Vec3& pos)->TRDrawNode3D* {
        auto sp = TRDrawNode3D::create();
        sp->setPosition3D(pos);
        _uiLayer->addChild(sp);
        _settleMentViews.push_back(sp);
        float hw = 300.f;
        float hh = hw/6.f;
        sp->drawLine(Vec3{-hw, -hh, 0}, Vec3{hw, -hh, 0});
        sp->drawLine(Vec3{-hw, hh, 0}, Vec3{hw, hh, 0});
        sp->drawLine(Vec3{-hw, hh, 0}, Vec3{-hw, -hh, 0});
        sp->drawLine(Vec3{hw, hh, 0}, Vec3{hw, -hh, 0});
        sp->setVisible(false);
//        sp->setZOrder(-100);
        return sp;
    };

    _rectScore = makeRect(_lbTotalScore->getPosition3D());
    _rectBtnFirst = makeRect(_lbBtnFirst->getPosition3D());
    _rectBtnSecond = makeRect(_lbBtnSecond->getPosition3D());

    _spWhiteShadow = Sprite::create("images/white_shadow.png");
    _uiLayer->addChild(_spWhiteShadow);
    _spWhiteShadow->setPosition3D(genPos3D({0.5,0.5}, 0));
    _spWhiteShadow->setScale(1000.f);
    _spWhiteShadow->setVisible(false);
    _spWhiteShadow->setZOrder(-123);

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){

        return true;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){

    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        if (_settlementBtnEffect) {
            if (this->isRectPointerIn(_rectBtnFirst, 300.f, 300.f/5.f, touch->getLocation())){
                this->clickBtnFirst();
            }
            if (this->isRectPointerIn(_rectBtnSecond, 300.f, 300.f/5.f, touch->getLocation())) {
                this->clickBtnSecond();
            }
        } else {
            if (!_paused && this->isRectPointerIn(_lbPauseIcon, 500.f, 25.f, touch->getLocation())) {
                aniPause();
            }
        }
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _spWhiteShadow->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _uiLayer);
}

bool SuperHero::isRectPointerIn(Node* rect,  float hw, float hh, const cocos2d::Vec2& pos)
{
    CCLOG("%f %f %f %f", pos.x, pos.y, rect->getPositionX(), rect->getPositionY());
    if (pos.x > rect->getPositionX()-hw && pos.x < rect->getPositionX()+hw &&
        pos.y > rect->getPositionY()-hh && pos.y < rect->getPositionY()+hh ) {
        return  true;
    }
    return  false;
}

void SuperHero::aniSettlements(bool iswin)
{
    // 隐藏进度和速度
    _lbJindu->runAction(FadeOut::create(0.5f));
    _lbSpeed->runAction(FadeOut::create(0.5f));
    _lbPts->runAction(FadeOut::create(0.5f));
    _lbPtsHistory->runAction(FadeOut::create(0.5f));

    //显示通用结算
    Color4B tcol = iswin ? Color4B::BLACK : Color4B::WHITE;

    auto setlb = [](cocos2d::Label* lb, const std::string& text, float delay, const Color4B& col){
        if (text.length() > 0)
            lb->setString(text);
        lb->setVisible(true);
        lb->setOpacity(0);
        lb->setTextColor(col);
        lb->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.5f), NULL));

    };

    auto anirect = [](TRDrawNode3D* n, float delay, bool iswin){
        n->setVisible(true);
        n->setOpacity(0);
        n->setLineColor(iswin ? Color4F::BLACK : Color4F::WHITE);
        n->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.5f), NULL));
    };

#ifdef DESP_CHAPTER_NAME
    setlb(_lbChapterName, fmt::sprintf("-- %s  --", EditState::s()->getChapterName().c_str()), 0.f, tcol);
#endif
    setlb(_lbMapName, EditState::s()->getSectionName(), 0.f, tcol);
    setlb(_lbMaxSpeed, fmt::sprintf("Max Speed: %.1f m/s[x10]", _maxSpeed), 0.25f, tcol);
    setlb(_lbTimeCost, fmt::sprintf("Time: %0.1f secs[x5]", _timeCost), 0.5f, tcol);
    setlb(_lbRunLength, fmt::sprintf("%.0f m", _runLength), 0.75f, tcol);
    setlb(_lbDynamicGlass, fmt::sprintf("%d dots[x100]", _hitDots), 1.f, Color4B::MAGENTA);
    setlb(_lbTotalScore, fmt::sprintf("∑= %d pts", 0), 1.25f, tcol);
    anirect(_rectScore, 1.25f, iswin);

    _targetPts = calcPts();
    TRLocale::s().updateSectionPts(EditState::s()->getSectionId(), _isInfinite, _targetPts);
    _ptsNow = 0;
    _lbTotalScore->schedule([this](float dt){
        _ptsNow += 137;
        if (_ptsNow > _targetPts) {
            _ptsNow = _targetPts;
        }
        _lbTotalScore->setString(fmt::sprintf("∑= %d pts", _ptsNow));
    }, 0.016,_targetPts/103, 1.25f, "k001");
}

int SuperHero::calcPts()
{
    return _runLength + 100 * _hitDots + _timeCost*5 + _maxSpeed*10;
}

void SuperHero::aniGameOverSettle()
{
    CCLOG("ani game over");
    aniSettlements(false);

    auto aniBtn = [](Label* lbbb, TRDrawNode3D* rect,  const std::string& text, float delay, bool iswin) {
        lbbb->setString(text);
        lbbb->setColor(iswin ? Color3B::BLACK : Color3B::WHITE);
        lbbb->setVisible(true);
        lbbb->setOpacity(0);
        lbbb->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.25f), NULL));
        rect->setVisible(true);
        rect->setLineColor(iswin ? Color4F::BLACK : Color4F::WHITE);
        rect->setOpacity(0);
        rect->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.25f), NULL));
    };

    aniBtn(_lbBtnFirst, _rectBtnFirst, TRLocale::s()["restart"], 1.5f, false);
    aniBtn(_lbBtnSecond, _rectBtnSecond, TRLocale::s()["to_menu"], 1.75f, false);
    _spWhiteShadow->scheduleOnce([this](float dt){_settlementBtnEffect = true;}, 1.5f, "dead2" );

}

void SuperHero::aniGameWinSettleAst()
{
    aniSettlements(true);
    TRLocale::s().setSectionPassed(EditState::s()->getSectionId(), _isInfinite, true);

    auto aniBtn = [](Label* lbbb, TRDrawNode3D* rect,  const std::string& text, float delay, bool iswin) {
        lbbb->setString(text);
        lbbb->setColor(iswin ? Color3B::BLACK : Color3B::WHITE);
        lbbb->setVisible(true);
        lbbb->setOpacity(0);
        lbbb->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.25f), NULL));
        rect->setVisible(true);
        rect->setLineColor(iswin ? Color4F::BLACK : Color4F::WHITE);
        rect->setOpacity(0);
        rect->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.25f), NULL));
    };
    auto& sec = TRStories::s().getSection(EditState::s()->getSectionId());
    if (sec._hasNext) {
        aniBtn(_lbBtnFirst, _rectBtnFirst, TRLocale::s()["next_section"], 1.5f, true);
    }
    aniBtn(_lbBtnSecond, _rectBtnSecond, TRLocale::s()["to_menu"], 1.75f, true);
}

void SuperHero::aniGameWinSettle()
{
    _spWhiteShadow->setVisible(true);
    _spWhiteShadow->setOpacity(0);
    _spWhiteShadow->runAction(Sequence::create(DelayTime::create(1.f),  FadeIn::create(2.f), NULL));

    _spWhiteShadow->scheduleOnce([this](float dt){
        this->aniGameWinSettleAst();
    }, 2.5f, "win" );
    _spWhiteShadow->scheduleOnce([this](float dt){_settlementBtnEffect = true;}, 4.5f, "win2" );
}

void SuperHero::clickBtnFirst()
{
    if (_paused) {
        auto aniBtn = [](Label* lbbb, TRDrawNode3D* rect) {
            lbbb->runAction(Sequence::create(FadeOut::create(0.5f),Hide::create(), NULL));
            rect->runAction(Sequence::create( FadeIn::create(0.5f),Hide::create(), NULL));
        };
        aniBtn(_lbBtnFirst, _rectBtnFirst);
        aniBtn(_lbBtnSecond, _rectBtnSecond);
        _settlementBtnEffect = false;
        _spWhiteShadow->scheduleOnce([this](float dt){
            _paused = false;
            if (_sfxEnable)
                CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
            if (_musicEnable) 
                CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
        }, 0.5f, "pause3" );

    } else if (_win) {
        // next section
        auto& sec = TRStories::s().getSection(EditState::s()->getSectionId());
        EditState::s()->config(sec._nextSid, false);
        Director::getInstance()->replaceScene(MenuScene::create(MENU_LOADING));

    } else {
        // restart
        CCLOG("todo restart");
        CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        Director::getInstance()->replaceScene(TurnScene::create());
    }
    if (_sfxEnable)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/menu_btn.mp3");
}

void SuperHero::clickBtnSecond()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    if (_sfxEnable)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/menu_btn.mp3");
    Director::getInstance()->replaceScene(MenuScene::create());
    CCLOG("fanhui");
}

void SuperHero::aniPause()
{
    if (_win || _dead) {
        return;
    }
    _isLastTouch = false;
    _paused = true;
    auto aniBtn = [](Label* lbbb, TRDrawNode3D* rect,  const std::string& text, float delay, bool iswin) {
        lbbb->setString(text);
        lbbb->setColor(iswin ? Color3B::BLACK : Color3B::WHITE);
        lbbb->setVisible(true);
        lbbb->setOpacity(0);
        lbbb->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.5f), NULL));
        rect->setVisible(true);
        rect->setLineColor(iswin ? Color4F::BLACK : Color4F::WHITE);
        rect->setOpacity(0);
        rect->runAction(Sequence::create(DelayTime::create(delay), FadeIn::create(0.5f), NULL));
    };
    aniBtn(_lbBtnFirst, _rectBtnFirst, TRLocale::s()["pause_return_game"], 0.f, false);
    aniBtn(_lbBtnSecond, _rectBtnSecond, TRLocale::s()["pause_return_menu"], 0.5f,false);
    _spWhiteShadow->scheduleOnce([this](float dt){_settlementBtnEffect = true;}, 0.5f, "pause2" );

    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();

    if (_sfxEnable)
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/menu_btn.mp3");
}
