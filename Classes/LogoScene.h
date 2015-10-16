// (C) 2015 Turnro Game

#ifndef __mcdrunning__LogoScene__
#define __mcdrunning__LogoScene__

#include "cocos2d.h"

class LogoScene : public cocos2d::Layer
{
public:

    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(LogoScene);
private:
    const float default_logo_time = 1.0f;
    const std::vector<std::pair<std::string, float> > logoImages = {
        {"images/logo_v0.png", default_logo_time}
    };
    const float full_show = 0.6f;
};

#endif /* defined(__mcdrunning__LogoScene__) */
