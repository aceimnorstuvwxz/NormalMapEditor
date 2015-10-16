// (C) 2015 Turnro.com

#ifndef __TurnrollMapEditor__RollNode__
#define __TurnrollMapEditor__RollNode__

#include "cocos2d.h"
#include <vector>

class RollNode
{
public:
    enum RollNodeType{
        RT_REAL,
        RT_UNREAL
    };
    bool _end;
    bool _glass;
    bool _gate;
    bool _uncover;
    int _type;
    cocos2d::Vec3 _position;
    cocos2d::Quaternion _posture;
    cocos2d::Vec3 _leftPoint;
    cocos2d::Vec3 _rightPoint;
    RollNode():_type(RT_REAL),_position(0,0,0),_end(false),_glass(false),_gate(false),_uncover(false){};
    RollNode(int type, const cocos2d::Vec3& position, const cocos2d::Quaternion& posture, bool glass, bool end, bool uncover):_type(type), _position(position),  _posture(posture), _end(end), _glass(glass), _uncover(uncover){};
};

class AssetNode
{
public:
    cocos2d::Vec3 _position;
};

// 悬浮，普通小方块
class FloatAssetNode: public AssetNode
{};

// 深渊，下方直抵地狱的柱子?
class AbyssAssetNode: public AssetNode
{};

// 天坠，从天空坠下来的柱子?
class SkyAssetNode: public AssetNode
{};

class RollMinMap
{
public:
    bool _end = false;
    void load(const std::string& file);
    std::vector<RollNode> _data;
    std::vector<FloatAssetNode> _floatAssets;
    std::vector<AbyssAssetNode> _abyssAssets;
    std::vector<SkyAssetNode> _skyAssets;
};

#endif /* defined(__TurnrollMapEditor__RollNode__) */
