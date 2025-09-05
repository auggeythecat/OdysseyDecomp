#pragma once

#include <basis/seadtypes.h>
#include <math/seadVector.h>
#include <math/seadQuat.h>

#include "Library/Camera/IUseCamera.h"
#include "Library/Nerve/NerveStateBase.h"
#include "Library/Play/Camera/CameraPoserSubjective.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
class NerveKeeper;
class ActorStateBase;
class Tank;
class IUseCamera;
class IntervalTrigger;
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


    al::Tank* mTankActor = nullptr; // 0x20
    // *(undefined8 *)(this + 0x28) = 0;
    al::IUseCamera* mIUseCamera = nullptr; // 0x30
    // *(undefined8 *)(this + 0x38) = 0;
    EnemyStateHackStart* mHackStart = nullptr; // 0x40
    // *(undefined8 *)(this + 0x48) = 0;
    f32 mfloat1 = 0.0; // 0x50
    f32 mfloat2 = 0.0; // 0x58

    // *(undefined8 *)(this + 0x68) = 0;
    // *(undefined4 *)(this + 0x70) = 0;
    // *(undefined8 *)(this + 0x74) = 0;
    // *(undefined4 *)(this + 0x7c) = 0;
    // *(undefined8 *)(this + 0x80) = 0x3f80000000000000;
    // *(undefined4 *)(this + 0x88) = 0;

    
    // f32 mfloat3 = 0x42660000; // 0x11c
    // f32 mfloat4 = 0xc20c0000; // 0x120
    // f32 mfloat5 = 0x3f4ccccd; // 0x124

    sead::Quatf* mQuat = nullptr; // 0x128
    f32 mfloat6 = 0.0; // 0x130
    al::IntervalTrigger* mIntervalTrigger = nullptr; //0x138


    f32 mfloat7 = 0.0;
    bool mIsFalling = false;  // 0x144
};
