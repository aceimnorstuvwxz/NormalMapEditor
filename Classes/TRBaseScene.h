// (C) 2015 Turnro Game

#ifndef __mcdrunning__TRBaseScene__
#define __mcdrunning__TRBaseScene__

#include <cocos2d.h>
#include "McdLayer.h"
#include <unordered_map>

class McdLayer;
class TRBaseScene:public cocos2d::Scene
{
public:
    typedef std::function<void(void)> COMMON_CALLBACK;

    virtual bool init() override;
    CREATE_FUNC(TRBaseScene);


    void addCommonBg();
    void addCommonBackBtn(COMMON_CALLBACK callback);
    cocos2d::Label* addCommonLabel(const cocos2d::Vec2& pos, const std::string& text, int size=25);
    cocos2d::ui::Button* addCommonBtn(const cocos2d::Vec2& pos, const std::string& text, COMMON_CALLBACK callback);
    cocos2d::Vec2 genPos(const cocos2d::Vec2& pos);
    cocos2d::Vec3 genPos3D(const cocos2d::Vec2& pos, float z);
    std::string genKey();
protected:
    McdLayer* _defaultLayer;
    COMMON_CALLBACK _backCallBack;
    int _btnAI = 0;
    std::unordered_map<int, COMMON_CALLBACK> _cbMap;

    static int _keyAi;
};

#endif /* defined(__mcdrunning__TRBaseScene__) */
