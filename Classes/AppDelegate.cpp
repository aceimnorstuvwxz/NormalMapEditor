#include "AppDelegate.h"
#include "GameScene.h"
#include "DDMapData.h"
#include "ScatOrcScene.h"
#include "DDUpgradeLoad.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(true);
    
    director->setAnimationInterval(1.0 / 30);

    register_all_packages();

    DDMapData::s()->recover();
//    TRLocale::s().recover();
    DDUpgradeRoadMap::build();

    auto scene = ScatOrcScene::create();

    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

//    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
//    CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();

    //TODO if in game , pause the game

    DDMapData::s()->store();
//    TRLocale::s().store();
//    TRLocale::s().applyVolumeSettings();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    DDMapData::s()->recover();


//    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
//    CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
//    EditState::s()->_needPause = true;
}
