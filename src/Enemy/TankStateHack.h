#pragma once

#include <basis/seadtypes.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/Nerve/IUseNerve.h"
#include "Library/Nerve/NerveStateBase.h"
#include "Library/Play/Camera/CameraPoserSubjective.h"
#include "Player/CapTargetInfo.h"
#include "basis/seadTypes.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
class NerveKeeper;
}  // namespace al

class IJudge;
class IUsePlayerHack;
class EnemyStateHackStart;

class TankStateHack : public al::ActorStateBase {
public:
    TankStateHack(al::LiveActor* parent, const al::ActorInitInfo* initinfo, f32 float1, f32 float2,
                  sead::Vector3f* vec3, sead::Quatf* quat, f32 float3);
    void appear();
    void kill();
    void control();
    void reset();

    void receiveMsgInitCapTargetInfo(const al::SensorMsg* message);
    void receiveMsgNpcScareByEnemy(const al::SensorMsg* message);
    bool receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* self,
                             al::HitSensor* other);
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other);
    bool receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other);

    void attackSensor(al::HitSensor* self, al::HitSensor* other);
    void updatePose();
    void updateVelocity(bool calcmove);
    void updateCamera();
    bool tryChangeNerveIfTrigerShoot();
    void forceEndIfHack();
    void calcAimCursorLayoutPos();

    void shoot();
    void endHack();

    void exeStartDemo();
    void exeWait();
    void exeMove();
    void exeShoot();
    void exeShootReload();
    void exeFall();
    void exeLand();

private:
    al::IntervalTrigger* mIntervalTrigger = nullptr;
    al::LiveActor* mTankActor = nullptr;
    IUsePlayerHack* mIUsePlayerHack = nullptr;
    al::IUseNerve* mIUseNerve = nullptr;
    EnemyStateHackStart* mEnemyStateHackStart = nullptr;
    IJudge* mIJudge = nullptr;
    CapTargetInfo* mCapTargetInfo = nullptr;

    f32 mParamFive = 0.0f;  // FIXME: This is a temporary name, it is wrong.
    f32 mParamSix = 0.0f;   // FIXME: This is a temporary name, it is wrong.

    s32 mBulletCounter = 0;   // FIXME: Currently my best guess for the name/function of this
                              // variable. It is almost certainly wrong.
    bool mIsFalling = false;  // FIXME: Currently my best guess for the name/function of this
                              // variable. It is almost certainly wrong.
    bool mIsDemo = true;  // FIXME: Currently my best guess for the name/function of this variable.
                          // It is almost certainly wrong.
    sead::Quatf mTMPQuatName = {0.0f, 0.0f, 0.0f,
                                0.0f};  // FIXME: This is a temporary name, it is wrong.
};
