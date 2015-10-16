// (C) 2015 Turnro.com

#include "BattleField.h"
#include "format.h"
#include "DDAgent.h"

USING_NS_CC;

void AgentNode::init(cocos2d::Layer * battleLayer)
{
    _battleLayer = battleLayer;

    auto testLow = BattleLowNode::create();
    testLow->setBuildingBg("images/test_building_bg.png");
    testLow->setFieldBg("images/test_field_bg.png");
    testLow->setPosition({0,0});
    testLow->setZOrder(BattleField::Z_LOW_NODE);
//    _battleLayer->addChild(testLow);
    _lowNode = testLow;
    _lowNode->retain();

    auto testMiddle = BattleMiddleNode::create();
    testMiddle->configShadowFile("images/test_middle_shadow.png");
    testMiddle->setPosition({0,0});
    testMiddle->setZOrder(BattleField::Z_MIDDLE_NODE);
    _battleLayer->addChild(testMiddle);
    testMiddle->retain();
    _middleNode = testMiddle;
//    testMiddle->configSelection(1);
//    testMiddle->configScopeLine({1,1,1,1});

    auto testHigh = BattleHighNode::create();
    testHigh->configBuildingAnimation("images/test_high_building_animation.png");
    testHigh->configBuildingNormalMap("images/test_high_building_normalmap.png");
    testHigh->setZOrder(BattleField::Z_HIGH_NODE);
    _battleLayer->addChild(testHigh);
    _highNode = testHigh;
    _highNode->retain();
}

//void AgentNode::config(const std::string& buildingFile)
//{
//    _highNode->configBuildingAnimation(buildingFile);
//}

void AgentNode::configAgentFloatPos(const Vec2& pos)
{
    _lowNode->setPosition(pos);
    _middleNode->setPosition(pos);
    _highNode->setPosition(pos);
}

void AgentNode::configAgentPos(const AgentPos &pos)
{
    _agentPos = pos;
}

void AgentNode::configAnimation(cocos2d::Texture2D* texture)
{
//    auto fn_config = fmt::sprintf("%s/config.json", path);
//    auto fn_animation = fmt::sprintf("%s/animation.png", path);
//    auto fn_shadow = fmt::sprintf("%s/shadow.png", path);
    _highNode->configAnimationTexture(texture);
}

void AgentNode::configAnimationIndex(int index)
{
    _highNode->configBuildingAnimationIndex(index);
}

void AgentNode::updateLights(LightNode* lights)
{
    _middleNode->updateLights(lights);
    _highNode->updateLights(lights);
}

void AgentNode::clear()
{
    _lowNode->setVisible(false);
    _middleNode->setVisible(false);
    _highNode->setVisible(false);
}

void AgentNode::build(int aid, int agentType, int agentLevel, bool boss, bool lowBlood)
{
    this->aid = aid;
    this->agentType = agentType;
    this->agentLevel = agentLevel;
    this->boss = boss;
    this->lowBlood = lowBlood;


    std::string typeKey = agentType2agentTypeString(agentType);
    CCLOG("build agent node: %s", typeKey.c_str());
    _middleNode->configShadowFile(fmt::sprintf("graphic/%s/shadow.png", typeKey));
    _middleNode->setVisible(true);

    _highNode->configBuildingAnimation(fmt::sprintf("graphic/%s/animation.png", typeKey));
    _highNode->configBuildingNormalMap(fmt::sprintf("graphic/%s/normalmap.png", typeKey));
    _highNode->setVisible(true);
}