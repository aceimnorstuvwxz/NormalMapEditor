// (C) 2015 Turnro.com

#include "BattleField.h"
#include "format.h"
#include "DDMapData.h"
#include "TRLocale.h"
#include "DDUpgradeLoad.h"

USING_NS_CC;


// LightManager
void LightNodeManager::init()
{
    for (int i = 0; i < DDConfig::NUM_LIGHT; i++) {
        _lightSet.push_back(&_lights[i]);
    }
}

LightNode* LightNodeManager::brrow()
{
    if (!_lightSet.empty()) {
        LightNode* ret = _lightSet.back();
        _lightSet.pop_back();
        return  ret;
    } else {
        return nullptr;
    }
}

void LightNodeManager::giveback(LightNode *node)
{
    assert(node != nullptr);
    node->quality = 0;
    _lightSet.push_back(node);
}

LightNode* LightNodeManager::getLights()
{
    return _lights;
}

// BattleField
void BattleField::init(cocos2d::Layer *battleLayer)
{
    _battleLayer = battleLayer;

    _lightManager.init();

    initMovingCover();

    auto sp = Sprite::create("images/battle_field_bg.png");
    sp->setScale(DDConfig::battleAreaRect().size.width/sp->getContentSize().width);
    sp->setZOrder(BattleField::Z_TEST_BG);
    _battleLayer->addChild(sp);
/*
    auto testLow = BattleLowNode::create();
    testLow->setBuildingBg("images/test_building_bg.png");
    testLow->setFieldBg("images/test_field_bg.png");
    testLow->setPosition({0,0});
    testLow->setZOrder(BattleField::Z_LOW_NODE);
    _battleLayer->addChild(testLow);

    auto testMiddle = BattleMiddleNode::create();
    testMiddle->configShadowFile("images/test_middle_shadow.png");
    testMiddle->setPosition({0,0});
    testMiddle->setZOrder(BattleField::Z_MIDDLE_NODE);
    _battleLayer->addChild(testMiddle);
    _testMiddleNode = testMiddle;
    _testMiddleNode->retain();
    testMiddle->configSelection(1);
    testMiddle->configScopeLine({1,1,1,1});

    auto testHigh = BattleHighNode::create();
    testHigh->configBuildingAnimation("images/test_high_building_animation.png");
    testHigh->configBuildingNormalMap("images/test_high_building_normalmap.png");
    testHigh->setZOrder(BattleField::Z_HIGH_NODE);
    _battleLayer->addChild(testHigh);
    _testHighNode = testHigh;*/

    for (int i = 0; i < DDConfig::BATTLE_NUM; i++) {
        for (int j = 0; j < DDConfig::BATTLE_NUM; j++) {
            AgentPos agentPos = {i - DDConfig::BATTLE_NUM/2,j - DDConfig::BATTLE_NUM/2};
            _agentNodeMap[agentPos] = &_rawAgentNodes[j*DDConfig::BATTLE_NUM+i];
            AgentNode* pNode = _agentNodeMap[agentPos];
            pNode->init(_battleLayer);
            pNode->configAgentPos(agentPos);
            pNode->configAgentFloatPos(BattleField::help_agentPos2agentFloatPos(agentPos));
//            pNode->configVisible(false);
        }
    }

    _shadowCover = ShadowCover::create();
    _shadowCover->setZOrder(Z_SHADOW_COVER);
    _shadowCover->setPosition({0,0});
    _battleLayer->addChild(_shadowCover);
    _shadowCover->setVisible(false);

    _redGreenCover = RedGreenCover::create();
    _redGreenCover->setZOrder(Z_REDGREEN_COVER);
    _redGreenCover->setPosition({0,0});
    _battleLayer->addChild(_redGreenCover);
    _redGreenCover->dismiss();

    _distanceBox = DistanceBox::create();
    _distanceBox->setZOrder(Z_DISTANCE_BOX);
    _distanceBox->setPosition({0,0});
    _battleLayer->addChild(_distanceBox);
    _distanceBox->dismiss();

    addTestLights();

    initTapMessgeThings();
    initMovingNewBuildThings();
}

void BattleField::configAnimationTexture(const std::string& fn)
{
    Director::getInstance()->getTextureCache()->reloadTexture(fn);
    auto texture = Director::getInstance()->getTextureCache()->addImage(fn);
    for (int i = 0; i < DDConfig::BATTLE_NUM; i++) {
        for (int j = 0; j < DDConfig::BATTLE_NUM; j++) {
            AgentPos agentPos = {i - DDConfig::BATTLE_NUM/2,j - DDConfig::BATTLE_NUM/2};
            _agentNodeMap[agentPos]->configAnimation(texture);
        }
    }

}
void BattleField::configAnimationIndex(int index)
{
    for (int i = 0; i < DDConfig::BATTLE_NUM; i++) {
        for (int j = 0; j < DDConfig::BATTLE_NUM; j++) {
            AgentPos agentPos = {i - DDConfig::BATTLE_NUM/2,j - DDConfig::BATTLE_NUM/2};
            _agentNodeMap[agentPos]->configAnimationIndex(index);
        }
    }
}

void BattleField::configShadowVisible()
{
    for (int i = 0; i < DDConfig::BATTLE_NUM; i++) {
        for (int j = 0; j < DDConfig::BATTLE_NUM; j++) {
            AgentPos agentPos = {i - DDConfig::BATTLE_NUM/2,j - DDConfig::BATTLE_NUM/2};
            _agentNodeMap[agentPos]->_middleNode->setVisible(!(_agentNodeMap[agentPos]->_middleNode->isVisible()));
        }
    }
}

Vec2 BattleField::help_battleLayerPos2agentFloatPos(const cocos2d::Vec2& srcPos)
{
    return {srcPos.x / DDConfig::battleCubeWidth(), srcPos.y / DDConfig::battleCubeWidth()};
}

cocos2d::Vec2 BattleField::help_agentPos2agentFloatPos(const AgentPos& agentPos)
{
    return {agentPos.x * DDConfig::battleCubeWidth(), agentPos.y * DDConfig::battleCubeWidth()};
}

void BattleField::step(float dt)
{
    for (auto p: _agentNodeMap) {
        p.second->updateLights(_lightManager.getLights());
    }
    _shadowCover->step(dt);
}


void BattleField::initMovingCover()
{
}

void BattleField::movingCoverIn()
{
}

void BattleField::movingCoverOut()
{
}



void BattleField::op_switch2minmap()
{
    CCLOG("turn2newMinMap");
    float turnInWait = 0.f;
    if (!_isMovingCovered) {
        movingCoverIn();
        turnInWait += 1.5f;
    }

    // 激活检查
    MapPos mappos = DDMapData::s()->battleFieldObservedMinMap;
    if(DDMapData::s()->getMinMaps()[mappos]->blocked) {
        showTapMessage(false);
    } else if (DDMapData::s()->getMinMaps()[mappos]->state == DDMinMap::T_ACTIVABLE) {
        showTapMessage(true);
    } else {
        _battleLayer->scheduleOnce([this](float dt){
            op_switch2minmap_continue();
        }, turnInWait, "turnOut");
    }


}

void BattleField::op_switch2minmap_continue()
{
    this->switchMinMap();
    this->movingCoverOut();
}

void BattleField::switchMinMap()
{
    MapPos mappos = DDMapData::s()->battleFieldObservedMinMap;
    CCLOG("switchMinMap to %d %d", mappos.x, mappos.y);

    // clean all
    for (int y = 0-DDConfig::BATTLE_NUM/2; y <= DDConfig::BATTLE_NUM/2; y++) {
        for (int x = 0-DDConfig::BATTLE_NUM/2; x <= DDConfig::BATTLE_NUM/2; x++) {
            AgentPos pos = {x,y};
            _agentNodeMap[pos]->clear();
        }
    }

    // build up
    auto minmap = DDMapData::s()->getBattleObservedMinMap();

    for (auto pa : minmap->posAgentMap) {
        auto agent = pa.second;
        _agentNodeMap[pa.first]->build(agent->aid, agent->type, agent->level, false, false);
    }

    // 计算视野
    calcVisionMapAndRefreshCover();
}

void BattleField::initTapMessgeThings()
{
    _messageBackgroundImage = Sprite::create("images/battle_message_bg.png");
    _messageBackgroundImage->setPosition({0,0});
    _messageBackgroundImage->setScale(DDConfig::battleAreaRect().size.width/_messageBackgroundImage->getContentSize().width);
//    _messageBackgroundImage->setVisible(false);
    _messageBackgroundImage->setZOrder(Z_MESSAGE_BACKGROUND);
//    _battleLayer->addChild(_messageBackgroundImage);

    _messageLabel = Label::createWithTTF("test message", TRLocale::s().font(), 20*DDConfig::fontSizeRadio());
    _messageLabel->setPosition({0,0});
    _messageLabel->setZOrder(Z_MESSAGE_LABEL);
//    _messageLabel->setVisible(false);
//    _battleLayer->addChild(_messageLabel);

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        auto point = touch->getLocation();
        auto rect = DDConfig::battleAreaRect();
        return _waitingToTapping && rect.containsPoint(point);
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        if (DDConfig::battleAreaRect().containsPoint(touch->getLocation())) {
            // 入侵此MinMap
            _waitingToTapping = false;
            DDMapData::s()->getMinMaps()[DDMapData::s()->battleFieldObservedMinMap]->state = DDMinMap::T_ACTIVE;
            this->dismissTapMessage();
            this->op_switch2minmap_continue();
        }
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

//    _battleLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _battleLayer);

}

void BattleField::showTapMessage(bool isToActive)
{
    _messageBackgroundImage->setVisible(true);
    _messageLabel->stopAllActions();
    if (isToActive) {
        _messageLabel->runAction(Blink::create(1000.f, 1000*2));
    }
    _waitingToTapping = isToActive;
    _messageLabel->setString(TRLocale::s()[isToActive?"battle_tap_to_invade":"battle_its_blocked_minmap"]);

}

void BattleField::dismissTapMessage()
{
    _waitingToTapping = false;
    _messageBackgroundImage->setVisible(false);
    _messageLabel->setVisible(false);
    _messageLabel->stopAllActions();
}

int BattleField::help_getWhichAgentTypeMoveIn(int buildingIndex) // 返回building field的序号对应的friend agent的类型。
{
    int t = DDAgent::AT_FRIEND_WALL;
    switch (buildingIndex) {
        case 0:
            t = DDAgent::AT_FRIEND_WALL;
            break;
        case 1:
            t = DDAgent::AT_FRIEND_MINER;
            break;
        case 2:
            t = DDAgent::AT_FRIEND_ARROW_TOWER;
            break;
        case 3:
            t = DDAgent::AT_FRIEND_MAGIC_TOWER;
            break;
        case 4:
            t = DDAgent::AT_FRIEND_CONNON_TOWER;
            break;
        case 5:
            t = DDAgent::AT_FRIEND_CURE_TOWER;
            break;
        case 6:
            t = DDAgent::AT_FRIEND_LIGHT_TOWER;
            break;
        default:
            assert(false);
            break;
    }
    return t;
}

std::pair<bool, AgentPos> BattleField::help_touchPoint2agentPos(const cocos2d::Vec2& point) // 计算触摸到的battle field内部的坐标
{
    bool isok = false;
    AgentPos retpos;
    auto rect = DDConfig::battleAreaRect();
    if (rect.containsPoint(point)) {
        isok = true;

        float cubewidth = rect.size.width / DDConfig::BATTLE_NUM;
        int rx = (point.x - rect.origin.x)/cubewidth;
        int ry = (point.y - rect.origin.y)/cubewidth;
        retpos = {rx - DDConfig::MINMAP_EXPAND, ry - DDConfig::MINMAP_EXPAND};
    }
    return {isok, retpos};
}


void BattleField::initMovingNewBuildThings()
{

    static bool touch_moved = false;
    static int which_is_selected = 0;
    static int agent_type = 0;
    static int agent_distance = 0;
    static bool ispuutable = false;
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        auto point = touch->getLocation();
        auto rect = DDConfig::buildingAreaRect();
        touch_moved = false;
        if (rect.containsPoint(touch->getLocation())) {
            //算出选中了哪一个
            auto point = touch->getLocation();
            float diffX = point.x - rect.origin.x;
            float widthStep = DDConfig::buildingAreaSelectionWidth();
            which_is_selected = diffX / widthStep;
            agent_type = help_getWhichAgentTypeMoveIn(which_is_selected);
            agent_distance = DDUpgradeRoadMap::getPropertyUpgradeMap(agent_type, PropertyType::ACTION_DISTANCE).start;
            CCLOG("buildingfiled select %d distance %d", which_is_selected, agent_distance);
            return true;
        } else {
            return false;
        }
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        touch_moved = true;
        auto rect = DDConfig::battleAreaRect();
        auto ifagentpos = help_touchPoint2agentPos(touch->getLocation());
        if (ifagentpos.first) {
            CCLOG("moving in at %d %d", ifagentpos.second.x, ifagentpos.second.y);
            // TODO 在视野地图的基础上，显示 红绿层
            if (!_redGreenCover->isVisible()) {
                this->calcRedGreenCoverAndShow();
            }
            if (!_distanceBox->isVisible()) {
                _distanceBox->show();
            }
            ispuutable = _redGreenMap[ifagentpos.second] == RedGreenCover::CS_GREEN;
            _distanceBox->configDistance(agent_distance);

            _distanceBox->configCenter(ifagentpos.second, ispuutable);
        } else {
            if (_redGreenCover->isVisible()) {
                _redGreenCover->dismiss();
            }
            if (_distanceBox->isVisible()) {
                _distanceBox->dismiss();
            }
        }
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        if (_redGreenCover->isVisible()) {
            _redGreenCover->dismiss();
        }
        if (_distanceBox->isVisible()) {
            _distanceBox->dismiss();
        }
        auto ifagentpos = help_touchPoint2agentPos(touch->getLocation());
        if (ifagentpos.first && _redGreenMap[ifagentpos.second]== RedGreenCover::CS_GREEN) {
            DDMapData::s()->appearAgent_friend(ifagentpos.second, agent_type);
        }
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

//    _battleLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _battleLayer);
}

void BattleField::calcVisionMapAndRefreshCover() //计算视野地图，同时更新cover层。
{
    // 清空
    for (int x = -DDConfig::MINMAP_EXPAND; x <= DDConfig::MINMAP_EXPAND; x++) {
        for (int y = -DDConfig::MINMAP_EXPAND; y <= DDConfig::MINMAP_EXPAND; y++) {
            _visionMap[{x, y}] = false;
        }
    }

    // 计算4个门
    MapPos current = DDMapData::s()->battleFieldObservedMinMap;
    const int DOOR_VISION_DISTANCE = 3;
    if (DDMapData::s()->isMinMapActive({current.x, current.y+1})) {
        calcVisionMap_ast({0, DDConfig::MINMAP_EXPAND}, DOOR_VISION_DISTANCE);
    }
    if (DDMapData::s()->isMinMapActive({current.x, current.y-1})) {
        calcVisionMap_ast({0, -DDConfig::MINMAP_EXPAND}, DOOR_VISION_DISTANCE);
    }
    if (DDMapData::s()->isMinMapActive({current.x+1, current.y})) {
        calcVisionMap_ast({DDConfig::MINMAP_EXPAND, 0}, DOOR_VISION_DISTANCE);
    }
    if (DDMapData::s()->isMinMapActive({current.x-1, current.y})) {
        calcVisionMap_ast({-DDConfig::MINMAP_EXPAND, 0}, DOOR_VISION_DISTANCE);
    }

    // 计算里面所有的我军agent导致的视野
    auto& minmap = DDMapData::s()->getBattleObservedMinMap();
    for (auto pair : minmap->posAgentMap) {
        if (pair.second->isFriend()) {
            calcVisionMap_ast(pair.first, pair.second->actionDistance);
        }
    }

    // debug print
    _shadowCover->coverAll();
    for (int y = DDConfig::MINMAP_EXPAND; y >= -DDConfig::MINMAP_EXPAND; y--) {
        std::stringstream ss;
        for (int x = -DDConfig::MINMAP_EXPAND; x <= DDConfig::MINMAP_EXPAND; x++) {
            AgentPos pos = {x, y};
            if (_visionMap[pos])
                _shadowCover->reveal(pos);
            ss << (_visionMap[{x, y}] ? "V": "_");
        }
        CCLOG("%s", ss.str().c_str());
    }


}


void BattleField::calcVisionMap_ast(const AgentPos& agentpos, int visionDistance)
{
    for (int x = agentpos.x - visionDistance; x <= agentpos.x + visionDistance; x++) {
        for (int y = agentpos.y - visionDistance; y <= agentpos.y + visionDistance; y++) {
            AgentPos pos = {x,y};
            if (pos.isLegal() && pos.distance(agentpos) <= visionDistance) {
                _visionMap[pos] = true;
            }
        }
    }

}

bool BattleField::help_isDoorPosition(const AgentPos& agentpos)
{
    return (agentpos.x == 0 && std::abs(agentpos.y) == DDConfig::MINMAP_EXPAND) || (agentpos.y == 0 && std::abs(agentpos.x) == DDConfig::MINMAP_EXPAND);
}


void BattleField::calcRedGreenCoverAndShow()
{
    auto& minmap = DDMapData::s()->getBattleObservedMinMap();
    for (auto pair : _visionMap) {
        if (pair.second && minmap->posAgentMap.count(pair.first) == 0 && !help_isDoorPosition(pair.first)) {
            // 视野内，没有agent占位，不再门口
            _redGreenMap[pair.first] = RedGreenCover::CS_GREEN;
        } else if (pair.second) {
            _redGreenMap[pair.first] = RedGreenCover::CS_RED;
        } else {
            _redGreenMap[pair.first] = RedGreenCover::CS_NONE;
        }
    }
    for (auto pair:_redGreenMap) {
        _redGreenCover->configCover(pair.first, pair.second);
    }
    _redGreenCover->show();
}