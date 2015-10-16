// (C) 2015 Turnro.com

#ifndef ScatOrcScene_hpp
#define ScatOrcScene_hpp

#include "TRBaseScene.h"

class ScatOrcScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(ScatOrcScene);

protected:
    cocos2d::Layer* _layer;

};

#endif /* ScatOrcScene_hpp */
