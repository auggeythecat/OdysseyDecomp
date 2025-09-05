#include "Enemy/TankStateHack.h"

#include <basis/seadtypes.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "Library/Math/IntervalTrigger.h"

#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/NerveExecutor.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Play/Camera/CameraPoserSubjective.h"
#include "Library/Thread/FunctorV0M.h"

#include "Enemy/EnemyStateHackStart.h"
#include "Util/Hack.h"
#include "Util/JudgeUtil.h"
#include "Util/SensorMsgFunction.h"
#include "math/seadQuat.h"
#include "math/seadVectorFwd.h"

namespace {
NERVE_IMPL(TankStateHack, Wait)
NERVE_IMPL(TankStateHack, StartDemo)
NERVE_IMPL(TankStateHack, Fall)
NERVE_IMPL(TankStateHack, Move)
NERVE_IMPL(TankStateHack, Land)
NERVE_IMPL(TankStateHack, ShootReload)
NERVE_IMPL(TankStateHack, Shoot)

NERVES_MAKE_STRUCT(TankStateHack, Wait, StartDemo, Fall, Move, Land, ShootReload, Shoot)
}  // namespace

TankStateHack::TankStateHack(al::LiveActor* parent, const al::ActorInitInfo* initinfo, f32 float1,
                             f32 float2, sead::Vector3f* vec3, sead::Quatf* quat, f32 float3)
    : al::ActorStateBase::ActorStateBase("キャプチャステート", parent) {

    al::NerveExecutor::initNerve(this, &NrvTankStateHack.Wait);
    EnemyStateHackStart* HackStart = new EnemyStateHackStart(parent, nullptr, nullptr));
    mHackStart = HackStart;
    al::initNerveState(this, HackStart, &NrvTankStateHack.StartDemo,"キャプチャ開始");
    
    al::IntervalTrigger* trigger = new al::IntervalTrigger(15.0);
    bool var1 = false;
    if (mTankActor != nullptr) {
        // var1 = (IUseAudioKeeper *)(mTankActor + 0x10);
    }

}

void TankStateHack::appear() {}

void TankStateHack::kill() {}

void TankStateHack::control() {}

void TankStateHack::reset() {}

void TankStateHack::receiveMsgInitCapTargetInfo(const al::SensorMsg* message) {
    rs::tryReceiveMsgInitCapTargetAndSetCapTargetInfo(message, mCapTargetInfo);
}

void TankStateHack::receiveMsgNpcScareByEnemy(const al::SensorMsg* message) {
    rs::tryReceiveMsgNpcScareByEnemyIgnoreTargetHack(message, mCapTargetInfo);
}

bool TankStateHack::receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* self,
                                        al::HitSensor* other) {}

bool TankStateHack::receiveMsg(const al::SensorMsg* message, al::HitSensor* self,
                               al::HitSensor* other) {}

bool TankStateHack::receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* self,
                                      al::HitSensor* other) {}

void TankStateHack::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

void TankStateHack::updatePose() {}

void TankStateHack::updateVelocity(bool calcmove) {}

void TankStateHack::updateCamera() {}

bool TankStateHack::tryChangeNerveIfTrigerShoot() {}

void TankStateHack::forceEndIfHack() {}

void TankStateHack::calcAimCursorLayoutPos() {}

void TankStateHack::shoot() {}

void TankStateHack::endHack() {}

void TankStateHack::exeWait() {}

void TankStateHack::exeMove() {}

void TankStateHack::exeShoot() {}

void TankStateHack::exeShootReload() {}

void TankStateHack::exeFall() {}

void TankStateHack::exeLand() {}
