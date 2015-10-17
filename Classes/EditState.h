// (C) 2015 Turnro Game

#ifndef __cube3d__EditState__
#define __cube3d__EditState__

#include "cocos2d.h"
#include <string>


class EditState
{
public:
    static EditState* s(){ return &_instance; };
    std::string _moduleName;
private:
    static EditState _instance;
};

#endif /* defined(__cube3d__EditState__) */
