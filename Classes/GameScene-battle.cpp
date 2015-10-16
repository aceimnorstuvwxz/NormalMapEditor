// (C) 2015 Turnro.com

#include "GameScene.h"
#include "DDConfig.h"
#include "TRLocale.h"

USING_NS_CC;

// GameScene的Battle Field部分

void GameScene::initBattleField()
{
    // 基础
    _battleLayer = Layer::create();
    _battleLayer->setPosition(DDConfig::battleAreaCenter());
    this->addChild(_battleLayer);
    _battleLayer->setZOrder(LZ_BATTLE);

    // 对话框
    _battleDialogBackgorund = Sprite::create("images/battle_dialog_bg.png");
    auto dialogRect = DDConfig::battleAreaRect();
    _battleDialogBackgorund->setScale(dialogRect.size.width/_battleDialogBackgorund->getContentSize().width, dialogRect.size.height/_battleDialogBackgorund->getContentSize().height);

    _battleDialogBackgorund->setPosition({0,0});
    _battleLayer->addChild(_battleDialogBackgorund);

    _battleDialogMessageLabel = Label::createWithTTF("", TRLocale::s().font(), 30*DDConfig::fontSizeRadio());
    _battleDialogMessageLabel->setPosition({0,dialogRect.size.height*0.2f});
    _battleLayer->addChild(_battleDialogMessageLabel);

    _battleDialogButtonNeutral = ui::Button::create("images/button_normal.png","images/button_selected.png", "images/button_disable.png");
    _battleDialogButtonNeutral->setPosition({0,-dialogRect.size.height*0.2f});
    _battleDialogButtonNeutral->setTitleText("Invade");
    _battleDialogButtonNeutral->setTitleFontSize(30*DDConfig::fontSizeRadio());
    _battleDialogButtonNeutral->setTitleFontName(TRLocale::s().font());
    _battleDialogButtonNeutral->setScale(DDConfig::battleDialogButtonSize().width/_battleDialogButtonNeutral->getContentSize().width, DDConfig::battleDialogButtonSize().height/_battleDialogButtonNeutral->getContentSize().height);
    _battleDialogButtonNeutral->addTouchEventListener(CC_CALLBACK_2(GameScene::battleButtonNeutralCallback, this));
    _battleLayer->addChild(_battleDialogButtonNeutral);
    _battleDialogBackgorund->setVisible(false);
    _battleDialogButtonNeutral->setVisible(false);
    _battleDialogMessageLabel->setVisible(false);

    // TODO negative / positive buttons
    _battleField.init(_battleLayer);
}

void GameScene::battleButtonNeutralCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{

    if (type != cocos2d::ui::Widget::TouchEventType::ENDED) {
        return;
    } else {
        _battleDialogBackgorund->setVisible(false);
        _battleDialogButtonNeutral->setVisible(false);
        _battleDialogMessageLabel->setVisible(false);
        if (_battleDialogNeutralFunc) {
            _battleDialogNeutralFunc();
        }
    }
}

void GameScene::battleDialog(const std::string& message, const std::string& neutralText, std::function<void(void)> func)
{
    _battleDialogBackgorund->setVisible(true);
    _battleDialogButtonNeutral->setVisible(true);
    _battleDialogMessageLabel->setVisible(true);
    _battleDialogNeutralFunc = func;
    _battleDialogMessageLabel->setString(message);
    _battleDialogButtonNeutral->setTitleText(neutralText);
    _battleIsDialogShowing = true;
}


