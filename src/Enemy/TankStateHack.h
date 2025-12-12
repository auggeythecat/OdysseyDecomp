#pragma once

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/NerveStateBase.h"
#include "math/seadVectorFwd.h"

namespace al {
class HitSensor;
class SensorMsg;
class NerveKeeper;
class ActorStateBase;
class IUseLayout;
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
    TankStateHack(Tank* parent, const al::ActorInitInfo& info, f32* mCannonRotator,
                  f32* mCannonScalor, sead::Vector3f* mFrontDir, sead::Quatf* mPose,
                  f32* mYRotator);
    void appear();
    void kill();
    void control();
    void reset();

    bool receiveMsgInitCapTargetInfo(const al::SensorMsg* message);
    bool receiveMsgNpcScareByEnemy(const al::SensorMsg* message);
    bool receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* other,
                             al::HitSensor* self);
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other);
    bool receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other);

    void endHack();

    void attackSensor(al::HitSensor* self, al::HitSensor* other);
    void updatePose();
    void updateVelocity(bool move);
    void updateCamera();
    bool tryChangeNerveIfTrigerShoot();
    bool forceEndIfHack();
    void calcAimCursorLayoutPos(sead::Vector2f* outvec) const;
    void shoot();

    void exeStartDemo();
    void exeWait();
    void exeMove();
    void exeShoot();
    void exeShootReload();
    void exeFall();
    void exeLand();

protected:
    friend class Tank;

private:
    Tank* mTankActor = nullptr;
    AimingCursor* mAimingCursor = nullptr;
    IUsePlayerHack* mPlayerHack = nullptr;
    CapTargetInfo* mCapTargetInfo = nullptr;
    EnemyStateHackStart* mHackStart = nullptr;
    HackerJudgeNormalFall* mHackerJudgeFall = nullptr;
    f32* mCannonRotator = nullptr;
    f32* mCannonScalor = nullptr;
    sead::Vector3f* mFrontDir = nullptr;;
    sead::Vector3f mVelocity = sead::Vector3f::zero; // <-- zero initialized. You can see the name from the asm in ghidra. Same for the rest of zero vectors
    sead::Vector3f mJointpos = sead::Vector3f::zero;
    sead::Vector3f mVector2 = sead::Vector3f::ey;
    // f32 mFloat1 = 0.0f; <-- This doesnt exist. Is padding to keep the aligment
    al::CameraTicket* mCameraTicket = nullptr;
    al::CameraArrowCollider* mCameraArrowCollider = nullptr;
    sead::Vector3f mVector3 = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mVector4 = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mVector5 = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mNormal = {0.0f, 0.0f, 0.0f};
    f32 mAngleDegree = 0.0f;
    f32 mRadianDegree = 0.0f;
    s32 something1 = 0;
    f32 sticksense = 0.0f;
    s32 something3 = 0;
    s32 something4 = 0;
    f32 mFloat1 = 0.0f;
    f32 mFloat2 = 0.0f;
    sead::Vector3f mVector6 = sead::Vector3f::zero;
    sead::Vector3f mVector7 = sead::Vector3f::zero;
    f32 fromexewait = 0.0f;
    f32 mWrongV1 = 0.0f;// <--- mistake
    f32 mWrongV2 = 0.0f;// <--- mistake
    bool mWrongBool = true;// <--- mistake
    f32 fromupdatepose = 0.0f;
    f32 mCamLimitx = 57.5f; // 0x11c
    f32 mCamLimity = -35.0f; // 0x120
    f32 mAimFollowRateV = 0.8f; // 0x124
    sead::Quatf* mPose = nullptr;
    f32* mYRotator = nullptr;
    al::IntervalTrigger* mIntervalTrigger = nullptr;
    f32 mMissing = 1.0f; // <--- mistake
    bool mIsFalling = false;
    s32 mAlphaMask = 0; // <--- Probably wrong name due shifted struct
};

static_assert(sizeof(TankStateHack) == 0x150);
