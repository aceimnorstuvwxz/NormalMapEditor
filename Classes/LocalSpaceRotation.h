// (C) 2015  Turnro.com

#ifndef __Turnroll__LocalSpaceRotation__
#define __Turnroll__LocalSpaceRotation__


#include "cocos2d.h"


NS_CC_BEGIN;
// Object Local空间的旋转。

// 原生的RotateBy/RotateTo都是在World Space旋转的。
// 如果要一个3D物体在自己坐标下选择，在原生情况下只能用take X动画。
// 这里提供另一个选择。

inline Quaternion rotation2quaternion(const Vec3& rotation)
{
    float halfRadx = CC_DEGREES_TO_RADIANS(rotation.x / 2.f), halfRady = CC_DEGREES_TO_RADIANS(rotation.y / 2.f), halfRadz =  -CC_DEGREES_TO_RADIANS(rotation.z / 2.f);
    float coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
    return {
        sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz,
        coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz,
        coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz,
        coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz
    };
}

class LocalSpaceRotateBy : public ActionInterval
{
public:
    static LocalSpaceRotateBy* create(float duration, const Vec3& angel);

    virtual LocalSpaceRotateBy* clone() const override;
    virtual LocalSpaceRotateBy* reverse(void) const override;
    virtual void startWithTarget(Node* target) override;

    virtual void update(float radio) override;

CC_CONSTRUCTOR_ACCESS:
    LocalSpaceRotateBy();
    virtual ~LocalSpaceRotateBy(){}
    bool init(float duration, const Vec3& angel);


protected:
    Quaternion _startQua;
    Vec3 _angel;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(LocalSpaceRotateBy);
};


class LocalSpaceRotateFromTo : public ActionInterval
{
public:
    static LocalSpaceRotateFromTo* create(float duration, const Quaternion& from, const Quaternion& to);

    virtual LocalSpaceRotateFromTo* clone() const override;
    virtual LocalSpaceRotateFromTo* reverse(void) const override;
    virtual void startWithTarget(Node* target) override;

    virtual void update(float radio) override;

CC_CONSTRUCTOR_ACCESS:
    LocalSpaceRotateFromTo();
    virtual ~LocalSpaceRotateFromTo(){}
    bool init(float duration, const Quaternion& from, const Quaternion& to);

protected:
    Quaternion _fromQua;
    Quaternion _toQua;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(LocalSpaceRotateFromTo);
};

class MoveFromTo : public ActionInterval
{
public:
    static MoveFromTo* create(float duration, const Vec3& from, const Vec3& to);

    virtual MoveFromTo* clone() const override;
    virtual MoveFromTo* reverse(void) const override;
    virtual void startWithTarget(Node* target) override;

    virtual void update(float radio) override;

CC_CONSTRUCTOR_ACCESS:
    MoveFromTo();
    virtual ~MoveFromTo(){}
    bool init(float duration, const Vec3& from, const Vec3& to);

protected:
    Vec3 _fromPos;
    Vec3 _toPos;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(MoveFromTo);
};



NS_CC_END;
#endif /* defined(__Turnroll__LocalSpaceRotateBy__) */
