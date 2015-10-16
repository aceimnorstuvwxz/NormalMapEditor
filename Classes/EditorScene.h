// (C) 2015 Turnro.com

#ifndef EditorScene_hpp
#define EditorScene_hpp

#include "TRBaseScene.h"

class EditorScene:public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(EditorScene);

protected:
    cocos2d::Layer* _layer;

};

#endif /* EditorScene_hpp */
