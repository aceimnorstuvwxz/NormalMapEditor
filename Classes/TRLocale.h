// (C) 2015 Turnro.com

#ifndef __Turnroll__TRLocale__
#define __Turnroll__TRLocale__


#include <string>
#include <fstream>
#include <iostream>
#include "cocos2d.h"

#include "../rapidjson/rapidjson.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"


// 同时肩负起其它的设置信息的持久化
class TRLocale
{
public:
    static TRLocale& s(){ return  _instance; }

    void load();
    std::string operator[](const std::string& key);
    void store();
    void recover();
    void setMusicEnable(bool enable);
    bool isMusicEnable();
    void setSfxEnable(bool enable);
    bool isSfxEnable();
    void setMusicVolume(float volume);
    float getMusicVolume();
    void setSfxVolume(float volume);
    float getSfxVolume();
    void applyVolumeSettings();
    std::string font();
    cocos2d::LanguageType getLanguageType();
    void shiftLanguage();
    std::pair<bool, int> getSectionStatic(int sid, bool infinite);
    void setSectionPassed(int sid, bool infinite, bool passed);
    void updateSectionPts(int sid, bool infinite, int pts);

protected:
    const float MUSIC_RADIO = 0.5; //调节系数，用于调节相对的音量。
    const float SFX_RADIO = 0.99;

    bool _hasHandSetted;
    cocos2d::LanguageType _languageType;
    bool _musicEnable;
    bool _sfxEnable;
    float _musicVolume;
    float _sfxVolume;
    TRLocale();
    static TRLocale _instance;
    bool _loaded = false;
    rjson::Document _doc;

};

#endif /* defined(__Turnroll__TRLocale__) */
