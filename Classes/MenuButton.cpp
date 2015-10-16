// (C) 2015 Turnro.com

#include "MenuButton.h"
#include "uiconf.h"
#include "format.h"
#include "TRLocale.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

MenuButton* MenuButton::create(const std::string& text, float width, float height, MENU_BTN_CALLBACK callback, MenuFrameProtocal* controller, bool gray)
{
    auto p = new MenuButton();
    if (p && p->init(text, width, height, callback, controller, gray)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool MenuButton::init(const std::string& text, float width, float height, MENU_BTN_CALLBACK callback, MenuFrameProtocal* controller, bool gray)
{
    assert(TRDrawNode3D::init());
    assert(callback);

    _gray = gray;
    _height = height;
    _width = width;
    _callback = callback;
    _text = text;
    _controller = controller;

    Vec3 lt = {-width/2, height/2, 0};
    Vec3 lb = {-width/2, -height/2, 0};
    Vec3 rt = {width/2, height/2, 0};
    Vec3 rb = {width/2, -height/2, 0};

    if (gray) {
        this->setLineColor(Color4F::GRAY);
    }

    drawLine(lt, lb);
    drawLine(lb, rb);
    drawLine(rb, rt);
    drawLine(rt, lt);

    _label = Label::createWithTTF("", TRLocale::s().font(), FONT_SIZE);
    _label->setPosition3D({0,-3,0});
    _label->setScale(0.5);
    if (gray) {
        _label->setTextColor(Color4B::GRAY);
    }
    this->addChild(_label);

    _labelCommentLeft = Label::createWithTTF("", TRLocale::s().font(), COMMENT_FONT_SIZE);
    _labelCommentLeft->setPosition3D({0, height/2 + 5, 0});
    _labelCommentLeft->setScale(0.5);
    this->addChild(_labelCommentLeft);
    _labelCommentLeft->setWidth(width*2); //注意这个系数必须与scale配合。
    _labelCommentLeft->setHorizontalAlignment(TextHAlignment::LEFT);

    _labelCommentRight = Label::createWithTTF("", TRLocale::s().font(), COMMENT_FONT_SIZE);
    _labelCommentRight->setPosition3D({0, height/2 + 5, 0});
    _labelCommentRight->setScale(0.5);
    this->addChild(_labelCommentRight);
    _labelCommentRight->setWidth(width*2);
    _labelCommentRight->setHorizontalAlignment(TextHAlignment::RIGHT);

    resetText();

    _drawNodeBullet = TRDrawNode3D::create();
    _drawNodeBullet->setPosition3D({0,-height/2,0});
    Vec3 tlt = {-width/2, height, 1};
    Vec3 tlb = {-width/2, 0, 1};
    Vec3 trt = {width/2, height, 1};
    Vec3 trb = {width/2, 0, 1};
    _drawNodeBullet->drawTriangle(trt, tlt, tlb);
    _drawNodeBullet->drawTriangle(trt, tlb, trb);
    _drawNodeBullet->setScaleY(0.001);
    this->addChild(_drawNodeBullet);

    _drawNodeBullet2 = TRDrawNode3D::create();
    _drawNodeBullet2->setPosition3D({0,height/2,0});
    Vec3 tlt2 = {-width/2, 0, 1};
    Vec3 tlb2 = {-width/2, -height, 1};
    Vec3 trt2 = {width/2, 0, 1};
    Vec3 trb2 = {width/2, -height, 1};
    _drawNodeBullet2->drawTriangle(trt2, tlt2, tlb2);
    _drawNodeBullet2->drawTriangle(trt2, tlb2, trb2);
    _drawNodeBullet2->setScaleY(0.001);
    this->addChild(_drawNodeBullet2);

    _drawNodeBullet->setVisible(false);
    _drawNodeBullet2->setVisible(false);
    return true;
}

void MenuButton::resetText()
{
    setText(_text);
    setComment(_commentLeft, _commentRight);
}

void MenuButton::setText(const std::string& text)
{
    _text = text;
    TTFConfig cfg(TRLocale::s().font().c_str(), FONT_SIZE, GlyphCollection::DYNAMIC);
    _label->setTTFConfig(cfg);
    _label->setString(text.length() > 0 ? TRLocale::s()[text] : "");
}

void MenuButton::setComment(const std::string& left, const std::string& right)
{
    TTFConfig cfg(TRLocale::s().font().c_str(), COMMENT_FONT_SIZE, GlyphCollection::DYNAMIC);
    _commentLeft = left;
    _commentRight = right;
    _labelCommentLeft->setTTFConfig(cfg);
    _labelCommentRight->setTTFConfig(cfg);
    _labelCommentLeft->setString(left);
    _labelCommentRight->setString(right);
}

bool MenuButton::isCursorIn(const cocos2d::Vec2& cursor)
{
    if (_gray) {
        return  false;// 当灰时，不会触发任何。
    }
    auto cc = cursor;
    cc.y = Director::getInstance()->getWinSize().height - cc.y;

    auto transform = this->getNodeToWorldTransform();
    // 坑， transform * vec3 这样的话，这个乘法的vec3.w = 0，而不是1，导致错误。
    // 应该使用 transformPoint。它会把vec3.w作为1去执行矩阵乘法。
    Vec3 leftBottom, rightTop;
    transform.transformPoint( -Vec3{_width/2, _height/2, 0}, &leftBottom);

    transform.transformPoint( Vec3{_width/2, _height/2, 0}, &rightTop);

    auto A = _controller->getMenuCamera()->project(leftBottom);
    auto B = _controller->getMenuCamera()->project(rightTop);
    auto diffA = A - cc;
    auto diffB = B - cc;
    return (diffA.x * diffB.x  < 0) && (diffA.y * diffB.y < 0);
}

void MenuButton::onPressed(const cocos2d::Vec2& cursor)
{
    if (isCursorIn(cursor)) {
        tideLoad();
    } else {
        tideCancel();
    }
}

void MenuButton::onMoved(const cocos2d::Vec2& cursor)
{
    if (isCursorIn(cursor)) {
        tideLoad();
    } else {
        tideCancel();
    }
}

void MenuButton::onCanceled(const cocos2d::Vec2& cursor)
{
    tideCancel();
}

void MenuButton::onReleased(const cocos2d::Vec2& cursor)
{
    if (isCursorIn(cursor)) {
        tideShot();
    } else {
        tideCancel();
    }
}

void MenuButton::onTurnIn()
{
}

void MenuButton::onTurnOut()
{
}

void MenuButton::tideLoad()
{
    if (_loaded == false) {
        _loaded = true;
        _drawNodeBullet->setScaleY(0.001);
        _drawNodeBullet->setVisible(true);
        _drawNodeBullet->runAction(ScaleTo::create(0.11, 1, 0.25, 1));
    }
}

void MenuButton::tideCancel()
{
    if (_loaded == true) {
        _loaded = false;
        _drawNodeBullet->runAction(Sequence::create(ScaleTo::create(0.11, 1, 0.001, 1), Hide::create(), NULL));
    }
}

void MenuButton::tideShot()
{
    _drawNodeBullet->runAction(ScaleTo::create(0.11, 1, 1, 1));
    scheduleOnce([this](float dt){
        _drawNodeBullet->setScaleY(0.001);
        _drawNodeBullet->setVisible(false);
        _drawNodeBullet2->setScaleY(1);
        _drawNodeBullet2->setVisible(true);
        _drawNodeBullet2->runAction(Sequence::create(ScaleTo::create(0.11, 1, 0.001, 1), Hide::create(), NULL));
    }, 0.11, fmt::sprintf("%d", rand()));
    scheduleOnce([this](float dt){_callback();}, 0.11, fmt::sprintf("%d", rand()));
    _loaded = false;
    if (TRLocale::s().isSfxEnable())
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/menu_btn.mp3");
}

void MenuButton::setOpacity(GLubyte opacity){
    _label->setOpacity(opacity);
    _labelCommentLeft->setOpacity(opacity);
    _labelCommentRight->setOpacity(opacity);
    TRDrawNode3D::setOpacity(opacity);
}

MenuButton::MenuButton():
_width(0)
,_height(0)
,_callback(nullptr)
,_label(nullptr)
,_controller(nullptr)
{}

MenuButton::~MenuButton()
{}