#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/NerveStateBase.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
class NerveKeeper;
class ActorStateBase;
class IUseCamera;
class IntervalTrigger;
class IUseCamera;
class LiveActor;
class CameraTicket;
class CameraArrowCollider;
}  // namespace al

class AimingCursor;
class HackerJudgeNormalFall;
class Tank;
class CapTargetInfo;
class IJudge;
class IUsePlayerHack;
class EnemyStateHackStart;

class TankStateHack : public al::ActorStateBase {
public:
    TankStateHack(Tank* parent, const al::ActorInitInfo& initinfo, f32* float1, f32* float2,
                  sead::Vector3f* vec3, sead::Quatf* quat, f32* float3);
    void appear();
    void kill();
    void control();
    void reset();

    void receiveMsgInitCapTargetInfo(const al::SensorMsg* message);
    void receiveMsgNpcScareByEnemy(const al::SensorMsg* message);
    bool receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* other,
                             al::HitSensor* self);
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other);
    bool receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other);

    void endHack();

    void attackSensor(al::HitSensor* self, al::HitSensor* other);
    void updatePose();
    void updateVelocity(bool calcmove);
    void updateCamera();
    bool tryChangeNerveIfTrigerShoot();
    bool forceEndIfHack();
    void calcAimCursorLayoutPos();
    void shoot();

    void exeStartDemo();
    void exeWait();
    void exeMove();
    void exeShoot();
    void exeShootReload();
    void exeFall();
    void exeLand();

private:
    bool mIsDead = false;
    al::LiveActor* mLiveActor = nullptr;
    Tank* mTankActor = nullptr;
    AimingCursor* mAimingCursor = nullptr;
    IUsePlayerHack* mIUsePlayerHack = nullptr;
    const CapTargetInfo* mCapTargetInfo = nullptr;
    EnemyStateHackStart* mHackStart = nullptr;
    HackerJudgeNormalFall* mHackerJudgeFall = nullptr;
    f32* mFloat1 = nullptr;
    f32* mFloat2 = nullptr;
    sead::Vector3f* mVec3 = nullptr;
    u64 mInt1 = 0;
    u32 mInt2 = 0;
    u64 mInt3 = 0;
    u32 mInt4 = 0;
    sead::Vector3f mFront_Maybe = {0.0, 0.0, 0.0};
    u32 mInt5 = 0;
    al::CameraTicket* mCameraTicket = nullptr;
    al::CameraArrowCollider* mCameraArrowCollider = nullptr;
    sead::Vector3f mCamPosition = {0.0, 0.0, 0.0};
    sead::Vector3f mCamRotation = {0.0, 0.0, 0.0};
    sead::Vector3f mShootLimit = {0.0, 0.0, 0.0};

    u32 mBulletCount = 0;

    bool mIsStanding = false;
    sead::Vector3f mSomeVec = {57.5, -25.0, 0.8};
    bool mIsUnkown = false;

    sead::Quatf* mPose = nullptr;
    f32* mFloat3 = nullptr;
    al::IntervalTrigger* mIntervalTrigger = nullptr;

    f32 mFloat4 = 0.0;
    bool mIsFalling = false;
    u32 mNotfloat5 = 0;
};
