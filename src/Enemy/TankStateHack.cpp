#include "Enemy/TankStateHack.h"

#include <basis/seadTypes.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

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
}  // namespace

struct {
    NERVES_MAKE_STRUCT(TankStateHack, Wait, StartDemo, Fall, Move, Land, ShootReload, Shoot)

} TankStateHackData;

TankStateHack::TankStateHack(al::LiveActor* parent, const al::ActorInitInfo* initinfo, f32 float1,
                             f32 float2, sead::Vector3f* vec3, sead::Quatf* quat, f32 float3)
    : al::ActorStateBase::ActorStateBase("キャプチャステート", parent) {
    mTMPQuatName = *quat;  // TEMPERARY
    mParamFive = float1;   // FIXME: This is a temporary name, it is wrong.
    mParamSix = float2;    // FIXME: This is a temporary name, it is wrong.
}

void TankStateHack::appear() {
    return;
}

void TankStateHack::kill() {}

void TankStateHack::control() {

    const char* HackCapState = nullptr;
    
    if (!al::isNerve(this, &TankStateHackData.NrvTankStateHack.StartDemo) ||
        (mEnemyStateHackStart->isHackStart())) {
        if (rs::isHackCapSeparateFlying(mIUsePlayerHack))
            HackCapState = "HackOnCapOn";
        else
            HackCapState = "HackOnCapOff";
    } else {
        HackCapState = "HackOffCapOn";
    }
    al::tryStartVisAnimIfNotPlaying(mTankActor, HackCapState);
    return;
}

void TankStateHack::reset() {}

void TankStateHack::receiveMsgInitCapTargetInfo(const al::SensorMsg* message) {
    rs::tryReceiveMsgInitCapTargetAndSetCapTargetInfo(message, mCapTargetInfo);
}

void TankStateHack::receiveMsgNpcScareByEnemy(const al::SensorMsg* message) {
    rs::tryReceiveMsgNpcScareByEnemyIgnoreTargetHack(message, mCapTargetInfo);
}

bool TankStateHack::receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* self,
                                        al::HitSensor* other) {
    bool ismsgstarthack = rs::isMsgStartHack(message);                                            
    if (ismsgstarthack) {
        al::invalidateClipping(mTankActor);
        mEnemyStateHackStart->tryStart(message, other, self);
        al::setNerve(mIUseNerve, &TankStateHackData.NrvTankStateHack.StartDemo);
    }
    return ismsgstarthack;
}

bool TankStateHack::receiveMsg(const al::SensorMsg* message, al::HitSensor* self,
                               al::HitSensor* other) {
    if (rs::isMsgEnableMapCheckPointWarp(message)) {
        return rs::isMsgEnableMapCheckPointWarpCollidedGround(message, mTankActor);
    }
    if ((rs::isMsgHackerDamageAndCancel(message)) && (mIUsePlayerHack != nullptr)) {
        return rs::requestDamage(mIUsePlayerHack);
    }
    if (!rs::isMsgHackSyncDamageVisibility(message)) {
        if ((!rs::isMsgPushToPlayer(message)) || (!rs::tryReceiveMsgPushToPlayerAndAddVelocityH(mTankActor, message, self, other, 3.0))) {
            return rs::receiveMsgRequestTransferHack(message, mIUsePlayerHack, self);
        }
    } 
    else if (mIUsePlayerHack != nullptr) {
        rs::syncDamageVisibility(mTankActor, mIUsePlayerHack);
    }
    return true;
}


bool TankStateHack::receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* self,
                                      al::HitSensor* other) {
    bool isHackEnd;
    if (((rs::isMsgCancelHack(message)) && (!rs::isMsgHackMarioDead(message)) && (!rs::isMsgHackMarioDemo(message)))) {
        isHackEnd = false;
    } else {
        endHack();
        isHackEnd = true;
    }
    return isHackEnd;
}

void TankStateHack::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (!rs::sendMsgHackerNoReaction(mIUsePlayerHack, self, other) &&
        (!rs::sendMsgHackAttackMapObj(self, other)) &&
        ((al::isOnGround(mTankActor, 0) || (rs::sendMsgTankHackTrample(self, other)))) &&
        ((al::isSensorEnemyAttack(self)) || (rs::sendMsgTankKickHack(self, other))) &&
        (rs::sendMsgHackAttackKick(self, other))) {
        al::startHitReactionHitEffect(mTankActor, "キックヒット", self, other);
        return;
    }
    return;
}

void TankStateHack::updatePose() {}

void TankStateHack::updateVelocity(bool calcmove) {}

void TankStateHack::updateCamera() {}

bool TankStateHack::tryChangeNerveIfTrigerShoot() {

}

void TankStateHack::forceEndIfHack() {}

void TankStateHack::calcAimCursorLayoutPos() {}

void TankStateHack::shoot() {}

void TankStateHack::endHack() {}

void TankStateHack::exeWait() {
    al::Nerve* Nerve = nullptr;

    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Wait");
        rs::resetJudge(mIJudge);
    }
    updateVelocity(false);
    updateCamera();
    updatePose();
    if (!tryChangeNerveIfTrigerShoot()) {
        if (!rs::updateJudgeAndResult(mIJudge)) {
            if (rs::isHackerStopMove(mTankActor, mIUsePlayerHack, 6.0))
                return;
            Nerve = &TankStateHackData.NrvTankStateHack.Move;
        } else {
            Nerve = &TankStateHackData.NrvTankStateHack.Fall;
        }
        al::setNerve(this, Nerve);
        return;
    }
    mBulletCounter = 0;
    return;
}

void TankStateHack::exeStartDemo() {
    sead::Vector3f TmpVecName = {0.0, 0.0, 0.0};

    if (al::isFirstStep(mIUseNerve)) {
        al::onCollide(mTankActor);
        al::setVelocityZero(mTankActor);
        rs::resetJudge(mIJudge);
        /* TODO: Translate this.
         *(undefined4 *)(this + 0xdc) = 0;
         *(undefined4 *)(this + 0xe0) = 0;
         *(undefined8 *)(this + 0xd0) = 0;
         *(undefined4 *)(this + 0x10c) = 0;
         *(undefined4 *)(this + 0x110) = 0;
         */
        mIsDemo = true;
    }
    if (mEnemyStateHackStart->isHackStart()) {
        al::tryStartVisAnimIfNotPlaying(mTankActor, "HackOn");
        if (al::getNerveStep(mIUseNerve) > 16) {
            al::calcQuatUp(&TmpVecName, mTMPQuatName);
            f32 angledegree = al::calcAngleDegree(TmpVecName, sead::Vector3f::ey);
            f32 normalangle = (angledegree / -60.0) + 1.0;
            s32 getnervestepresult = al::getNerveStep(mIUseNerve);
            f32 maxanimationframe = al::getActionFrameMax(mTankActor, "HackStart");
            al::lerpValue(1.0, normalangle + normalangle + (1 - normalangle),
                          (f32)getnervestepresult - 16.0 / maxanimationframe - 15.0);
        }
    }
}

void TankStateHack::exeMove() {
    al::Nerve* mNerve = nullptr;
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Move");
        rs::resetJudge(mIJudge);
    }
    updateVelocity(true);
    updateCamera();
    updatePose();
    if (!tryChangeNerveIfTrigerShoot()) {
        if (!rs::updateJudgeAndResult(mIJudge)) {
            if (rs::isHackerStopMove(mTankActor, mIUsePlayerHack, 6.0))
                return;
            mNerve = &TankStateHackData.NrvTankStateHack.Move;
        } else {
            mNerve = &TankStateHackData.NrvTankStateHack.Fall;
        }
        al::setNerve(this, mNerve);
        return;
    }
    mBulletCounter = 0;
    return;
}

void TankStateHack::exeShoot() {
    al::Nerve* mNerve = nullptr;
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Shoot");
        al::startHitReaction(mTankActor, "発射");
        mBulletCounter++;
    }
    updateVelocity(al::isGreaterEqualStep(this, 16));
    updateCamera();
    updatePose();
    if ((al::isGreaterEqualStep(this, 17)) &&
        (((mBulletCounter < 3 /* || (Tank::countAliveBullet(mTankActor) < 2) */) &&
          (tryChangeNerveIfTrigerShoot())))) {
        return;
    }
    if ((rs::updateJudgeAndResult(mIJudge)) && (!mIsFalling)) {
        al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
        mNerve = &TankStateHackData.NrvTankStateHack.Land;
    } else {
        if (al::isActionEnd(mTankActor))
            return;
        if (mBulletCounter < 3) {
            mBulletCounter = 0;
            if (tryChangeNerveIfTrigerShoot())
                return;
            if (rs::isHackerStopMove(mTankActor, mIUsePlayerHack, 6.0))
                mNerve = &TankStateHackData.NrvTankStateHack.Wait;
            else if (rs::updateJudgeAndResult(mIJudge))
                mNerve = &TankStateHackData.NrvTankStateHack.Move;
            else
                mNerve = &TankStateHackData.NrvTankStateHack.Fall;
        } else {
            mNerve = &TankStateHackData.NrvTankStateHack.ShootReload;
        }
    }
    al::setNerve(this, mNerve);
    return;
}

void TankStateHack::exeShootReload() {
    al::Nerve* mNerve = nullptr;
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Reload");
        mBulletCounter = 0;
    }
    updateVelocity(false);
    updateCamera();
    updatePose();
    if ((rs::updateJudgeAndResult(mIJudge)) && (!mIsFalling)) {
        al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
        mNerve = &TankStateHackData.NrvTankStateHack.Land;
    } else {
        if (al::isActionEnd(mTankActor))
            return;
        if (rs::isHackerStopMove(mTankActor, mIUsePlayerHack, 6.0))
            mNerve = &TankStateHackData.NrvTankStateHack.Wait;
        else if (rs::updateJudgeAndResult(mIJudge))
            mNerve = &TankStateHackData.NrvTankStateHack.Move;
        else
            mNerve = &TankStateHackData.NrvTankStateHack.Fall;
    }
    al::setNerve(this, mNerve);
    return;
}

void TankStateHack::exeFall() {
    if (al::isFirstStep(this))
        al::startAction(mTankActor, "Wait");
    updateVelocity(false);
    updateCamera();
    updatePose();

    if (!tryChangeNerveIfTrigerShoot()) {
        if (al::isOnGround(mTankActor, 0)) {
            al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
            al::setNerve(this, &TankStateHackData.NrvTankStateHack.Land);
            return;
        }
    } else {
        mIsFalling = true;
        mBulletCounter = 0;
    }
    return;
}

void TankStateHack::exeLand() {
    if (al::isFirstStep(this)) {
        mIsFalling = false;
        al::startAction(mTankActor, "Land");
    }
    updateVelocity(false);
    updateCamera();
    updatePose();

    if (al::isActionEnd(mTankActor)) {
        al::setNerve(this, &TankStateHackData.NrvTankStateHack.Wait);
        return;
    }
    return;
}
