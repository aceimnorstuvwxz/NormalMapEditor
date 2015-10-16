// (C) 2015 Turnro.com

#include "LocalSpaceRotation.h"


NS_CC_BEGIN;



LocalSpaceRotateBy* LocalSpaceRotateBy::create(float duration, const Vec3& angel)
{
    LocalSpaceRotateBy* p = new (std::nothrow) LocalSpaceRotateBy();
    p->init(duration, angel);
    p->autorelease();
    return p;
}

LocalSpaceRotateBy::LocalSpaceRotateBy(){}

bool LocalSpaceRotateBy::init(float duration, const Vec3& angel)
{
    if (ActionInterval::initWithDuration(duration)) {
        _angel = angel;
        return true;
    }

    return false;
}


LocalSpaceRotateBy* LocalSpaceRotateBy::clone() const
{
    auto p = new (std::nothrow) LocalSpaceRotateBy();
    p->init(_duration, _angel);
    p->autorelease();
    return p;
}

LocalSpaceRotateBy* LocalSpaceRotateBy::reverse(void) const
{
    auto p = new (std::nothrow) LocalSpaceRotateBy();
    p->init(_duration, -_angel);
    p->autorelease();
    return p;
}

void LocalSpaceRotateBy::startWithTarget(cocos2d::Node *target)
{
    ActionInterval::startWithTarget(target);
    _startQua = target->getRotationQuat();
}

void LocalSpaceRotateBy::update(float radio)
{
    if (_target) {
        Vec3 rot = _angel*radio;

        auto tmpQua = _startQua;
        tmpQua.multiply(rotation2quaternion(rot));
        _target->setRotationQuat(tmpQua);
    }
}


///////LocalSpaceRotateFromTo////////


LocalSpaceRotateFromTo* LocalSpaceRotateFromTo::create(float duration, const Quaternion& from, const Quaternion& to)
{
    LocalSpaceRotateFromTo* p = new (std::nothrow) LocalSpaceRotateFromTo();
    p->init(duration, from, to);
    p->autorelease();
    return p;
}

LocalSpaceRotateFromTo::LocalSpaceRotateFromTo(){}

bool LocalSpaceRotateFromTo::init(float duration, const Quaternion& from, const Quaternion& to)
{
    if (ActionInterval::initWithDuration(duration)) {
        _fromQua = from;
        _toQua = to;
        return true;
    }

    return false;
}

LocalSpaceRotateFromTo* LocalSpaceRotateFromTo::clone() const
{
    auto p = new (std::nothrow) LocalSpaceRotateFromTo();
    p->init(_duration, _fromQua, _toQua);
    p->autorelease();
    return p;
}

LocalSpaceRotateFromTo* LocalSpaceRotateFromTo::reverse(void) const
{
    auto p = new (std::nothrow) LocalSpaceRotateFromTo();
    p->init(_duration, _toQua, _fromQua);
    p->autorelease();
    return p;
}

void LocalSpaceRotateFromTo::startWithTarget(cocos2d::Node *target)
{
    ActionInterval::startWithTarget(target);
}

void LocalSpaceRotateFromTo::update(float radio)
{
    if (_target) {

        Quaternion dst;
        Quaternion::slerp(_fromQua, _toQua, radio, &dst);
        _target->setRotationQuat(dst);
    }
}

///////MoveFromTo////////


MoveFromTo* MoveFromTo::create(float duration, const Vec3& from, const Vec3& to)
{
    MoveFromTo* p = new (std::nothrow) MoveFromTo();
    p->init(duration, from, to);
    p->autorelease();
    return p;
}

MoveFromTo::MoveFromTo(){}

bool MoveFromTo::init(float duration, const Vec3& from, const Vec3& to)
{
    if (ActionInterval::initWithDuration(duration)) {
        _fromPos = from;
        _toPos = to;
        return true;
    }

    return false;
}

MoveFromTo* MoveFromTo::clone() const
{
    auto p = new (std::nothrow) MoveFromTo();
    p->init(_duration, _fromPos, _toPos);
    p->autorelease();
    return p;
}

MoveFromTo* MoveFromTo::reverse(void) const
{
    auto p = new (std::nothrow) MoveFromTo();
    p->init(_duration, _toPos, _fromPos);
    p->autorelease();
    return p;
}

void MoveFromTo::startWithTarget(cocos2d::Node *target)
{
    ActionInterval::startWithTarget(target);
}

void MoveFromTo::update(float radio)
{
    if (_target) {
        _target->setPosition3D(_fromPos.lerp(_toPos, radio));
    }
}


NS_CC_END;

