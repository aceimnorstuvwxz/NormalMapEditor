// (C) 2015 Turnro.com

#include "StoryMenuFrame.h"
#include "format.h"
#include "TRStories.h"
#include "TRLocale.h"
#include "EditState.h"
USING_NS_CC;

StoryMenuFrame* StoryMenuFrame::create(MenuFrameProtocal *controller)
{
    auto p = new StoryMenuFrame();
    if (p && p->init(controller)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool StoryMenuFrame::init(MenuFrameProtocal* controller)
{
    assert(MenuFrame::init(controller));

    setPosition3D({0, -650, 0});

    float y_start = 210;
    float width = 250;
    float height = 40;
    float y_now = y_start;
    float y_step = 55;

    auto& chapters = TRStories::s().getChapters();
    bool isLastPassed = true;
    for (auto iter = chapters.begin(); iter != chapters.end(); iter++) {
        auto lb = Label::createWithTTF(TRLocale::s()[iter->_name], TRLocale::s().font(), CH_FONT_SIZE);
        _keyOfLb.push_back({lb, iter->_name});
        lb->setScale(0.5);
        lb->setPosition3D({0, y_now, 0});
        if (!isLastPassed) {
            lb->setTextColor(Color4B::GRAY);
        }
        this->addChild(lb);
        y_now -= y_step;

        for (auto jter = iter->_sections.begin(); jter != iter->_sections.end(); jter++) {
            int id = jter->_id;
            auto btn = MenuButton::create(isLastPassed ? jter->_name : "locked", width, height, [this, id](){
                EditState::s()->config(id, false);
                _controller->move2frame(MENU_LOADING);
            }, _controller, isLastPassed == false);
            auto sectionStatic = TRLocale::s().getSectionStatic(id, false);
            btn->setComment(sectionStatic.second > 0 ? fmt::sprintf("%d pts", sectionStatic.second) : "", (isLastPassed && !sectionStatic.first) ? "*":"");

            btn->setPosition3D({0, y_now, 0});
            y_now -= y_step;
            this->addChild(btn);
            _buttons.push_back(btn);
            isLastPassed = sectionStatic.first;
        }
    }
    _camDiff = std::abs(y_now - y_start) - 7*y_step;
    _originPos = getPosition3D();
    return true;
}

void StoryMenuFrame::resetText()
{
    for (auto b : _buttons) {
        b->resetText();
    }
    TTFConfig cfg(TRLocale::s().font().c_str(), CH_FONT_SIZE, GlyphCollection::DYNAMIC);
    for (auto& pa : _keyOfLb) {
        pa.first->setTTFConfig(cfg);
        pa.first->setString(TRLocale::s()[pa.second]);
    }
}

void StoryMenuFrame::onPressed(const cocos2d::Vec2& cursor)
{
    _moved = false;
    _lastCursor = cursor;
    for (auto btn: _buttons) {
        btn->onPressed(cursor);
    }
}
void StoryMenuFrame::onMoved(const cocos2d::Vec2& cursor)
{
    _moved = true;
    for (auto btn: _buttons) {
        btn->onMoved(cursor);
    }

    Vec2 diff = cursor - _lastCursor;
    _diffNow += diff.y * 0.4f;
    if (_diffNow < 0) {
        _diffNow = 0;
    } else if (_diffNow > _camDiff) {
        _diffNow = _camDiff;
    }
    this->setPosition3D(_originPos + Vec3{0, _diffNow, 0});
    _lastCursor = cursor;
}
void StoryMenuFrame::onCanceled(const cocos2d::Vec2& cursor)
{
    for (auto btn: _buttons) {
        btn->onCanceled(cursor);
    }
}
void StoryMenuFrame::onReleased(const cocos2d::Vec2& cursor)
{
    for (auto btn : _buttons) {
        if (_moved) btn->onCanceled(cursor);
        else btn->onReleased(cursor);
    }
}

std::string StoryMenuFrame::getTitle()
{
    return "head_story_mode";
}

void StoryMenuFrame::onTurnOut()
{
    this->runAction(MoveTo::create(0.33, _originPos));
}