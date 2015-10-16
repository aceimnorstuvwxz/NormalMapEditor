// (C) 2015 Turnro.com

#include "TRLocale.h"
#include "format.h"
#include "SimpleAudioEngine.h"
#include "GameSharing.h"
#include "TRStories.h"

USING_NS_CC;
TRLocale TRLocale::_instance;

/*
 {
 ENGLISH = 0,
 CHINESE,
 FRENCH,
 ITALIAN,
 GERMAN,
 SPANISH,
 DUTCH,
 RUSSIAN,
 KOREAN,
 JAPANESE,
 HUNGARIAN,
 PORTUGUESE,
 ARABIC,
 NORWEGIAN,
 POLISH,
 TURKISH,
 UKRAINIAN,
 ROMANIAN,
 BULGARIAN
 };
 */

static std::string type2key(const LanguageType& type)
{
    switch (type) {
        case cocos2d::LanguageType::ENGLISH:
            return "ENGLISH";
        case cocos2d::LanguageType::CHINESE:
            return "CHINESE";
        case cocos2d::LanguageType::FRENCH:
            return "FRENCH";
        case cocos2d::LanguageType::ITALIAN:
            return "ITALIAN";
        case cocos2d::LanguageType::GERMAN:
            return "GERMAN";
        case cocos2d::LanguageType::SPANISH:
            return "SPANISH";
        case cocos2d::LanguageType::DUTCH:
            return "DUTCH";
        case cocos2d::LanguageType::RUSSIAN:
            return "RUSSIAN";
        case cocos2d::LanguageType::KOREAN:
            return "KOREAN";
        case cocos2d::LanguageType::JAPANESE:
            return "JAPANESE";
        case cocos2d::LanguageType::HUNGARIAN:
            return "HUNGARIAN";
        case cocos2d::LanguageType::PORTUGUESE:
            return "PORTUGUESE";
        case cocos2d::LanguageType::ARABIC:
            return "ARABIC";
        case cocos2d::LanguageType::NORWEGIAN:
            return "NORWEGIAN";
        case cocos2d::LanguageType::POLISH:
            return "POLISH";
        case cocos2d::LanguageType::TURKISH:
            return "TURKISH";
        case cocos2d::LanguageType::UKRAINIAN:
            return "UKRAINIAN";
        case cocos2d::LanguageType::ROMANIAN:
            return "ROMANIAN";
        case cocos2d::LanguageType::BULGARIAN:
            return "BULGARIAN";
        default:
            return "ENGLISH";
    }
}

std::string TRLocale::operator[](const std::string& key)
{
    if (!_loaded) load();
    auto& map = _doc[type2key(getLanguageType()).c_str()];
    return map[key.c_str()].GetString();
}

std::string TRLocale::font()
{
    if (!_loaded) load();
    auto& map = _doc[type2key(getLanguageType()).c_str()];
    return fmt::sprintf("fonts/%s.ttf", map["__font__"].GetString());
}

void TRLocale::load()
{
    _loaded = true;
    std::ifstream fin;
    auto data = cocos2d::FileUtils::getInstance()->getDataFromFile("string.json");
    unsigned char * pc = (unsigned char *)malloc(sizeof(unsigned char) * (data.getSize() + 1));
    for (int i = 0; i < data.getSize(); i++) {
        pc[i] = data.getBytes()[i];
    }
    pc[data.getSize()] = '\0';

    _doc.Parse((char*)pc);

    free(pc);
}

LanguageType TRLocale::getLanguageType()
{
    if (_hasHandSetted) {
        return _languageType;
    } else {
        // fix bug of crash in other language!
        auto la = Application::getInstance()->getCurrentLanguage();
        if (la != cocos2d::LanguageType::CHINESE) {
            return cocos2d::LanguageType::ENGLISH;
        } else {
            return cocos2d::LanguageType::CHINESE;
        }
    }
}

void TRLocale::shiftLanguage()
{
    //TODO 等所有翻译完后再进行真的shifting
    _hasHandSetted = true;
    if (_languageType == cocos2d::LanguageType::CHINESE) _languageType = cocos2d::LanguageType::ENGLISH;
    else _languageType = cocos2d::LanguageType::CHINESE;
}

void TRLocale::store()
{
    CCLOG("store config");
    UserDefault::getInstance()->setBoolForKey("language_setted", _hasHandSetted);
    UserDefault::getInstance()->setIntegerForKey("language_type", static_cast<int>(_languageType));
    UserDefault::getInstance()->setBoolForKey("sfx_enable", _sfxEnable);
    UserDefault::getInstance()->setBoolForKey("music_enable", _musicEnable);
    UserDefault::getInstance()->setFloatForKey("music_volume", _musicVolume);
    UserDefault::getInstance()->setFloatForKey("sfx_volume", _sfxVolume);
}

void TRLocale::recover()
{
    CCLOG("recover config");
    _hasHandSetted = UserDefault::getInstance()->getBoolForKey("language_setted", false);
    _languageType = static_cast<LanguageType>( UserDefault::getInstance()->getIntegerForKey("language_type", 0));
    _sfxEnable = UserDefault::getInstance()->getBoolForKey("sfx_enable", true);
    _musicEnable = UserDefault::getInstance()->getBoolForKey("music_enable", true);
    _musicVolume = UserDefault::getInstance()->getFloatForKey("music_volume", 1.f);
    _sfxVolume = UserDefault::getInstance()->getFloatForKey("sfx_volume", 1.f);
}

void TRLocale::setMusicEnable(bool enable)
{
    _musicEnable = enable;
}

bool TRLocale::isMusicEnable()
{
//    return  _musicEnable;
    return true;
    // 使用volume来控制。弃用enable xxx。
}

void TRLocale::setSfxEnable(bool enable)
{
    _sfxEnable = enable;
}

bool TRLocale::isSfxEnable()
{
//    return _sfxEnable;
    return  true;
}

void TRLocale::setMusicVolume(float volume)
{
    _musicVolume = volume;
}
float TRLocale::getMusicVolume()
{
    return _musicVolume;
}
void TRLocale::setSfxVolume(float volume)
{
    _sfxVolume = volume;
}

float TRLocale::getSfxVolume()
{
    return _sfxVolume;
}

void TRLocale::applyVolumeSettings()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(_musicVolume*MUSIC_RADIO);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(_sfxVolume*SFX_RADIO);
}

std::pair<bool, int> TRLocale::getSectionStatic(int sid, bool infinite)
{
    bool ifpass = UserDefault::getInstance()->getBoolForKey(fmt::sprintf("%d_ifpass_%s", sid, infinite?"infinite":"normal").c_str(), false);
    int pts = UserDefault::getInstance()->getIntegerForKey(fmt::sprintf("%d_pts_%s", sid, infinite?"infinite":"normal").c_str(), 0);
    return {ifpass, pts};
}

void TRLocale::setSectionPassed(int sid, bool infinite, bool passed)
{
    UserDefault::getInstance()->setBoolForKey(fmt::sprintf("%d_ifpass_%s", sid, infinite?"infinite":"normal").c_str(), passed);
}

void TRLocale::updateSectionPts(int sid, bool infinite, int pts)
{
    int oldPts = UserDefault::getInstance()->getIntegerForKey(fmt::sprintf("%d_pts_%s", sid, infinite?"infinite":"normal").c_str(), 0);
    if (pts > oldPts) {
        UserDefault::getInstance()->setIntegerForKey(fmt::sprintf("%d_pts_%s", sid, infinite?"infinite":"normal").c_str(), pts);
        auto lbid = infinite ? TRStories::s().getEndlessSection(sid)._lbid:TRStories::s().getSection(sid)._lbid;
//        GameSharing::submitScoreToLeaderboard(pts, lbid);
    }
}

TRLocale::TRLocale()
{}
