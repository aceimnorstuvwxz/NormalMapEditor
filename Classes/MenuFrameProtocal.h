// (C) 2015 Turnro.com

#ifndef __Turnroll__MenuFrameProtocal__
#define __Turnroll__MenuFrameProtocal__


enum __frame_id_def__{
    MENU_MAIN,
    MENU_STORIES,
    MENU_SETTINGS,
    MENU_ROCORDS,
    MENU_HELP,
    MENU_LOADING,
    MENU_CREDIT,
    MENU_ENDLESS,
    MENU_MAX
};

class MenuFrameProtocal
{
public:
    virtual void move2frame(int frameid) = 0;
    virtual cocos2d::Camera* getMenuCamera() = 0;
    virtual void resetText() = 0;
};

#endif /* defined(__Turnroll__MenuFrameProtocal__) */
