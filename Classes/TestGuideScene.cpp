// (C) 2015 Turnro.com

#include "TestGuideScene.h"
#include "TurnScene.h"
#include "WelcoSceneSec.h"
#include "GlassDemoScene.h"
#include "PostureEditScene.h"
#include "MenuScene.h"
USING_NS_CC;

bool TestGuideScene::init()
{
    assert(TRBaseScene::init());


    addCommonBtn({0.5,0.8}, "glass test", [](){
           Director::getInstance()->pushScene(GlassDemoScene::create());
    });
    addCommonBtn({0.5,0.7}, "posture edit", [](){
           Director::getInstance()->pushScene(PostureEditScene::create());
    });

    addCommonBtn({0.5,0.3}, "Welco", [](){
        Director::getInstance()->pushScene(WelcoSceneSec::create());
    });

    return true;
}