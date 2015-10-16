// (C) 2015 Turnro.com

#include "PostureEditScene.h"
#include "PolyPlaneSprite.h"
#include "EditState.h"
#include "SimpleAudioEngine.h"

#include "HandShapes.h"
#include "LocalSpaceRotation.h"
#include "format.h"
USING_NS_CC;


//#define AIR

PixelPlane* PostureEditScene::genAirObj(){

    auto node = PixelPlane::create("3d/pp_hand.fsh", EditState::s()->getBgmTextureFile());
    node->setScale(0.2);
    node->setCameraMask(_camera->getCameraMask());
    _layer->addChild(node);
    int INDEX_LEN = sizeof(hand_left_air_index)/sizeof(int);
    for (int j = 0; j < INDEX_LEN-3; j += 6) {
        node->drawTriangle(fetchLeftAirHandPoint(j), fetchLeftAirHandPoint(j+1), fetchLeftAirHandPoint(j+2), {0,0},{1,0},{0,1});
        node->drawTriangle(fetchLeftAirHandPoint(j+3), fetchLeftAirHandPoint(j+4), fetchLeftAirHandPoint(j+5), {0,0},{1,1},{1,0});
    }
    return  node;
}

PixelPlane* PostureEditScene::genRoadObj(){

    auto node = PixelPlane::create("3d/pp_hand.fsh", EditState::s()->getBgmTextureFile());
    node->setScale(0.2);
    node->setCameraMask(_camera->getCameraMask());
    _layer->addChild(node);
    int INDEX_LEN = sizeof(hand_left_road_index)/sizeof(int);
    for (int j = 0; j < INDEX_LEN-3; j += 6) {
        node->drawTriangle(fetchLeftRoadHandPoint(j), fetchLeftRoadHandPoint(j+1), fetchLeftRoadHandPoint(j+2), {0,0},{1,0},{0,1});
        node->drawTriangle(fetchLeftRoadHandPoint(j+3), fetchLeftRoadHandPoint(j+4), fetchLeftRoadHandPoint(j+5), {0,0},{1,1},{1,0});
    }
    return  node;
}

bool PostureEditScene::init()
{
    assert(TRBaseScene::init());
    _layer = Layer::create();

    this->addChild(_layer);

    auto size = Director::getInstance()->getVisibleSize();

    auto camera = Camera::createPerspective(60, size.width/size.height, 0.01, 1000);
    camera->setPosition3D({0,0,0});
    camera->lookAt(Vec3{0,0,-20},Vec3{0,1,0});
    camera->setCameraFlag(CameraFlag::USER1);
    _layer->addChild(camera);
    camera->setCameraMask((unsigned short)CameraFlag::USER1);
    _camera = camera;


    this->addCommonBtn({0.1f,0.95f}, "back", [](){
        Director::getInstance()->popScene();
    });

    _lb = this->addCommonLabel({0.75,0.35}, "message");


    for (int i = 0; i < N; i++) {
#ifdef AIR
        _postureObjs[i] = genAirObj();
#else
        _postureObjs[i] = genRoadObj();
#endif
        _postureObjs[i]->setPosition3D({0,-1.f*i,-10});
    }
#ifdef AIR
    _aniObj = genAirObj();
#else
    _aniObj = genRoadObj();
#endif
    _aniObj->setPosition3D({0,0,0});
    _aniObj->setVisible(false);


//
//    Quaternion from = {0.203,-0.172,-0.017,0.964};
//    Quaternion to = {-0.172,0.0,0.0,0.985};
////    _leftRoadHand->runAction(RepeatForever::create(Sequence::create(MoveFromTo::create(2.0, Vec3{0,5,5}, Vec3{-0,0,-5}), MoveFromTo::create(2.0, Vec3{0,0,-5}, Vec3{0,-5,-4}) ,NULL)));

    this->addCommonBtn({0.2f,0.8f}, "<<<", [this](){
        _selectIndex = _selectIndex > 0 ? _selectIndex - 1 : _selectIndex;
    });
    this->addCommonBtn({0.8f,0.8f}, ">>>", [this](){
        _selectIndex = _selectIndex < N-1 ? _selectIndex + 1 : _selectIndex;
    });
    this->addCommonBtn({0.8f,0.9f}, ">>>2", [this](){
        int old = _selectIndex;
        _selectIndex = _selectIndex < N-1 ? _selectIndex + 1 : _selectIndex;
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[old]->getPosition3D());

    });
    this->addCommonBtn({0.5f,0.9f}, "ani", [this](){
        if (_aning) {
            _aning = false;
            _aniObj->setVisible(false);
            _aniObj->stopAllActions();
            for (int i = 0; i < N; i++) {
                _postureObjs[i]->setVisible(true);
            }
        } else {
            _aning = true;
            _aniObj->setVisible(true);
            Vector<cocos2d::FiniteTimeAction *> acs;
            for (int i = 0; i < N-1; i++) {
                auto ac = MoveFromTo::create(ANI_TIME/N, _postureObjs[i]->getPosition3D(), _postureObjs[i+1]->getPosition3D());
                acs.pushBack(ac);
            }
            auto ac = Sequence::create(acs);
            _aniObj->runAction(RepeatForever::create(ac));
            for (int i = 0; i < N; i++) {
                _postureObjs[i]->setVisible(false);
                auto p = _postureObjs[i]->getPosition3D();
                CCLOG("%f %f %f", p.x, p.y, p.z);
            }
        }

    });

    this->addCommonBtn({0.8f,0.675f}, "x+", [this](){
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[_selectIndex]->getPosition3D() + Vec3{MOVE_STEP, 0, 0});
    });
    this->addCommonBtn({0.2f,0.675f}, "x-", [this](){
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[_selectIndex]->getPosition3D() + Vec3{-MOVE_STEP, 0, 0});

    });
    this->addCommonBtn({0.5f,0.7f}, "y+", [this](){
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[_selectIndex]->getPosition3D() + Vec3{0, MOVE_STEP, 0});

    });
    this->addCommonBtn({0.5f,0.65f}, "y-", [this](){
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[_selectIndex]->getPosition3D() + Vec3{0, -MOVE_STEP, 0});

    });
    this->addCommonBtn({0.5f,0.8f}, "z+", [this](){
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[_selectIndex]->getPosition3D() + Vec3{0, 0, MOVE_STEP});

    });
    this->addCommonBtn({0.5f,0.75f}, "z-", [this](){
        _postureObjs[_selectIndex]->setPosition3D(_postureObjs[_selectIndex]->getPosition3D() + Vec3{0, 0, -MOVE_STEP});
    });


    scheduleUpdate();
    return  true;
}


void PostureEditScene::update(float dt)
{
    _shaderTime += dt;
    for (int i = 0; i < N; i++) {
        _postureObjs[i]->setOpacity(128);
    }
    _postureObjs[_selectIndex]->setOpacity(255);
    std::string message;
    if (this->_aning) {
        for (int i = 0; i < N; i++) {
            auto p = _postureObjs[i]->getPosition3D();
            message += fmt::sprintf("%f %f %f\n", p.x, p.y, p.z);
        }
    } else {
        auto p = _postureObjs[_selectIndex]->getPosition3D();
        message = fmt::sprintf("N=%d\n (%f,%f,%f)", _selectIndex, p.x, p.y, p.z);
    }
    _lb->setString(message);
}