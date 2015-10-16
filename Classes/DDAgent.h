

#ifndef DDAgent_hpp
#define DDAgent_hpp

#include "DDConfig.h"



// Agent元素的系列定义
enum class DDElementType : int
{
    Metal = 0,
    Wood = 1,
    Water = 2,
    Fire = 3,
    Earth = 4,
    None = 5
};

inline DDElementType genRandomElementType()
{
    int a = std::abs(cocos2d::random())%5;
    return static_cast<DDElementType>(a);
}


struct AgentPos
{
    int x;
    int y;

    int distance(const AgentPos& r)
    {
        return std::abs(r.x-x) + std::abs(r.y-y);
    }
    bool isLegal()
    {
        return std::max(std::abs(x), std::abs(y)) <= DDConfig::MINMAP_EXPAND;
    }
};

bool operator==(const AgentPos& r, const AgentPos& l);

namespace std{
        template<>
        class hash<AgentPos> {
            public :
            size_t operator()(const AgentPos &p) const {
                return p.y * 1000 + p.x;
            }
        };
}

class DDAgent
{
public:
    enum AgentType{
        /* 注意要与dungeonMapGen.py内同步 */
        AT_3RD_MINE = 0,
        AT_3RD_STONE = 1,
        AT_3RD_TREE = 2,
        AT_3RD_WATER = 3,
        AT_3RD_VOLCANO = 4,

        AT_ENEMY_FAR = 5,
        AT_ENEMY_NEAR = 6,
        AT_ENEMY_NEST = 7,

        AT_FRIEND_ARROW_TOWER = 8,
        AT_FRIEND_CONNON_TOWER = 9,
        AT_FRIEND_CORE = 10,
        AT_FRIEND_CURE_TOWER = 11,
        AT_FRIEND_LIGHT_TOWER = 12,
        AT_FRIEND_MAGIC_TOWER = 13,
        AT_FRIEND_MINER = 14,
        AT_FRIEND_WALL = 15,

        AT_MAX
    };
    int aid;
    AgentPos pos;
    int type;
    int level;

    int bloodMax = 1;//血量最大值
    int blood = 1;//血量
    int attack = 0;//攻击
    int shield = 0;//护盾
    int unbeatableAmount = 0;//无敌剩余计数，>0时，被攻击无效
    int cure = 0;//治疗
    int cureNum = 1;//能够治疗的单位数
    int selfCure = 0;//自我治疗
    int selfCureAmount = 0;//剩余自我治疗次数，负值表示持久治疗
    DDElementType elementType = DDElementType::None;//属性（金木水火土）
    //    int attackEffectSlowDown;//攻击导致对方减速
    int actionDistance = 0;//机能距离（攻击和治疗和挖矿）
    int actionPeriod = 10;//机能间隔
    int actionPeriodIndex = 10;//机能间隔计时


//    int movePeriod;//移动间隔（移动速度相关） 移动与actionPeriod共用
//    int movePeriodIndex;//移动间隔计时
//    int slowDownCount;//剩余的减速时间


//    //墙
//    enum TYPE_WALL{
//        SINGLE,
//        HORIZONTAL,
//        VERTICAL
//    };
//    int wallType;


    //矿 && 矿机
    int mineSpeed = 0;
    int mineCapacity = 0;
    int mineAmount = 0;


    //炮
    int cannonSputtDistance = 1;
    float cannonSputtDamageRadio = 1;//[1-10] 实际*0.1计算伤害！

    //怪
    bool isBoss = false;

    // 攻击目标锁定
    bool hasLockedTarget = false;
    AgentPos lockedTargetPos;

    //Nest
    float nestChanceToRelax = 0;//[0-1]出现休息片段的机会
    int nestActionRelaxPeriod =  0;//休息时长
    int nestActionRelaxIndex = 0;//休息计时

    float nestChanceToNear = 1;//近战怪的可能性
    float nestChanceToBoss = 0;//头目的可能性(boss所有属性提高一倍)

    int nestBlood = 1;
    int nestAttack = 0;
    DDElementType nestElementType = DDElementType::None;//属性（金木水火土）
    int nestAttackDistanceNear = 1;//机能距离（攻击和治疗）
    int nestAttackDistanceFar = 2;//机能距离（攻击和治疗）
    int nestAttackPeriod = 10;//怪物的机能间隔

    bool isFriend(){
        return type == AT_FRIEND_MINER || type == AT_FRIEND_WALL || type == AT_FRIEND_ARROW_TOWER || type == AT_FRIEND_MAGIC_TOWER || type == AT_FRIEND_CONNON_TOWER || type == AT_FRIEND_CURE_TOWER || type == AT_FRIEND_LIGHT_TOWER || type == AT_FRIEND_CORE;
    }
};

static std::string agentType2agentTypeString(int agentType)
{
        /*    enum AgentType{
         AT_3RD_MINE,
         AT_3RD_STONE,
         AT_3RD_TREE,
         AT_3RD_WATER,
         AT_3RD_VOLCANO,

         AT_ENEMY_FAR,
         AT_ENEMY_NEAR,
         9*
         AT_FRIEND_ARROW_TOWER,
         AT_FRIEND_CONNON_TOWER,
         AT_FRIEND_CORE,
         AT_FRIEND_CURE_TOWER,
         AT_FRIEND_LIGHT_TOWER,
         AT_FRIEND_MAGIC_TOWER,
         AT_FRIEND_MINER,
         AT_FRIEND_WALL,
         };*/
    switch (agentType) {
        case DDAgent::AT_3RD_MINE:
            return "3rd_mine";
        case DDAgent::AT_3RD_STONE:
            return "3rd_stone";
        case DDAgent::AT_3RD_TREE:
            return "3rd_tree";
        case DDAgent::AT_3RD_VOLCANO:
            return "3rd_volcano";
        case DDAgent::AT_3RD_WATER:
            return "3rd_water";
        case DDAgent::AT_ENEMY_FAR:
            return "enemy_far";
        case DDAgent::AT_ENEMY_NEAR:
            return "enemy_near";
        case DDAgent::AT_ENEMY_NEST:
            return "enemy_nest";
        case DDAgent::AT_FRIEND_ARROW_TOWER:
            return "friend_arrow_tower";
        case DDAgent::AT_FRIEND_CONNON_TOWER:
            return "friend_cannon_tower";
        case DDAgent::AT_FRIEND_CORE:
            return "friend_core";
        case DDAgent::AT_FRIEND_CURE_TOWER:
            return "friend_cure_tower";
        case DDAgent::AT_FRIEND_LIGHT_TOWER:
            return "friend_light_tower";
        case DDAgent::AT_FRIEND_MAGIC_TOWER:
            return "friend_magic_tower";
        case DDAgent::AT_FRIEND_MINER:
            return "friend_miner";
        case DDAgent::AT_FRIEND_WALL:
            return "friend_wall";

        default:
            return "";
            break;
    }

}


#endif /* DDAgent_hpp */
