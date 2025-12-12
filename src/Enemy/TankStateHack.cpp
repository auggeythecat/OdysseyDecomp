#include "Enemy/TankStateHack.h"

#include "Library/Audio/IUseAudioKeeper.h"
#include "Library/Camera/CameraArrowCollider.h"
#include "Library/Camera/CameraPoser.h"
#include "Library/Camera/CameraPoserFunction.h"
#include "Library/Camera/CameraTicket.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Camera/IUseCamera.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/IUseCollision.h"
#include "Library/Layout/IUseLayout.h"
#include "Library/Layout/LayoutActorUtil.h"
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
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Se/Function/SeFunction.h"
#include "Library/Se/SeFunction.h"

#include "Enemy/EnemyStateHackStart.h"
#include "Enemy/Tank.h"
#include "Enemy/TankBullet.h"
#include "Layout/AimingCursor.h"
#include "Player/HackerJudge.h"
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

TankStateHack::TankStateHack(Tank* parent, const al::ActorInitInfo& info, f32* CannonRotator,
                             f32* CannonScalor, sead::Vector3f* FrontDir, sead::Quatf* Pose,
                             f32* YRotator)
    : al::ActorStateBase::ActorStateBase("キャプチャステート", parent), mTankActor(parent), mCannonRotator(CannonRotator),
        mCannonScalor(CannonScalor), mFrontDir(FrontDir), mPose(Pose),mYRotator(YRotator) {
    al::NerveExecutor::initNerve(&NrvTankStateHack.Wait, 1);

    mHackStart = new EnemyStateHackStart(parent, nullptr, nullptr);
    al::initNerveState(this, mHackStart, &NrvTankStateHack.StartDemo, "キャプチャ開始");

    mIntervalTrigger = new al::IntervalTrigger(15.0f);

    al::setSeUserSyncParamPtr(mTankActor, &fromupdatepose, "斜面傾き");
    mCameraTicket = al::initProgramableCamera(parent, info, nullptr, &mVector3, &mVector5, nullptr);

    alCameraFunction::initPriorityCapture(mCameraTicket);
    alCameraPoserFunction::initGyroCameraCtrl(mCameraTicket->getPoser());
    alCameraPoserFunction::initSnapShotCameraCtrl(mCameraTicket->getPoser());
    alCameraPoserFunction::validateSnapShotCameraZoomFovy(mCameraTicket->getPoser());
    alCameraPoserFunction::validateSnapShotCameraRoll(mCameraTicket->getPoser());

    mCameraArrowCollider = new al::CameraArrowCollider(parent->getCollisionDirector()); // <-- see CameraArrowCollider header in context
    mCapTargetInfo = rs::createCapTargetInfo(parent, nullptr);
    mHackerJudgeFall = new HackerJudgeNormalFall(parent, 5);
    mAimingCursor = new AimingCursor("タンク照準レイアウト", al::getLayoutInitInfo(info));
}

void TankStateHack::appear() {
    al::IUseAudioKeeper* keeper;
    NerveStateBase::appear();

    al::invalidateDitherAnim(mTankActor);
    // mFloatNAN = 0;

    al::setCameraFovyDegree(mCameraTicket, 50.0);
    al::setNerve(this, &NrvTankStateHack.StartDemo);

    keeper = nullptr;
    if (mActor)
        keeper = mActor;

    al::setSeKeeperPlayNamePrefix(keeper, "PHack");
    keeper = nullptr;
    if (mActor)
        keeper = mActor;
    alSeFunction::startListenerPoser(keeper, "カメラ位置", 30);
    keeper = nullptr;
    if (mActor)
        keeper = mActor;
    alSeFunction::startSituation(keeper, "乗り物の中", -1);
}

void TankStateHack::kill() {
    al::setModelAlphaMask(mTankActor, 1.0);
    mAlphaMask = 1.0f;
    // mFloatNAN = 0.0f;
    al::validateDitherAnim(mTankActor);
    mAimingCursor->end();
    *mCannonRotator = 0.0;
    al::showModelIfHide(mTankActor);
    al::setNerve(this, &NrvTankStateHack.StartDemo);

    al::IUseCamera* camDirector;
    al::IUseAudioKeeper* keeper;

    if (al::isActiveCamera(mCameraTicket)) {
        camDirector = nullptr;

        if (mTankActor)
            camDirector = mTankActor;
        al::endCamera(camDirector, mCameraTicket, -1, false);
    }
    keeper = nullptr;
    if (mActor)
        keeper = mActor;
    alSeFunction::endListenerPoser(keeper, "カメラ位置", 0);
    keeper = nullptr;
    if (mActor)
        keeper = mActor;
    alSeFunction::endSituation(keeper, "乗り物の中", -1);
    keeper = nullptr;
    if (mActor)
        keeper = mActor;
    al::resetSeKeeperPlayNamePrefix(keeper);
    NerveStateBase::kill();
}

void TankStateHack::control() {
    if (al::isNerve(this, &NrvTankStateHack.StartDemo) && !mHackStart->isHackStart())
        al::tryStartVisAnimIfNotPlaying(mTankActor, "HackOffCapOn");
    else if (rs::isHackCapSeparateFlying(mPlayerHack))
        al::tryStartVisAnimIfNotPlaying(mTankActor, "HackOnCapOff");
    else
        al::tryStartVisAnimIfNotPlaying(mTankActor, "HackOnCapOn");
}

void TankStateHack::reset() {
    mNormal.set(sead::Vector3f::ey);
    mVector2.set(sead::Vector3f::zero);
    mVelocity.set(sead::Vector3f::zero);
}

bool TankStateHack::receiveMsgInitCapTargetInfo(const al::SensorMsg* message) {
    return rs::tryReceiveMsgInitCapTargetAndSetCapTargetInfo(message, mCapTargetInfo);
}

bool TankStateHack::receiveMsgNpcScareByEnemy(const al::SensorMsg* message) {
    return rs::tryReceiveMsgNpcScareByEnemyIgnoreTargetHack(message, mCapTargetInfo);
}

bool TankStateHack::receiveMsgHackStart(const al::SensorMsg* message, al::HitSensor* other,
                                        al::HitSensor* self) {
    if (rs::isMsgStartHack(message)) {
        al::invalidateClipping(mActor);
        mPlayerHack = mHackStart->tryStart(message, other, self);
        al::setNerve(this, &NrvTankStateHack.StartDemo);
        return true;
    }
    return false;
}

bool TankStateHack::receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                               al::HitSensor* self) {
    if (rs::isMsgEnableMapCheckPointWarp(message))
        return rs::isMsgEnableMapCheckPointWarpCollidedGround(message, mTankActor);

    if (rs::isMsgHackerDamageAndCancel(message) && mPlayerHack)
        return rs::requestDamage(mPlayerHack);

    if (rs::isMsgHackSyncDamageVisibility(message)) {
        if (mPlayerHack)
            rs::syncDamageVisibility(mTankActor, mPlayerHack);
    } else if ((!rs::isMsgPushToPlayer(message)) || !rs::tryReceiveMsgPushToPlayerAndAddVelocityH(
                                                        mTankActor, message, other, self, 3.0)) {
        return rs::receiveMsgRequestTransferHack(message, mPlayerHack, other);
    }
    return true;
}

bool TankStateHack::receiveMsgHackEnd(const al::SensorMsg* message, al::HitSensor* other,
                                      al::HitSensor* self) {
    if ((rs::isMsgCancelHack(message) || rs::isMsgHackMarioDead(message)) ||
        rs::isMsgHackMarioDemo(message)) {
        endHack();
        return true;
    }
    return false;
}

void TankStateHack::endHack() {
    sead::Vector3f quatUp;

    al::validateClipping(mActor);
    al::startVisAnim(mActor, "HackOff");
    sead::Vector3f getTrans = al::getTrans(mTankActor);
    sead::Vector3f trans = getTrans;

    al::calcQuatUp(&quatUp, *mPose);
    sead::Vector3f quatUpMult = quatUp * 250.0f;
    sead::Vector3f dir = {0.0, 0.0, 0.0};

    sead::Vector3f pos = trans + quatUp * 50;
    al::IUseCollision* collision = nullptr;

    if (mTankActor)
        collision = mTankActor;

    bool hitpos =
        alCollisionUtil::getHitPosOnArrow(collision, &dir, pos, quatUpMult, nullptr, nullptr);
    if (!hitpos) {
        rs::endHack(&mPlayerHack);
    } else {
        pos = (trans + dir) * 0.5;
        rs::endHackFromTargetPos(&mPlayerHack, pos);
    }
    kill();
}

void TankStateHack::attackSensor(al::HitSensor* other, al::HitSensor* self) {
    if ((((!rs::sendMsgHackerNoReaction(mPlayerHack, self, other)) &&
          !rs::sendMsgHackAttackMapObj(self, other)) &&
         ((al::isOnGround(mTankActor, 0) || !rs::sendMsgTankHackTrample(self, other)))) &&
        (((!al::isSensorEnemyAttack(other) || !rs::sendMsgTankKickHack(self, other)) &&
          rs::sendMsgHackAttackKick(self, other)))) {
        al::startHitReactionHitEffect(mActor, "キックヒット", other, self);
    }
    return;
}

void TankStateHack::updatePose() {
    sead::Vector3f frontDir;
    sead::Quatf front;

    al::calcFrontDir(&frontDir, mTankActor);

    if (al::isOnGround(mTankActor, 0)) {
        al::makeQuatUpFront(&front, mNormal, frontDir);
        sead::QuatCalcCommon<f32>::slerpTo(*mPose, *mPose, front, 0.15);
        // mFloat7 = al::calcAngleDegree(mNormal, sead::Vector3f::ey);
    } else if (al::isNerve(this, &NrvTankStateHack.Fall)) {
        al::makeQuatUpFront(&front, sead::Vector3f::ey, frontDir);
        sead::QuatCalcCommon<f32>::slerpTo(*mPose, *mPose, front, 0.15);
        // mFloat7 = 0.0f;
    }

    if (!al::isNerve(this, &NrvTankStateHack.StartDemo)) {
        al::calcQuatUp(&frontDir, *mPose);
        f32 angleDegree = al::calcAngleDegree(frontDir, sead::Vector3f::ey);
        angleDegree = angleDegree / -60.0 + 1;
        *mCannonScalor = angleDegree + angleDegree + (1.0 - angleDegree);
    }
}

void TankStateHack::updateVelocity(bool move) {
    // bool isGrounded = al::isOnGround(mTankActor, 0);

    sead::Vector3f groundNormal = sead::Vector3f::ey;
    sead::Vector3f moveDir = {0.0, 0.0, 0.0};

    if (al::isOnGround(mTankActor, 0))
        groundNormal = al::getCollidedGroundNormal(mTankActor);

    al::lerpVec(&groundNormal, mNormal, groundNormal, 0.2f);
    al::normalize(&groundNormal);
    mNormal = groundNormal;

    // f32 speedFactor;

    if (al::isOnGround(mTankActor, 0)) {
        mVector2 *= 0.8f;
        mVector2 = groundNormal * -(mVector2.length() + 0.7f);
    } else {
        mVector2 *= 0.9f;
        mVector2 = groundNormal * -(mVector2.length() + 2.3f);
    }

    if (move)
        rs::calcHackerMoveDir(&moveDir, mPlayerHack, groundNormal);

    sead::Vector3f calculatedVelocity;

    if (al::isOnGround(mTankActor, 0)) {
        al::alongVectorNormalH(&calculatedVelocity, al::getVelocity(mTankActor), mNormal,
                               groundNormal);

        mVelocity = (moveDir * 2.3f) + (calculatedVelocity * 0.8f);

    } else {
        mVelocity *= 0.9f;
    }

    // if (!al::isNearZero(mVelocity, 0.001f) && mFloat4 < 1) {
        sead::Vector3f velNormal = mVelocity;
        al::normalize(&velNormal);

        if (velNormal.dot(*mFrontDir) < 0.0f)
            velNormal *= -1.0f;

        al::lerpVec(mFrontDir, *mFrontDir, velNormal, 0.05f);
}

void TankStateHack::updateCamera() {
    // if (mFloat6) {
        alCameraPoserFunction::resetGyro(mCameraTicket->getPoser());
        alCameraPoserFunction::setGyroLimitAngleV(mCameraTicket->getPoser(), 180.0, -180.0);
        alCameraPoserFunction::setGyroSensitivity(mCameraTicket->getPoser(), 0.95, 1.0);
        // mFloat6 = 0;
    // }
    sead::Vector2f camRotate = {0.0, 0.0};
    alCameraPoserFunction::calcCameraRotateStick(&camRotate, mCameraTicket->getPoser());

    // s32 counter = mFloat4;
    if (!al::tryNormalizeOrZero(&camRotate)) {
        // mAngleTypeMulted = 0.0;
        // if (0 < counter) goto LAB_710018c444;
    }
}

bool TankStateHack::forceEndIfHack() {
    if (al::isNerve(this, &NrvTankStateHack.StartDemo) || (!mPlayerHack)) {
        return false;
    } else {
        endHack();
        return true;
    }
}

void TankStateHack::calcAimCursorLayoutPos(sead::Vector2f* outvec) const {
    al::IUseLayout* layout = nullptr;
    sead::Vector2f pos;

    if (mAimingCursor)
        layout = mAimingCursor;
    pos = al::getLocalTrans(layout);
    *outvec = pos;
}

void TankStateHack::shoot() {
    sead::Vector3f jointPos = {0.0, 0.0, 0.0};
    al::calcJointPos(&jointPos, mTankActor, "Shoot");

    sead::Vector3f diff = mNormal - jointPos;
    al::normalize(&diff);

    mTankActor->shootByPlayer(&diff, 75.0, 90);
}

void TankStateHack::exeWait() {
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Wait");
        rs::resetJudge(mHackerJudgeFall);
    }
    updateVelocity(false);
    updateCamera();
    updatePose();

    if (!tryChangeNerveIfTrigerShoot()) {
        if (!rs::updateJudgeAndResult(mHackerJudgeFall)) {
            if (rs::isHackerStopMove(mTankActor, mPlayerHack, 6.0))
                return;
            al::setNerve(this, &NrvTankStateHack.Move);
        } else {
            al::setNerve(this, &NrvTankStateHack.Fall);
        }
        return;
    }
    // mFloat6 = 0.0;
    return;
}

void TankStateHack::exeMove() {
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Move");
        rs::resetJudge(mHackerJudgeFall);
    }
    updateVelocity(true);
    updateCamera();
    updatePose();
    if (!tryChangeNerveIfTrigerShoot()) {
        if (!rs::updateJudgeAndResult(mHackerJudgeFall)) {
            if (!rs::isHackerStopMove(mTankActor, mPlayerHack, 6.0))
                return;
            al::setNerve(this, &NrvTankStateHack.Wait);
        } else {
            al::setNerve(this, &NrvTankStateHack.Fall);
        }
        return;
    }
    // mFloat6 = 0.0;
}

void TankStateHack::exeShoot() {
    if (al::isFirstStep(this)) {
        al::startAction(mTankActor, "Shoot");
        al::startHitReaction(mTankActor, "発射");
        al::setVelocityZero(mTankActor);
        // mFloat6++;
    }

    updateVelocity(al::isGreaterEqualStep(this, 15));
    updateCamera();
    updatePose();

    // if (al::isGreaterEqualStep(this, 17) &&
        // ((mFloat6 < 3 || mTankActor->countAliveBullet() < 2) && tryChangeNerveIfTrigerShoot())) {
        // return;
    // }

    al::Nerve* nerve;
    if ((!rs::updateJudgeAndResult(mHackerJudgeFall)) && (mIsFalling)) {
        al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
        nerve = &NrvTankStateHack.Land;
    }
    //  else {

    //     if (!al::isActionEnd(mTankActor)) {
    //     return;
    //     }
    //     if (mFloat6 < 3) {
    //     mFloat6 = 0;

    //     if (tryChangeNerveIfTrigerShoot()) {
    //         return;
    //     }

    //     if (!rs::isHackerStopMove(mTankActor, mPlayerHack, 6.0)) {
    //         nerve = &NrvTankStateHack.Wait;
    //     }
    //     else if (!rs::updateJudgeAndResult(mHackerJudgeFall)) {
    //         nerve = &NrvTankStateHack.Move;
    //     }
    //     else {
    //         nerve = &NrvTankStateHack.Fall;
    //     }
    //     }
    //     else {
    //     nerve = &NrvTankStateHack.ShootReload;
    //     }
    // }
    al::setNerve(this, nerve);
    return;
}

void TankStateHack::exeShootReload() {
    if (al::isFirstStep((IUseNerve*)this)) {
        al::startAction(mTankActor, "Reload");
        // mFloat6 = 0;
    }
    updateVelocity(false);
    updateCamera();
    updatePose();

    if (rs::updateJudgeAndResult(mHackerJudgeFall) || !mIsFalling) {
        if (!al::isActionEnd(mTankActor))
            return;
        if (rs::isHackerStopMove(mTankActor, mPlayerHack, 6.0)) {
            al::setNerve(this, &NrvTankStateHack.Wait);
            return;
        } else if (!rs::updateJudgeAndResult(mHackerJudgeFall)) {
            al::setNerve(this, &NrvTankStateHack.Move);
            return;
        } else {
            al::setNerve(this, &NrvTankStateHack.Fall);
            return;
        }
    }

    al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
    al::setNerve(this, &NrvTankStateHack.Land);
    return;

    //   al::setNerve((IUseNerve *)this,(Nerve *)ppuVar3);
    //   return;
}

void TankStateHack::exeFall() {
    if (al::isFirstStep((IUseNerve*)this)) {
        al::startAction(mTankActor, "Reload");
        // mFloat6 = 0;
    }
    updateVelocity(false);
    updateCamera();
    updatePose();

    if (!tryChangeNerveIfTrigerShoot()) {
        if (al::isOnGround(mTankActor, 0)) {
            al::reboundVelocityFromCollision(mTankActor, 0.0, 0.0, 1.0);
            al::setNerve(this, &NrvTankStateHack.Land);
            return;
        }
    } else {
        mIsFalling = true;
        // mFloat6 = 0;
    }
}

void TankStateHack::exeLand() {}
