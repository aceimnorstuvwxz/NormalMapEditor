// (C) 2015 Turnro.com

#include "BattleField.h"
#include "format.h"
#include "DDMapData.h"
#include "TRLocale.h"

USING_NS_CC;

// 处理来自其它地方的消息!!

void BattleField::op_mapdata_message(DDBattleObserveMessage message, int aid_src, int aid_des) // mapdata 来通知battle field各种消息！
{
    CCLOG("mapdata message %d %d %d", static_cast<int>(message), aid_src, aid_des);

}
