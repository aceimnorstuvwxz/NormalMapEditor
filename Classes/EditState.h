// (C) 2015 Turnro Game

#ifndef __cube3d__EditState__
#define __cube3d__EditState__

#include "cocos2d.h"
#include <string>
#include "RollNode.h"
#include "Maxmap.h"

#define RoadHalfWidth  0.5f
#define StaticGlassHeight 3.f

class EditState
{
public:
    static EditState* s(){ return &_instance; };
    void config(int sectionId, bool infinity);
    std::string getMapFile();
    std::string getBgmFile();
    std::string getSectionName();
    std::string getChapterName();
    std::string getSectionQuote();
    std::string getEnvSoundFile();
    std::string getBgmTextureFile();
    int getSectionId() { return _sectionId; }
    bool isInfinite() { return _infinity; }
    bool _needPause = false;
private:
    static EditState _instance;
    int _sectionId;
    bool _infinity;
};

#endif /* defined(__cube3d__EditState__) */
