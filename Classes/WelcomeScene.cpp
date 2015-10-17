// (C) 2015 Turnro Game

#include <string>
#include <vector>
#include <iostream>

#include "Msg.h"
#include "format.h"
#include "uiconf.h"
#include "EditState.h"
#include "WelcomeScene.h"
#include "EditorScene.h"

USING_NS_CC;

//#define __TR_DEV__

bool WelcomeScene::init()
{
    assert(TRBaseScene::init());

    // 显示的结构
    auto lbTitle = Label::createWithTTF(Msg::s()["title"], "fonts/fz.ttf", 25);
    lbTitle->setPosition(genPos({0.5,0.5}));
    lbTitle->setTextColor(Color4B::GRAY);
    _defaultLayer->addChild(lbTitle);

    auto lbVersion = Label::createWithTTF(Msg::s()["version"], "fonts/fz.ttf", 10);
    lbVersion->setPosition(genPos({0.5,0.075}));
    lbVersion->setTextColor(Color4B::GRAY);
    _defaultLayer->addChild(lbVersion);

    addCommonLabel({.5f, .45f}, Msg::s()["input_desc"], 10);

    _lbInput = Label::createWithTTF("", "fonts/fz.ttf", 10);
    _lbInput->setPosition(genPos({.5f,.43f}));
    _defaultLayer->addChild(_lbInput);

    auto lbCopy = Label::createWithTTF(Msg::s()["copyright"], "fonts/fz.ttf", 10);
    lbCopy->setPosition(genPos({0.5, 0.05}));
    lbCopy->setTextColor(Color4B::GRAY);
    _defaultLayer->addChild(lbCopy);

    // 键盘的纹理
    // keyboard
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode code, Event* event){
        if (code == EventKeyboard::KeyCode::KEY_SHIFT) {
            _isUpcase = true;
        }
        this->onKeyPressed(code);
    };
    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode code, Event* event){
        if (code == EventKeyboard::KeyCode::KEY_SHIFT) {
            _isUpcase = false;
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

//    EditState::s()->prepareWorkSpace();

#ifdef __TR_DEV__
    auto func = [this](float dt){
        _input = "testbase7";
        this->onInputFinish();
    };
    scheduleOnce(func, 0.5f, genKey());
#endif

    return true;
}

void WelcomeScene::onKeyPressed(const EventKeyboard::KeyCode& code)
{
    if (code == EventKeyboard::KeyCode::KEY_KP_ENTER ||
        code == EventKeyboard::KeyCode::KEY_RETURN ||
        code == EventKeyboard::KeyCode::KEY_ENTER) {
        //enter
        onInputFinish();
    } else if (code == EventKeyboard::KeyCode::KEY_ESCAPE){
        //esc
        _input.clear();
    } else if (code == EventKeyboard::KeyCode::KEY_BACKSPACE ||
               code == EventKeyboard::KeyCode::KEY_DELETE){
        //delete
        _input.pop_back();
    } else {
        //input
        if (code == EventKeyboard::KeyCode::KEY_MINUS) {
            _input += "_";
        }
        if (code >= EventKeyboard::KeyCode::KEY_0 && code <= EventKeyboard::KeyCode::KEY_9) {
            char c = (char)((int)code - (int)EventKeyboard::KeyCode::KEY_0 + 48);
            _input += c;
        }
        if (code >= EventKeyboard::KeyCode::KEY_A && code <= EventKeyboard::KeyCode::KEY_Z) {
            char c = (char)((int)code - (int)EventKeyboard::KeyCode::KEY_A + (_isUpcase ? 65 : 97));
            _input += c;
        }
    }
    _lbInput->setString(_input);
}

void WelcomeScene::onInputFinish()
{
    if (_input.size() <= 0) return;

    EditState::s()->_moduleName = _input;

    _input.clear();
    _lbInput->setString(_input);
    Director::getInstance()->pushScene(EditorScene::create());
}