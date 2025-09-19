#include "Enemy/TankStateHack.h"

#include "Library/Audio/IUseAudioKeeper.h"
#include "Library/Camera/CameraArrowCollider.h"
#include "Library/Camera/CameraPoser.h"
#include "Library/Camera/CameraPoserFunction.h"
#include "Library/Camera/CameraTicket.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Camera/IUseCamera.h"
#include "Library/Collision/CollisionDirector.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Math/IntervalTrigger.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Se/Function/SeFunction.h"
#include "Library/Se/SeFunction.h"
#include "Library/Thread/FunctorV0M.h"

#include "Enemy/EnemyStateHackStart.h"
#include "Enemy/Tank.h"
#include "Layout/AimingCursor.h"
#include "Player/HackerJudge.h"
#include "Player/IUsePlayerHack.h"
#include "Util/Hack.h"
#include "Util/JudgeUtil.h"
#include "Util/SensorMsgFunction.h"

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

TankStateHack::TankStateHack(Tank* parent, const al::ActorInitInfo& initinfo, f32* float1,
                             f32* float2, sead::Vector3f* vec3, sead::Quatf* quat, f32* float3)
    : al::ActorStateBase::ActorStateBase("キャプチャステート", parent) {
    mVec3 = vec3;
    mTankActor = parent;
    mFloat1 = float1;
    mFloat2 = float2;
    mPose = quat;
    mFloat3 = float3;

    al::NerveExecutor::initNerve(&NrvTankStateHack.Wait, 1);

    EnemyStateHackStart* hackStart = new EnemyStateHackStart(parent, nullptr, nullptr);
    mHackStart = hackStart;
    al::initNerveState(this, hackStart, &NrvTankStateHack.StartDemo, "キャプチャ開始");

    al::IntervalTrigger* intervalTrigger = new al::IntervalTrigger(15.0);
    mIntervalTrigger = intervalTrigger;

    al::IUseCamera* iUseCamera;
    if (parent != nullptr)
        iUseCamera = (al::IUseCamera*)parent;
    al::CameraTicket* cameraTicket = al::initProgramableCamera(
        iUseCamera, initinfo, nullptr, &mCamPosition, &mCamRotation, (nullptr));
    mCameraTicket = cameraTicket;

    alCameraFunction::initPriorityCapture(cameraTicket);
    alCameraPoserFunction::initGyroCameraCtrl((al::CameraPoser*)mCameraTicket);
    alCameraPoserFunction::initSnapShotCameraCtrl((al::CameraPoser*)mCameraTicket);
    alCameraPoserFunction::validateSnapShotCameraZoomFovy((al::CameraPoser*)mCameraTicket);
    alCameraPoserFunction::validateSnapShotCameraRoll((al::CameraPoser*)mCameraTicket);
    al::CameraArrowCollider* cameraArrowCollider =
        new al::CameraArrowCollider((al::CollisionDirector*)parent);
    mCameraArrowCollider = cameraArrowCollider;

    mCapTargetInfo = rs::createCapTargetInfo(parent, nullptr);

    HackerJudgeNormalFall* hackerJudgeFall = new HackerJudgeNormalFall(parent, 5);
    mHackerJudgeFall = hackerJudgeFall;

    al::LayoutInitInfo layoutInfo = al::getLayoutInitInfo(initinfo);
    AimingCursor* aimingCursor = new AimingCursor("タンク照準レイアウト", layoutInfo);
    mAimingCursor = aimingCursor;
}

void TankStateHack::appear() {
    al::IUseAudioKeeper* iUseAudioKeeper;
    mIsDead = false;
    al::invalidateDitherAnim(mTankActor);
    mNotfloat5 = 0;
    al::setCameraFovyDegree(mCameraTicket, 50.0);
    al::setNerve(this, &NrvTankStateHack.StartDemo);
    iUseAudioKeeper = nullptr;
    if (mTankActor != nullptr)
        iUseAudioKeeper = (al::IUseAudioKeeper*)mTankActor;
    al::setSeKeeperPlayNamePrefix(iUseAudioKeeper, "PHack");
    iUseAudioKeeper = nullptr;
    if (mLiveActor != nullptr)
        iUseAudioKeeper = (al::IUseAudioKeeper*)mLiveActor;
    alSeFunction::startListenerPoser(iUseAudioKeeper, "カメラ位置", 30);
    iUseAudioKeeper = nullptr;
    if (mTankActor != nullptr)
        iUseAudioKeeper = (al::IUseAudioKeeper*)mTankActor;
    alSeFunction::startSituation(iUseAudioKeeper, "乗り物の中", -1);
}

void TankStateHack::kill() {
    al::setModelAlphaMask(mTankActor, 1.0);
    mFloat4 = 0.0;
    mNotfloat5 = 0;
    al::validateDitherAnim(mTankActor);
    mAimingCursor->end();
    mFloat1 = 0;
    al::showModelIfHide(mTankActor);
    al::setNerve(this, &NrvTankStateHack.StartDemo);
    if (al::isActiveCamera(mCameraTicket)) {
        al::IUseCamera* iUseCamera = nullptr;
        if (mTankActor != nullptr)
            iUseCamera = (al::IUseCamera*)mTankActor;
        al::endCamera(iUseCamera, mCameraTicket, -1, false);
    }

    al::IUseAudioKeeper* iUseAudioKeeper;
    iUseAudioKeeper = nullptr;
    if (mTankActor != nullptr)
        iUseAudioKeeper = (al::IUseAudioKeeper*)mTankActor;
    alSeFunction::endListenerPoser(iUseAudioKeeper, "カメラ位置", 0);
    iUseAudioKeeper = nullptr;
    if (mTankActor != nullptr)
        iUseAudioKeeper = (al::IUseAudioKeeper*)mTankActor;
    alSeFunction::endSituation(iUseAudioKeeper, "乗り物の中", -1);
    iUseAudioKeeper = nullptr;
    if (mTankActor != nullptr)
        iUseAudioKeeper = (al::IUseAudioKeeper*)mTankActor;
    al::resetSeKeeperPlayNamePrefix(iUseAudioKeeper);
    // mIsDead = true;
}

void TankStateHack::control() {
    const char* hackcap;
    if (al::isNerve(this, &NrvTankStateHack.StartDemo) || mHackStart->isHackStart())
        hackcap = rs::isHackCapSeparateFlying(mIUsePlayerHack) ? "HackOnCapOn" : "HackOnCapOff";
    else
        hackcap = "HackOffCapOn";
    al::tryStartVisAnimIfNotPlaying(mTankActor, hackcap);
}

void TankStateHack::reset() {
    mInt1 = 0;                       // 0x68
    mInt2 = 0;                       // 0x70
    mInt3 = 0;                       // 0x74
    mInt4 = 0;                       // 0x7c
    mFront_Maybe = {0.0, 0.0, 0.0};  // 0x80
    mInt5 = 0;                       // 0x88
}

void TankStateHack::receiveMsgInitCapTargetInfo(const al::SensorMsg* message) {
    rs::tryReceiveMsgInitCapTargetAndSetCapTargetInfo(message, mCapTargetInfo);
}

void TankStateHack::receiveMsgNpcScareByEnemy(const al::SensorMsg* message) {
    rs::tryReceiveMsgNpcScareByEnemyIgnoreTargetHack(message, mCapTargetInfo);
}

bool TankStateHack::receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* other,
                                        al::HitSensor* self) {
    bool startHack = rs::isMsgStartHack(message);
    if (startHack) {
        al::invalidateClipping(mTankActor);
        IUsePlayerHack* playerHack = mHackStart->tryStart(message, other, self);
        mIUsePlayerHack = playerHack;
        al::setNerve(this, &NrvTankStateHack.StartDemo);
    }
    return startHack;
}

bool TankStateHack::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                               al::HitSensor* self) {
    if (rs::isMsgEnableMapCheckPointWarp(message))
        return rs::isMsgEnableMapCheckPointWarpCollidedGround(message, mTankActor);
    if (rs::isMsgHackerDamageAndCancel(message) && mIUsePlayerHack != nullptr)
        return rs::requestDamage(mIUsePlayerHack);
    if (!rs::isMsgHackSyncDamageVisibility(message)) {
        if (!rs::isMsgPushToPlayer(message) ||
            !rs::tryReceiveMsgPushToPlayerAndAddVelocity(mTankActor, message, self, other, 3.0)) {
            return rs::receiveMsgRequestTransferHack(message, mIUsePlayerHack, other);
        }
    } else if (mIUsePlayerHack != nullptr) {
        rs::syncDamageVisibility(mTankActor, mIUsePlayerHack);
    }

    return true;
}

bool TankStateHack::receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* other,
                                      al::HitSensor* self) {
    bool isendhack;
    if (!rs::isMsgCancelHack(message) && !rs::isMsgHackMarioDead(message) &&
        !rs::isMsgHackMarioDemo(message)) {
        isendhack = false;
    } else {
        endHack();
        isendhack = true;
    }
    return isendhack;
}

void TankStateHack::endHack() {
    al::validateClipping(mLiveActor);
    al::startVisAnim(mLiveActor, "HackOff");
    sead::Vector3f gotTrans = al::getTrans(mTankActor);
    sead::Vector3f gottranscopy = gotTrans;
}

void TankStateHack::attackSensor(al::HitSensor* other, al::HitSensor* self) {
    if ((!rs::sendMsgHackerNoReaction(mIUsePlayerHack, self, other)) &&
        (!rs::sendMsgHackAttackMapObj(self, other))) {
        al::startHitReactionHitEffect(mTankActor, "キックヒット", other, self);
    }
}

void TankStateHack::updatePose() {
    sead::Quatf qStack1;
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, mTankActor);
    if (al::isOnGround(mTankActor, 0)) {
        if (al::isNerve(this, &NrvTankStateHack.Fall)) {
            al::makeQuatUpFront(&qStack1, sead::Vector3f::ey, frontDir);
            sead::QuatCalcCommon<f32>::slerpTo(*mPose, *mPose, qStack1, 0.15);
            mIsUnkown = false;
        }
    } else {
        al::makeQuatUpFront(&qStack1, mFront_Maybe, frontDir);
        sead::QuatCalcCommon<f32>::slerpTo(*mPose, *mPose, qStack1, 0.15);
        mIsUnkown = al::calcAngleDegree(mFront_Maybe, sead::Vector3f::ey);
    }
    if (!al::isNerve(this, &NrvTankStateHack.StartDemo)) {
        // al::calcQuatUp(qStack1, mPose);
        // TODO: How to convert Quat to Vector3
    }
}

void TankStateHack::updateVelocity(bool calcmove) {}

void TankStateHack::updateCamera() {}

bool TankStateHack::tryChangeNerveIfTrigerShoot() {
    bool triggerhack = rs::isTriggerHackAnyButton(mIUsePlayerHack);
    if (triggerhack) {
        alCameraPoserFunction::reduceGyroSencitivity((al::CameraPoser*)mCameraTicket);
        sead::Vector3f Jointpos = {0.0, 0.0, 0.0};
        al::calcJointPos(&Jointpos, mTankActor, "Shoot");
        sead::Vector3f shootPos = mShootLimit - Jointpos;
        al::normalize(&shootPos);
        // mTankActor->shootByPlayer(&shootPos, 75.0, 90);
        al::setNerve(this, &NrvTankStateHack.ShootReload);
    }

    return triggerhack;
}

bool TankStateHack::forceEndIfHack() {}

void TankStateHack::calcAimCursorLayoutPos() {}

void TankStateHack::shoot() {}

void TankStateHack::exeWait() {
    al::Nerve* nerve;
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Wait");
        rs::resetJudge(mHackerJudgeFall);
    }
    updateVelocity(false);
    updateCamera();
    updatePose();
    if (!tryChangeNerveIfTrigerShoot()) {
        if (!rs::updateJudgeAndResult(mHackerJudgeFall)) {
            if (rs::isHackerStopMove(mTankActor, mIUsePlayerHack, 6.0))
                return;
            nerve = &NrvTankStateHack.Move;
        } else {
            nerve = &NrvTankStateHack.Fall;
        }
        al::setNerve(this, nerve);
        return;
    }
    mBulletCount = 0;
}

void TankStateHack::exeMove() {
    al::Nerve* nerve;
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Move");
        rs::resetJudge(mHackerJudgeFall);
    }
    updateVelocity(false);
    updateCamera();
    updatePose();

    if (!tryChangeNerveIfTrigerShoot()) {
        if (!rs::updateJudgeAndResult(mHackerJudgeFall)) {
            if (!rs::isHackerStopMove(mTankActor, mIUsePlayerHack, 6.0))
                return;
            nerve = &NrvTankStateHack.Wait;
        } else {
            nerve = &NrvTankStateHack.Fall;
        }
        al::setNerve(this, nerve);
        return;
    }
    mBulletCount = 0;
}

void TankStateHack::exeShoot() {
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Shoot");
        al::startHitReaction(mTankActor, "発射");
        al::setVelocityZero(mTankActor);
        ++mBulletCount;
    }
    al::isGreaterEqualStep(this, 15);
    updateVelocity(false);
    updateCamera();
    updatePose();
    if (al::isGreaterEqualStep(this, 17) &&
        (((mBulletCount < 3 || (mTankActor->countAliveBullet() < 2)) &&
          tryChangeNerveIfTrigerShoot()))) {
        return;
    }
}

void TankStateHack::exeShootReload() {}

void TankStateHack::exeFall() {
    if (al::isFirstStep(this))
        al::startAction(mLiveActor, "Wait");
    updateVelocity(false);
    updateCamera();
    updatePose();
    if (tryChangeNerveIfTrigerShoot()) {
        mIsFalling = true;
        mIsStanding = false;
    } else if (al::isOnGround(mLiveActor, 0)) {
        al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
        al::setNerve(this, &NrvTankStateHack.Land);
    }
}

void TankStateHack::exeLand() {
    if (al::isFirstStep(this)) {
        mIsFalling = false;
        al::startAction(mTankActor, "Land");
    }
    updateVelocity(false);
    updateCamera();
    updatePose();
    if (al::isActionEnd(mTankActor))
        al::setNerve(this, &NrvTankStateHack.Wait);
}
