// (C) 2015 Turnro.com

#ifndef __Turnroll__TRChapterSection__
#define __Turnroll__TRChapterSection__

#include <string>
#include <fstream>
#include <iostream>
#include "cocos2d.h"

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"

struct TRSection
{
    int _id;
    std::string _name;//section name
    std::string _cname;//chapter name
    std::string _message;
    std::string _bgm;
    std::string _maxmap;
    int _lbid;//leaderboard
    bool _hasNext;
    int _nextSid;
};

struct TRChapter
{
    std::string _name;
    std::vector<TRSection> _sections;
};

class TRStories
{
public:
    static TRStories& s(){ return  _instance; }

    void load();
    void loadEndless();
    std::vector<TRChapter>& getChapters();
    TRSection& getSection(int sid);
    TRSection& getEndlessSection(int sid);

protected:
    TRStories();
    static TRStories _instance;
    bool _loaded = false;
    bool _endlessLoaded = false;
    std::vector<TRChapter> _chapters;
    std::vector<TRChapter> _endlessChapters;
    std::unordered_map<int, TRSection*> _sectionMap;
    std::unordered_map<int, TRSection*> _endlessSectionMap;
};


#endif /* defined(__Turnroll__TRChapterSection__) */
