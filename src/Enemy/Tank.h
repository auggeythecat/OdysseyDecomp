#pragma once

#include <basis/seadTypes.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Project/HitSensor/HitSensor.h"

namespace al {
struct ActorInitInfo;

class EnemyStateBlowDown;
template <class T>
class DeriveActorGroup;
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
    friend class TankStateHack;

    Tank(const char* name);
    void init(const al::ActorInitInfo& initinfo) override;

    void enableShoot() { mIsShoot = true; };
    void disableShoot() { mIsShoot = false; };
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
    bool isMyBullet(al::HitSensor* self);
    TankBullet shootByPlayer(const sead::Vector3f* vector, f32 f321, u32 int1);
    bool isSwoon() const;
    void appearCtrl();
    void preInitHandleByMofumofu();
    void appearAndDemoWait();
    void endDemoWait();
    void startShootByMofumofu();
    void startBlowDownByMofumofu(al::HitSensor*);
    void startRevive();
    void startRevivePrepare();
    void forceEndHackByMofumofu();
    void setSubjectiveCameraLimitDegree(f32 camlimitx, f32 camlimity);
    void setSubjectiveCameraAimFollorRateV(f32 aimfollowrateV);
    void isHacking() const;
    bool isRevivePrepare() const;
    bool isEnableStartAttack() const;
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

    s32 countAliveBullet() const;

private:
    al::DeriveActorGroup<TankBullet>* mBulletGroup = nullptr;
    TankStateHack* mTankStateHack = nullptr;
    EnemyStateDamageCap* mEnemyStateDamageCap = nullptr;
    EnemyStateReset* mEnemyStateReset = nullptr;
    EnemyStateReviveInsideScreen* mEnemyStateReviveInsideScreen = nullptr;
    EnemyStateReviveInsideScreen* mEnemyStateReviveInsideScreenCopy = nullptr;
    EnemyStateSwoon* mEnemyStateSwoon = nullptr;
    al::EnemyStateBlowDown* mEnemyStateBlowDown = nullptr;
    al::MtxConnector* mMtxConnector = nullptr;
    sead::Quatf mSpawnPose = sead::Quatf::unit;
    f32 mCannonRotator = 0.0f;
    f32 mCannonScalor = 0.0f;
    f32 mHipRotator = 0.0f;
    sead::Vector3f mFrontDir = sead::Vector3f::ex;
    f32 mWheelRRotator = 0.0f;
    f32 mWheelLRotator = 0.0f;
    s32 mTotalShots = 0;
    s32 mAnimFrame = 0;
    s32 mNerveStep = 0;
    s32 mResetCounter = 0;
    bool mIsCaptured = false;
    bool mIsRailAtEnd = false;
    bool mIsNerveReset = false;
    bool mIsOffCollideAtWait = false;
    sead::Quatf mPose = sead::Quatf::unit;
    f32 mYRotator = 0.0f;
    f32 mClippingRadius = -1.0f;
    bool mIsShoot = true;
    bool mIsShootToCamera = false;
    bool mIsMoonCave = false;
};

static_assert(sizeof(Tank) == 0x1b0);
