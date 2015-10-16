// (C) 2015 Turnro.com

#include "Bigmap.h"
#include "DDMapData.h"
#include "format.h"

USING_NS_CC;

void Bigmap::init(cocos2d::Layer *bigmapLayer, cocos2d::Layer* uiLayer)
{
    _bigmapLayer = bigmapLayer;
    _uiLayer = uiLayer;

    _bigmapBaseImage = Sprite::create(fmt::sprintf("mapdata/%d.png", DDMapData::s()->getTemplateIndex()));
    _bigmapBaseImage->setScale(DDConfig::bigmapCubePixelScaler());
    _bigmapBaseImage->setZOrder(Z_BASE_IMAGE);
    _bigmapLayer->addChild(_bigmapBaseImage);
//    _bigmapBaseImage->setPositionY(1);

    _agentsNode = BigmapAgentsNode::create();
    _agentsNode->setScale(DDConfig::bigmapCubePixelScaler());
    _agentsNode->setZOrder(Z_BIGMAP_NODE);
    _bigmapLayer->addChild(_agentsNode);

    _coverNode = BigmapCoverNode::create();
    _coverNode->setScale(DDConfig::bigmapCubePixelScaler());
    _coverNode->setZOrder(Z_BIGMAP_COVER);
    _bigmapLayer->addChild(_coverNode);

    _centerMinMapPos = {0,0};

    initMessageThings();
    initTouchThings();
}

void Bigmap::configProtocals(DDBattleFieldProtocal *battleProtocal)
{
    _battleProtocal = battleProtocal;
}

void Bigmap::step(float dt)
{
    _agentsNode->configAsDirty();//todo
    _coverNode->configAsDirty();
}

void Bigmap::initTouchThings()
{
    static MapPos move_start_mappos;
    static bool touch_moved;
    _selectionIcon = Sprite::create("images/bigmap_select.png");

    _selectionIcon->setScale(DDConfig::bigmapCubeWidth()/_selectionIcon->getContentSize().width);
    _selectionIcon->setVisible(false);
    _bigmapLayer->addChild(_selectionIcon);
    _selectionIcon->setZOrder(Z_SELECTION_IMAGE);

    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        auto point = touch->getLocation();
        move_start_mappos = _centerMinMapPos;
        touch_moved = false;
        auto rect = DDConfig::bigmapAreaRect();
        return rect.containsPoint(point);
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        touch_moved = true;
        if (DDConfig::bigmapAreaRect().containsPoint(touch->getLocation())) {
            auto delta = touch->getLocation() - touch->getStartLocation();
            int diffx = delta.x/DDConfig::bigmapCubeWidth();
            int diffy = delta.y/DDConfig::bigmapCubeWidth();
            help_bigmapMoveTo({move_start_mappos.x-diffx,move_start_mappos.y+diffy});
        }
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        if (DDConfig::bigmapAreaRect().containsPoint(touch->getLocation()) && touch_moved == false) {
            //点击选择新的Minmap
            auto point = touch->getLocation();
            auto delta = point - DDConfig::bigmapAreaCenter();
            MapPos clickPos = _centerMinMapPos;
            clickPos.x += (std::abs(delta.x)+DDConfig::bigmapCubeWidth()*0.5f)/DDConfig::bigmapCubeWidth() * (delta.x > 0 ? 1:-1);
            clickPos.y += (std::abs(delta.y)+DDConfig::bigmapCubeWidth()*0.5f)/DDConfig::bigmapCubeWidth() * (delta.y > 0 ? 1:-1);
            help_tryToSwitchTo(clickPos);
        }
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
    };

    _bigmapLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _bigmapLayer);
}

void Bigmap::help_bigmapMoveTo(const MapPos& mappos)
{
    _centerMinMapPos = mappos;
    _bigmapLayer->setPosition(DDConfig::bigmapAreaCenter() + DDConfig::bigmapCubeWidth()*Vec2{static_cast<float>(-mappos.x), static_cast<float>(mappos.y)});
}

void Bigmap::help_tryToSwitchTo(const MapPos& mappos)
{
    if (_selectedMapPos == mappos) { return;}
    // 检查是否可以选择
    if (DDMapData::s()->getMinMaps().find(mappos) != DDMapData::s()->getMinMaps().end())
    {
        _selectionIcon->setVisible(true);
        _selectionIcon->runAction(MoveTo::create(0.25f, DDConfig::bigmapCubeWidth()*Vec2{static_cast<float>(mappos.x), static_cast<float>(mappos.y)}));

        // battle field和big map要互相照顾好。
        DDMapData::s()->battleFieldObservedMinMap = mappos;
        _battleProtocal->op_switch2minmap();
        _selectedMapPos = mappos;
    }
}