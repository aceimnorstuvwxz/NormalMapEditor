// (C) 2015 Turnro.com

#ifndef DDProtocals_hpp
#define DDProtocals_hpp

enum class DDBattleObserveMessage:int
{
    MINE,//矿机挖到了矿（矿机，矿）。
    MINE_IDLE,//矿机无矿可挖(矿机）

    CURE,//治疗(治疗者, 被治疗者)
//    CURE_SELF,//Agent自我治疗(Agent)

    ATTACK,//攻击 （攻击者，被攻击者）
    ATTACK_SPUTT,//攻击(溅射) （溅射中心，被溅射者）

    ENEMY_IN, //怪物的进入 (怪物)
    ENEMY_OUT, //怪物的出去 (怪物)
    ENEMY_NEW, //母巢产生新敌人 (母巢，新怪物)
    ENEMY_DIE, //敌人死亡 (怪物)
    ENEMY_MOVE, //敌人移动 (怪物)

    FRIEND_UPGRADE, //建筑的升级 (我军)
    FRIEND_NEW, //放入新我军 (新我军, -1)
    FRIEND_DIE, //我军死亡 (我军)
    FRIEND_SELL, //我军被卖 (我军)

    REMOVE_3RD, //第三方被挖走 (第三方)
};


class DDBattleFieldProtocal
{
public:
    virtual void op_switch2minmap() = 0; // bigmap 来通知battle field切换minmap!
    virtual void op_mapdata_message(DDBattleObserveMessage message, int aid_src, int aid_des) = 0; // mapdata 来通知battle field各种消息！
};

class DDBuildingFieldProtocal
{
public:
    virtual void op_cancelSelection() = 0; // 在其它地方点击后，通知取消BuildingField区域的选择框！
    virtual void op_tellResourceChanged() = 0; // 在资源变化时，告知BuildingField可能需要改变各建筑的是否够资源修建性！
};


class DDPropertyFieldProtocal
{
public:
};



#endif /* DDProtocals_hpp */
