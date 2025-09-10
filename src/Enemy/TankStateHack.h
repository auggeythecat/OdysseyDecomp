#pragma once

// #include <basis/seadtypes.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/NerveStateBase.h"
#include "math/seadVectorFwd.h"

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
    void forceEndIfHack();
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
    /*

*(undefined ***)this = &PTR_getNerveKeeper_7101ca9d78;

memset(this + 0x90,0,0x60);
*(undefined4 *)(this + 0xf8) = 0;
*(undefined8 *)(this + 0xf0) = 0;
*(undefined4 *)(this + 0x104) = 0;
this[0x114] = (TankStateHack)0x1;
*(undefined8 *)(this + 0x108) = 0;
*(undefined4 *)(this + 0x110) = 0;
*(undefined4 *)(this + 0x118) = 0;
*(undefined8 *)(this + 0x138) = 0;
*(undefined8 *)(this + 0xfc) = 0;

*(undefined4 *)(this + 0x140) = 0x3f800000;
*(undefined4 *)(this + 0x148) = 0;
*/
    // bool mIsDead = false; // 0x10
    al::LiveActor* mLiveActor = nullptr; // 0x18
    Tank* mTankActor = nullptr;                               // 0x20
    AimingCursor* mAimingCursor = nullptr;                    // 0x28
    IUsePlayerHack* mIUsePlayerHack = nullptr;                // 0x30
    const CapTargetInfo* mCapTargetInfo = nullptr;            // 0x38
    EnemyStateHackStart* mHackStart = nullptr;                // 0x40
    HackerJudgeNormalFall* mHackerJudgeFall = nullptr;        // 0x48
    f32* mfloat1 = nullptr;                                   // 0x50
    f32* mfloat2 = nullptr;                                   // 0x58
    sead::Vector3f* mvec3 = nullptr;                          // 0x60
    u64 mInt1 = 0;                                            // 0x68
    u32 mInt2 = 0;                                            // 0x70
    u64 mInt3 = 0;                                            // 0x74
    u32 mInt4 = 0;                                            // 0x7c
    sead::Vector3f mFront_Maybe = {0.0, 0.0, 0.0};                                        // 0x80
    u32 mInt5 = 0;                                            // 0x88
    al::CameraTicket* mCameraTicket = nullptr;                // 0x90
    al::CameraArrowCollider* mCameraArrowCollider = nullptr;  // 0x98
    sead::Vector3f mCamPosition = {0.0, 0.0, 0.0};            // 0xa0
    sead::Vector3f mCamRotation = {0.0, 0.0, 0.0};            // 0xb8

    sead::Vector3f mShootLimit = {0.0, 0.0, 0.0}; // 0xc4



    u32 mBulletCount = 0; // 0x108 Something with shooting/counting shot


    sead::Vector3f mSomeVec = {57.5, -25.0, 0.8};  // 0x11c
    bool misUnkown = false; // 0x118
    void* mUnknown3[0x154 / 8];

    sead::Quatf* mPose = nullptr;                     // 0x128
    f32* mfloat3 = nullptr;                           // 0x130
    al::IntervalTrigger* mIntervalTrigger = nullptr;  // 0x138

    void* mUnknown4[0x16 / 8];
    f32 mfloat4 = 0.0;        // 0x140
    bool mIsFalling = false;  // 0x144
    u32 mNotfloat5 = 0;       // 0x148 (Used in appear, and kill UKNOWN NAME)
};
