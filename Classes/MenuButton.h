// (C) 2015 Turnro.com

#ifndef __Turnroll__MenuButton__
#define __Turnroll__MenuButton__

#include "cocos2d.h"
#include "TRDrawNode3D.h"
#include "MenuFrameProtocal.h"

// 对于点击的计算，有两个方案：
// 1) 在3D下进行intersetion计算。
// 2) 把Button坐标project到屏幕后在屏幕2D上计算。（选这个）


class MenuButton: public TRDrawNode3D
{
public:
    typedef std::function<void(void)> MENU_BTN_CALLBACK;
    static MenuButton* create(const std::string& text, float width, float height, MENU_BTN_CALLBACK callback, MenuFrameProtocal* controller, bool gray = false);
    virtual bool init(const std::string& text, float width, float height, MENU_BTN_CALLBACK callback, MenuFrameProtocal* controller, bool gray);
    void setText(const std::string& text);
    void setComment(const std::string& left, const std::string& right);
    void resetText();

    virtual void onPressed(const cocos2d::Vec2& cursor);
    virtual void onMoved(const cocos2d::Vec2& cursor);
    virtual void onCanceled(const cocos2d::Vec2& cursor);
    virtual void onReleased(const cocos2d::Vec2& cursor);
    virtual void onTurnIn();
    virtual void onTurnOut();
    bool isCursorIn(const cocos2d::Vec2& cursor);
    virtual void setOpacity(GLubyte opacity) override;

protected:
    MenuButton();
    virtual ~MenuButton();

    const float FONT_SIZE = 36;
    const float COMMENT_FONT_SIZE = 18;

    bool _gray;

    float _width;
    float _height;
    // MenuButton的实际显示的字符串都是要从TRLocale中查询的，
    // resetText()会使重新查询，而下面这几个text只是key。
    std::string _text;
    std::string _commentLeft;
    std::string _commentRight;
    cocos2d::Label* _label;
    cocos2d::Label* _labelCommentLeft;
    cocos2d::Label* _labelCommentRight;
    MENU_BTN_CALLBACK _callback;
    MenuFrameProtocal* _controller;
    TRDrawNode3D* _drawNodeBullet;
    TRDrawNode3D* _drawNodeBullet2;

    bool _loaded = false;
    void tideLoad();
    void tideCancel();
    void tideShot();
};

#endif /* defined(__Turnroll__MenuButton__) */
