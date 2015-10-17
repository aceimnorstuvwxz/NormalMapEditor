// (C) 2015 Turnro Game
#ifndef __cube3d__WelcomeScene__
#define __cube3d__WelcomeScene__

#include "cocos2d.h"
#include "TRBaseScene.h"

class WelcomeScene: public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(WelcomeScene);

private:

    cocos2d::Label* _lbInput;
    void onKeyPressed(const cocos2d::EventKeyboard::KeyCode& code);
    std::string _input;
    bool _isUpcase = false;
    void onInputFinish();
};

#endif /* defined(__cube3d__WelcomeScene__) */
