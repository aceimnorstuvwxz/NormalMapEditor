// (C) 2015 Turnro Game

#include "EditState.h"

#include "format.h"
#include "TRStories.h"
#include "TRLocale.h"

USING_NS_CC;
EditState EditState::_instance;

void EditState::config(int sectionId,  bool infinity)
{
    _sectionId = sectionId;
    _infinity = infinity;
}

std::string EditState::getMapFile()
{
    return _infinity ? TRStories::s().getEndlessSection(_sectionId)._maxmap : TRStories::s().getSection(_sectionId)._maxmap;
}
std::string EditState::getBgmFile()
{
    return _infinity ? TRStories::s().getEndlessSection(_sectionId)._bgm : TRStories::s().getSection(_sectionId)._bgm;
}

std::string EditState::getSectionName()
{
    return TRLocale::s()[_infinity ? TRStories::s().getEndlessSection(_sectionId)._name : TRStories::s().getSection(_sectionId)._name];
}
std::string EditState::getChapterName()
{
    return TRLocale::s()[_infinity ? TRStories::s().getEndlessSection(_sectionId)._cname : TRStories::s().getSection(_sectionId)._cname];
}
std::string EditState::getSectionQuote()
{
    return TRLocale::s()[_infinity ? TRStories::s().getEndlessSection(_sectionId)._message : TRStories::s().getSection(_sectionId)._message];
}
std::string EditState::getEnvSoundFile()
{
    return "sounds/ambience/11.mp3";
}
std::string EditState::getBgmTextureFile()
{
    return fmt::sprintf("%s.wav.png", getBgmFile());
}