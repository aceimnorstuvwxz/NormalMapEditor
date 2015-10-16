// (C) 2015 Turnro.com

#include "MenuScene.h"
#include "MenuButton.h"
#include "MainMenuFrame.h"
#include "uiconf.h"
#include "StoryMenuFrame.h"
#include "SettingsMenuFrame.h"
#include "HelpMenuFrame.h"
#include "format.h"
#include "SimpleAudioEngine.h"
#include "WelcoSceneSec.h"
#include "RolkConfig.h"
#include "CreditMenuFrame.h"
#include "EndlessMenuFrame.h"

USING_NS_CC;

MenuScene* MenuScene::create(int frameId)
{
    auto p = new MenuScene();
    if (p && p->init(frameId)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool MenuScene::init(int frameId)
{
    assert(TRBaseScene::init());

    _layer = Layer::create();
    this->addChild(_layer);

    initCameraNode();
    initMenuFrames();

#ifdef __GAME_DEV__
    addCommonBtn({0.00,0.15}, "reset", [](){Director::getInstance()->replaceScene(MenuScene::create());});
    addCommonBtn({0.00,0.1}, "back", [](){Director::getInstance()->replaceScene(WelcoSceneSec::create());});
#endif

    initTouchListener();
    _cameraNode->setPosition3D(_menuFrames[frameId]->getPosition3D());
    _firstTimeSpecial = frameId == MENU_MAIN;
    move2frame(frameId);
    animateTail(false, 0.f);

    scheduleUpdate();

    if (TRLocale::s().isMusicEnable())
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(fmt::sprintf("sounds/ambience/menu.mp3", static_cast<int>(rand_0_1()*100)%10).c_str(), true);

    return true;
}

void  MenuScene::initCameraNode()
{

    auto size = Director::getInstance()->getVisibleSize();

    _cameraNode = Sprite3D::create();
    _cameraNode->setPosition3D({0,0,0});
    _layer->addChild(_cameraNode);

    _camera = Camera::createPerspective(60, size.width/size.height, 0.01, 5000);
    _camera->setPosition3D({0,0,500});
    _camera->lookAt(Vec3{0,0,0},Vec3{0,1,0});
    _camera->setCameraFlag(CameraFlag::USER1);
    _cameraNode->addChild(_camera);
    _camera->setCameraMask((unsigned short)CameraFlag::USER1);

    // 必须，否则Camera与子节点的遍历错误，会导致子节点中的Label的跟随父亲相对移动出现错误。
    // 而Sprite3D，可能不是必须的，但现在保留它作为容器。
    _cameraNode->setCameraMask(_camera->getCameraMask());

    const float HEAD_POS_Y = 133;

    _lbCameraHeadText = Label::createWithTTF(TRLocale::s()["head_main_menu"], TRLocale::s().font(), FONT_SIZE);
    _lbCameraHeadText->setScale(0.1);
    _lbCameraHeadText->setPosition3D({0, HEAD_POS_Y, 250});
    _lbCameraHeadText->setCameraMask(_camera->getCameraMask());
    _lbCameraHeadText->setTextColor(Color4B::BLACK);
    _cameraNode->addChild(_lbCameraHeadText);

    _drawNodeHeadBg = TRDrawNode3D::create();
    _drawNodeHeadBg->setCameraMask(_camera->getCameraMask());
    _cameraNode->addChild(_drawNodeHeadBg);
    _drawNodeHeadBg->setPosition3D({0,0,245});
    const float HEAD_BG_POS_Y = HEAD_POS_Y +5;
    const float HEAD_BG_HEIGHT_HALF = 10;
    Vec3 vlt = {-1000, HEAD_BG_POS_Y+HEAD_BG_HEIGHT_HALF, 0};
    Vec3 vrt = {1000, HEAD_BG_POS_Y+HEAD_BG_HEIGHT_HALF, 0};
    Vec3 vlb = {-1000, HEAD_BG_POS_Y-HEAD_BG_HEIGHT_HALF, 0};
    Vec3 vrb = {1000, HEAD_BG_POS_Y-HEAD_BG_HEIGHT_HALF, 0};

    _drawNodeHeadBg->setTriangleColor(Color4F::WHITE);
    _drawNodeHeadBg->drawTriangle(vrt, vlt, vlb);
    _drawNodeHeadBg->drawTriangle(vrt, vlb, vrb);

    _drawNodeHeadBg2 = TRDrawNode3D::create();
    _drawNodeHeadBg2->setCameraMask(_camera->getCameraMask());
    _cameraNode->addChild(_drawNodeHeadBg2);
    _drawNodeHeadBg2->setPosition3D({0,0,240});
    const float HEAD_BG2_HEIGHT = 3.f;
    Vec3 vlt2 = {-1000, HEAD_BG_POS_Y+HEAD_BG_HEIGHT_HALF, 0};
    Vec3 vrt2 = {1000, HEAD_BG_POS_Y+HEAD_BG_HEIGHT_HALF, 0};
    Vec3 vlb2 = {-1000, HEAD_BG_POS_Y-HEAD_BG_HEIGHT_HALF-HEAD_BG2_HEIGHT, 0};
    Vec3 vrb2 = {1000, HEAD_BG_POS_Y-HEAD_BG_HEIGHT_HALF-HEAD_BG2_HEIGHT, 0};

    _drawNodeHeadBg2->setTriangleColor({0.85,0.85,0.85,1});
    _drawNodeHeadBg2->drawTriangle(vrt, vlt, vlb);
    _drawNodeHeadBg2->drawTriangle(vrt, vlb, vrb);

    // 尾部 返回按钮
    const float TAIL_POS_Y = -138;

    _lbCameraTailText = Label::createWithTTF(TRLocale::s()["back"], TRLocale::s().font(), FONT_SIZE);
    _lbCameraTailText->setScale(0.1);
    _lbCameraTailText->setPosition3D({0, TAIL_POS_Y, 250});
    _lbCameraTailText->setCameraMask(_camera->getCameraMask());
    _lbCameraTailText->setTextColor(Color4B::WHITE);
    _cameraNode->addChild(_lbCameraTailText);

    const float TAIL_BG_POS_Y = TAIL_POS_Y - 2;
    const float TAIL_BG_HEIGHT_HALF = 10;

    Vec3 tvlt = {-1000, TAIL_BG_POS_Y+TAIL_BG_HEIGHT_HALF, 0};
    Vec3 tvrt = {1000, TAIL_BG_POS_Y+TAIL_BG_HEIGHT_HALF, 0};
    Vec3 tvlb = {-1000, TAIL_BG_POS_Y-TAIL_BG_HEIGHT_HALF, 0};
    Vec3 tvrb = {1000, TAIL_BG_POS_Y-TAIL_BG_HEIGHT_HALF, 0};

    _drawNodeTailBg = TRDrawNode3D::create();
    _drawNodeTailBg->setCameraMask(_camera->getCameraMask());
    _cameraNode->addChild(_drawNodeTailBg);
    _drawNodeTailBg->setPosition3D({0,0,245});

    _drawNodeTailBg->setTriangleColor(Color4F::BLACK);
    _drawNodeTailBg->drawLine(tvrt, tvlt);
    _drawNodeTailBg->drawTriangle(tvrt, tvlt, tvlb);
    _drawNodeTailBg->drawTriangle(tvrt, tvlb, tvrb);
    _backButtonTopRelativePos = tvrt;
}

void MenuScene::resetText()
{
    TTFConfig cfg(TRLocale::s().font().c_str(), FONT_SIZE, GlyphCollection::DYNAMIC);

    _lbCameraHeadText->setTTFConfig(cfg);
    _lbCameraTailText->setTTFConfig(cfg);
    auto key = _menuFrames[_selectedFrameId]->getTitle();
    _lbCameraHeadText->setString(key.length() > 0 ? TRLocale::s()[key] : "");
    _lbCameraTailText->setString(TRLocale::s()["back"]);
}

void MenuScene::setBackPressed(bool ispressed)
{
    _drawNodeTailBg->setTriangleColor(ispressed ? Color4F::WHITE : Color4F::BLACK);
    _lbCameraTailText->setTextColor(ispressed ? Color4B::BLACK : Color4B::WHITE);
}

void MenuScene::animateTail(bool isshow, float aniTime, bool totalOut)
{
    const float MOVE_DIFF_Y = totalOut ? 30.f:15.f;
    if (_isTailShowing) {
        // 先移动出
        if (aniTime > 0) {
            auto ac = MoveBy::create(aniTime*0.333, {0, -MOVE_DIFF_Y, 0});
            _drawNodeTailBg->runAction(ac->clone());
            _lbCameraTailText->runAction(ac);
        } else {
            _drawNodeTailBg->setPositionY(_drawNodeTailBg->getPositionY()-MOVE_DIFF_Y);
            _lbCameraTailText->setPositionY(_lbCameraTailText->getPositionY()-MOVE_DIFF_Y);
        }
    }

    if (isshow) {
        // 再移动入
        scheduleOnce([this, MOVE_DIFF_Y, aniTime](float dt){
            auto ac = MoveBy::create(aniTime/2, {0, MOVE_DIFF_Y, 0});
            _drawNodeTailBg->runAction(ac->clone());
            _lbCameraTailText->runAction(ac);
        }, aniTime*0.666, fmt::sprintf("%d", rand()));
    }

    _isTailShowing = isshow;
}

void MenuScene::animateHead(bool isshow, float aniTime, bool totalOut)
{
    const float MOVE_DIFF_Y = totalOut ? 30.f:15.f;
    if (_isHeadShowing) {
        // 先移动出
        auto ac = MoveBy::create(aniTime*0.333, {0, MOVE_DIFF_Y, 0});
        _drawNodeHeadBg->runAction(ac->clone());
        _drawNodeHeadBg2->runAction(ac->clone());
        _lbCameraHeadText->runAction(ac);
    }

    if (isshow) {
        scheduleOnce([this, MOVE_DIFF_Y, aniTime](float dt){
            auto ac = MoveBy::create(aniTime*0.333, {0, -MOVE_DIFF_Y, 0});
            _drawNodeHeadBg->runAction(ac->clone());
            _drawNodeHeadBg2->runAction(ac->clone());
            _lbCameraHeadText->runAction(ac);
        }, aniTime*0.666, fmt::sprintf("%d", rand()));
    }
    _isHeadShowing = isshow;
}

void MenuScene::initMenuFrames()
{
    auto _mainFrame = MainMenuFrame::create(this);
    _layer->addChild(_mainFrame);
    _mainFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_MAIN] = _mainFrame;

    auto _levelFrame = StoryMenuFrame::create(this);
    _layer->addChild(_levelFrame);
    _levelFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_STORIES] = _levelFrame;

    auto _settingsFrame = SettingsMenuFrame::create(this);
    _layer->addChild(_settingsFrame);
    _settingsFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_SETTINGS] = _settingsFrame;

    auto _helpFrame = HelpMenuFrame::create(this);
    _layer->addChild(_helpFrame);
    _helpFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_HELP] = _helpFrame;

    auto _loadingFrame = LoadingMenuFrame::create(this);
    _layer->addChild(_loadingFrame);
    _loadingFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_LOADING] = _loadingFrame;
    _loadingMenuFrame = _loadingFrame;

    auto _creditFrame = CreditMenuFrame::create(this);
    _layer->addChild(_creditFrame);
    _creditFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_CREDIT] = _creditFrame;

    auto _endlessFrame = EndlessMenuFrame::create(this);
    _layer->addChild(_endlessFrame);
    _endlessFrame->setCameraMask(_camera->getCameraMask(), true);
    _menuFrames[MENU_ENDLESS] = _endlessFrame;
    
    for (auto iter = _menuFrames.begin(); iter != _menuFrames.end(); iter++) {
        iter->second->setVisible(false);
    }
}

void MenuScene::move2frame(int frameid)
{
    _isMoving = true;
    float moveTime = FRAME_MOVE_TIME_FACTOR * (_menuFrames[frameid]->getPosition3D() - _cameraNode->getPosition3D()).length();
    _menuFrames[frameid]->setVisible(true);
    _menuFrames[frameid]->onTurnIn();
    _cameraNode->runAction(MoveTo::create(moveTime, _menuFrames[frameid]->getPosition3D()));
    _isAnyFrameSelected = true;
    int oldFrameId = _selectedFrameId;
    _selectedFrameId = frameid;
    animateTail(frameid != MENU_MAIN && frameid != MENU_LOADING, moveTime, frameid == MENU_LOADING);
    if (_firstTimeSpecial) {
        _firstTimeSpecial = false;
        _drawNodeHeadBg->setOpacity(0);
        _drawNodeHeadBg2->setOpacity(0);
        _lbCameraHeadText->setOpacity(0);
        auto fi = FadeIn::create(0.5);
        _drawNodeHeadBg2->runAction(fi->clone());
        _drawNodeHeadBg->runAction(fi->clone());
        _lbCameraHeadText->runAction(fi);
    } else {
        animateHead(frameid != MENU_LOADING, moveTime, frameid == MENU_LOADING);
        scheduleOnce([this, frameid](float dt){_lbCameraHeadText->setString(TRLocale::s()[_menuFrames[frameid]->getTitle()]);}, moveTime/2, fmt::sprintf("%d", rand()));
    }
    scheduleOnce([this, oldFrameId](float dt){
        if (oldFrameId != MENU_MAX) {
            _menuFrames[oldFrameId]->setVisible(false);
            _menuFrames[oldFrameId]->onTurnOut();
        }
        _isMoving = false;
    }, moveTime, fmt::sprintf("%d", rand()));
}

bool MenuScene::isCursorInBack(const cocos2d::Vec2& cursor)
{
    auto cc = cursor;
    cc.y = Director::getInstance()->getWinSize().height - cc.y;

    auto transform = _drawNodeTailBg->getNodeToWorldTransform();
    Vec3 top;
    transform.transformPoint(_backButtonTopRelativePos, &top);

    auto stop = _camera->project(top);
    return cc.y > stop.y;
}

void MenuScene::initTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event){
        if (_isMoving)  return true;
        setBackPressed(isCursorInBack(touch->getLocation()));
        if (_isAnyFrameSelected)
            _menuFrames[_selectedFrameId]->onPressed(touch->getLocation());
        return true;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event){
        if (_isMoving)  return;
        setBackPressed(isCursorInBack(touch->getLocation()));
        if (_isAnyFrameSelected)
            _menuFrames[_selectedFrameId]->onMoved(touch->getLocation());
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event){
        if (_isMoving)  return;
        setBackPressed(false);
        if (_isTailShowing && isCursorInBack(touch->getLocation())) {
            moveback();
            if (TRLocale::s().isSfxEnable())
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/menu_btn_ast.mp3");
        } else if (_isAnyFrameSelected)
            _menuFrames[_selectedFrameId]->onReleased(touch->getLocation());
    };

    listener->onTouchCancelled = [this](Touch* touch, Event* event){
        if (_isMoving)  return;
        setBackPressed(false);
        if (_isAnyFrameSelected)
            _menuFrames[_selectedFrameId]->onCanceled(touch->getLocation());
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

cocos2d::Camera* MenuScene::getMenuCamera()
{
    return _camera;
}

void MenuScene::update(float dt)
{
    _glassFactor += _glassFactorAdding ? 0.01 * dt : -0.01 * dt;
    if (_glassFactor > 1) {
        _glassFactorAdding = false;
    }
    if (_glassFactor < 0) {
        _glassFactorAdding = true;
    }
//    _glass->setFactor(_glassFactor);
}

void MenuScene::moveback()
{
    int tid = _selectedFrameId == MENU_CREDIT ? MENU_HELP : MENU_MAIN;
    move2frame(tid);
}
