#pragma once

#include <basis/seadTypes.h>

#include "Library/LiveActor/LiveActor.h"

namespace al {
struct ActorInitInfo;

class EnemyStateBlowDown;
class HitSensor;
class LiveActor;
class LiveActorGroup;
class MtxConnector;
class NerveStateBase;
class SensorMsg;
}  // namespace al

class EnemyStateReset;
class EnemyStateDamageCap;
class EnemyStateReviveInsideScreen;
class EnemyStateSwoon;
class TankBullet;
class TankStateHack;

class Tank : public al::LiveActor {
public:
    Tank(const char* name);
    void init(const al::ActorInitInfo& initinfo) override;

    void enableShoot();
    void disableShoot();
    void initAfterPlacement() override;
    void appear() override;
    bool isExistAndNearRail();
    void kill() override;
    void control() override;
    void die();
    void calcAnim() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    bool isMyBullet();
    TankBullet shootByPlayer(const sead::Vector3f* vector, f32 float1, u32 int1);
    void isSwoon();
    void appearCtrl();
    void preInitHandleByMofumofu();
    void appearAndDemoWait();
    void endDemoWait();
    void startShootByMofumofu();
    void startBlowDownByMofumofu(al::HitSensor*);
    void startRevive();
    void startRevivePrepare();
    void forceEndHackByMofumofu();
    void setSubjectiveCameraLimitDegree(f32 tmpname, f32 tmpname2);
    void setSubjectiveCameraAimFollowRateV(f32 tmpname);
    void isHacking();
    bool isRevivePrepare();
    bool isEnableStartAttack();
    void turn();
    bool isEnableShoot();

    void exeWait();
    void exeMove();
    void exeHack();
    void exeReset();
    void exeSwoon();
    void exeBlowDown();
    void exeReviveInsideScreenNoAutoRevive();
    void exeReviveInsideScreen();
    void exeDamageCap();
    void exePressDown();
    void exeAppear();
    void exeAttackSign();
    void exeShoot();
    void exeAttackHit();
    void exePressReaction();
    void exeDemoWait();

    s32 countAliveBullet();

private:
    al::LiveActorGroup* mBulletGroup = nullptr;
    TankStateHack* mTankStateHack = nullptr;
    EnemyStateDamageCap* mEnemyStateDamageCap = nullptr;
    EnemyStateReset* mEnemyStateReset = nullptr;
    EnemyStateReviveInsideScreen* mEnemyStateReviveInsideScreen1 = nullptr;
    EnemyStateReviveInsideScreen* mEnemyStateReviveInsideScreen2 = nullptr;
    EnemyStateSwoon* mEnemyStateSwoon = nullptr;
    al::EnemyStateBlowDown* mEnemyStateBlowDown = nullptr;
    al::MtxConnector* mMtxConnector = nullptr;
    sead::Quatf mPose2 = {0.0, 0.0, 0.0, 0.0};
    float mCannonRotator = 0.0;
    float mCannonScalor = 0.0;
    float mHipRotator = 0.0;
    sead::Vector3f mFrontDir = {0.0, 0.0, 0.0};


    float mPosey = 0.0;
    float mPosez = 0.0;
    float mRWheelRotator = 0.0;
    float mLWheelRotator = 0.0;

    bool mIsCapInit = false;
    bool mIsOffCollideAtWait = false;
    sead::Quatf mPose = {0.0, 0.0, 0.0, 0.0};
    float mYRotator = 0.0;
    float mClippingRadius = 0.0;
    bool mCanShoot = true;
    bool mIsShootToCamera = false;
    bool mIsMoonCave = false;
};
