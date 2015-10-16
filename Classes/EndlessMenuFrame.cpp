// (C) 2015 Turnro.com

#include "EndlessMenuFrame.h"
#include "uiconf.h"
#include "EditState.h"
#include "format.h"

USING_NS_CC;

EndlessMenuFrame* EndlessMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new EndlessMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool EndlessMenuFrame::init(MenuFrameProtocal *controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0, -650, 0});

    float x_diff = 0;
    float width = 250;
    float height = 40;
    float y_now = 210;
    float y_step = 55;

    {
        auto tbtn = MenuButton::create("end_random", width, height, [this](){

            EditState::s()->config(static_cast<int>(rand_0_1()*100) % N_ENDLESS, true);
            _controller->move2frame(MENU_LOADING);
        }, _controller);
        tbtn->setPosition3D({x_diff, y_now, 0});
        this->addChild(tbtn);
        _buttons.push_back(tbtn);
        y_now -= y_step;
        _btnRandom = tbtn;
    }

    auto func = [this, height, width,x_diff,&y_now,y_step](int idx) {
        auto tbtn = MenuButton::create(fmt::sprintf("n_en_%d",idx), width, height, [this, idx](){

            EditState::s()->config(idx, true);
            _controller->move2frame(MENU_LOADING);

        }, _controller);
        tbtn->setPosition3D({x_diff, y_now, 0});
        auto sectionStatic = TRLocale::s().getSectionStatic(idx, true);
        tbtn->setComment(sectionStatic.second > 0 ? fmt::sprintf("%d pts", sectionStatic.second) : "", "");
        this->addChild(tbtn);
        _buttons.push_back(tbtn);
        y_now -= y_step;
        _btnSections[idx] = tbtn;
    };
    for (int i = 0; i < N_ENDLESS; i++) {
        func(i);
    }

    return true;
}

void EndlessMenuFrame::resetText()
{
    for (auto b : _buttons) {
        b->resetText();
    }
}

void EndlessMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onPressed(cursor);
    }
}
void EndlessMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onMoved(cursor);
    }
}
void EndlessMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onCanceled(cursor);
    }
}
void EndlessMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
    for (auto btn : _buttons) {
        btn->onReleased(cursor);
    }
}

std::string EndlessMenuFrame::getTitle()
{
    return "infinite_mode";
}