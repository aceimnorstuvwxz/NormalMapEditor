// (C) 2015 Turnro.com

#include "MenuFrame.h"

USING_NS_CC;

bool MenuFrame::init(MenuFrameProtocal* controller)
{
    assert(Sprite3D::init());

    _controller = controller;

    return true;
}

void MenuFrame::onTurnIn()
{
    resetText();
}

void MenuFrame::onTurnOut()
{
    
}