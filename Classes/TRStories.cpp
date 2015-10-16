// (C) 2015 Turnro.com

#include "TRStories.h"
#include "format.h"

USING_NS_CC;

TRStories TRStories::_instance;

void TRStories::load()
{
    if (_loaded) return;
    _loaded = true;
    auto data = FileUtils::getInstance()->getStringFromFile("maps/stories.json");

    rjson::Document doc;
    doc.Parse(data.c_str());

    auto& chapters = doc["chapters"];
    for (auto iter = chapters.Begin(); iter != chapters.End(); iter++) {
        TRChapter chapterNode;
        chapterNode._name = (*iter)["name"].GetString();
        auto& sections = (*iter)["sections"];
        for (auto jter = sections.Begin(); jter != sections.End(); jter++) {
            TRSection sectionNode;
            sectionNode._id = (*jter)["id"].GetInt();
            sectionNode._name = (*jter)["name"].GetString();
            sectionNode._cname = chapterNode._name;
            sectionNode._message = (*jter)["message"].GetString();
            sectionNode._bgm = (*jter)["bgm"].GetString();
            sectionNode._maxmap = (*jter)["maxmap"].GetString();
            sectionNode._lbid = (*jter)["lbid"].GetInt();
            chapterNode._sections.push_back(sectionNode);
        }
        _chapters.push_back(chapterNode);
    }

    // 算_hasNext和_nextSid
    bool hasNext = false;
    int nextSid = 0;
    for (auto iter = _chapters.rbegin(); iter != _chapters.rend(); iter++) {
        for (auto jter = iter->_sections.rbegin(); jter != iter->_sections.rend(); jter++) {
            jter->_hasNext = hasNext;
            jter->_nextSid = nextSid;
            hasNext = true;
            nextSid = jter->_id;
        }
    }

    // map
    for (auto iter = _chapters.begin(); iter != _chapters.end(); iter++) {
        for (auto& section : iter->_sections) {
            _sectionMap[section._id] = &section;
        }
    }
}

void TRStories::loadEndless()
{
    if (_endlessLoaded) return;
    _endlessLoaded = true;
    auto data = FileUtils::getInstance()->getStringFromFile("maps/endless.json");

    rjson::Document doc;
    doc.Parse(data.c_str());

    auto& chapters = doc["chapters"];
    for (auto iter = chapters.Begin(); iter != chapters.End(); iter++) {
        TRChapter chapterNode;
        chapterNode._name = (*iter)["name"].GetString();
        auto& sections = (*iter)["sections"];
        for (auto jter = sections.Begin(); jter != sections.End(); jter++) {
            TRSection sectionNode;
            sectionNode._id = (*jter)["id"].GetInt();
            sectionNode._name = (*jter)["name"].GetString();
            sectionNode._cname = chapterNode._name;
            sectionNode._message = (*jter)["message"].GetString();
            sectionNode._bgm = (*jter)["bgm"].GetString();
            sectionNode._maxmap = (*jter)["maxmap"].GetString();
            sectionNode._lbid = (*jter)["lbid"].GetInt();
            sectionNode._hasNext = false;
            sectionNode._nextSid = 0;
            chapterNode._sections.push_back(sectionNode);

        }
        _endlessChapters.push_back(chapterNode);
    }

    for (auto iter = _endlessChapters.begin(); iter != _endlessChapters.end(); iter++) {
        for (auto& section : iter->_sections) {
            _endlessSectionMap[section._id] = &section;
        }
    }
}

std::vector<TRChapter>& TRStories::getChapters()
{
    return  _chapters;
}

TRSection& TRStories::getSection(int id)
{
    return *_sectionMap[id];
}

TRSection& TRStories::getEndlessSection(int sid)
{
    return *_endlessSectionMap[sid];
}

TRStories::TRStories(){};


