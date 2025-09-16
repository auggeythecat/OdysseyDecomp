#include "Enemy/Tank.h"

#include "Enemy/EnemyStateDamageCap.h"
#include "Enemy/EnemyStateReset.h"
#include "Enemy/EnemyStateReviveInsideScreen.h"
#include "Enemy/EnemyStateSwoon.h"
#include "Enemy/TankStateHack.h"
#include "Library/Base/StringUtil.h"
#include "Library/Collision/KCollisionServer.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Collision/PartsMtxConnector.h"
#include "Library/Item/ItemUtil.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Math/MathUtil.h"
#include "Library/Movement/EnemyStateBlowDown.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/IUseRail.h"
#include "Library/Rail/RailUtil.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolderAccessor.h"
#include "Util/PlayerUtil.h"
#include "math/seadMathCalcCommon.h"
#include "math/seadQuat.h"
#include "math/seadVectorFwd.h"

namespace {
NERVE_IMPL(Tank, Wait)
NERVE_IMPL(Tank, Move)
NERVE_IMPL(Tank, Hack)
NERVE_IMPL(Tank, Reset)
NERVE_IMPL(Tank, Swoon)
NERVE_IMPL(Tank, BlowDown)
NERVE_IMPL(Tank, ReviveInsideScreenNoAutoRevive)
NERVE_IMPL(Tank, ReviveInsideScreen)
NERVE_IMPL(Tank, DamageCap)
NERVE_IMPL(Tank, PressDown)
NERVE_IMPL(Tank, Appear)
NERVE_IMPL(Tank, AttackSign)
NERVE_IMPL(Tank, Shoot)
NERVE_IMPL(Tank, AttackHit)
NERVE_IMPL(Tank, PressReaction)
NERVE_IMPL(Tank, DemoWait)

NERVES_MAKE_STRUCT(Tank, Wait, Move, Hack, Reset, Swoon, BlowDown, ReviveInsideScreenNoAutoRevive,
                   ReviveInsideScreen, DamageCap, PressDown, Appear, AttackSign, Shoot, AttackHit,
                   PressReaction, DemoWait)
}  // namespace

static EnemyStateSwoonInitParam gEnemyStateSwoonInitParam = EnemyStateSwoonInitParam(
    "SwoonStart", "Swoon", "SwoonEnd", nullptr, "SwoonStartFall", "SwoonStartLand");

Tank::Tank(const char* name) : al::LiveActor(name) {}

void Tank::init(const al::ActorInitInfo& info) {

    
    al::initActorWithArchiveName(this, info, "Tank", nullptr);
    al::initNerve(this, &NrvTank.Wait, 7);
    al::IUseRail* iUseRail = mIUseRail;

    if (al::isExistRail(iUseRail)) {
        al::setRailPosToStart(iUseRail);
        al::syncRailTrans(this);
        al::setNerve(this, &NrvTank.Move);
    }
    GameDataHolderAccessor* gameDataHolderAccessor = new GameDataHolderAccessor(mIUseSceneObj);
    const char * stageName = GameDataFunction::getCurrentStageName(*gameDataHolderAccessor);

    if (al::isEqualString(stageName, "MoonWorldCaptureParadeStage")) {
        mIsMoonCave = true;
    }

    
}

void Tank::enableShoot() {
    mCanShoot = true;
}

void Tank::disableShoot() {
    mCanShoot = false;
}

void Tank::initAfterPlacement() {
    if (mMtxConnector != nullptr)
        al::attachMtxConnectorToCollision(mMtxConnector, this, false);
    al::startMtpAnim(this, "AppearStart");
    bool iswet = al::isInAreaObj(this, "WetArea");
    al::updateMaterialCodeWet(this, iswet);
}

void Tank::appear() {
    if (al::isAlive(this) && al::isNerve(this, &NrvTank.Reset))
        return;
    al::onCollide(this);
    al::startAction(this, "Wait");
    al::LiveActor::appear();
    if (al::isExistRail(mIUseRail)) {
        sead::Vector3f nearestRail;
        al::calcNearestRailPos(&nearestRail, mIUseRail, al::getTrans(this));
        sead::Vector3f transpost = al::getTrans(this);

        if (sead::Mathf::sqrt((nearestRail.x - transpost.x) * (nearestRail.x - transpost.x) +
                              (nearestRail.y - transpost.y) * (nearestRail.y - transpost.y) +
                              (nearestRail.z - transpost.z) * (nearestRail.z - transpost.z)) <=
            150.0) {
            al::Nerve* nerve = &NrvTank.Move;
            al::setNerve(this, nerve);
            return;
        }
    }
    al::Nerve* nerve = &NrvTank.Wait;
    al::setNerve(this, nerve);
    return;
}

bool Tank::isExistAndNearRail() {
    bool israil;
    if (al::isExistRail(mIUseRail)) {
        return false;
    } else {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, mIUseRail, al::getTrans(this));
        sead::Vector3f transpost = al::getTrans(this);

        israil = sead::Mathf::sqrt((nearestRail.x - transpost.x) * (nearestRail.x - transpost.x) +
                                   (nearestRail.y - transpost.y) * (nearestRail.y - transpost.y) +
                                   (nearestRail.z - transpost.z) * (nearestRail.z - transpost.z)) <=
                 150.0;
        return israil;
    }
    return false;
}

void Tank::kill() {
    al::showModelIfHide(this);
    al::LiveActor::kill();
}

void Tank::control() {}

void Tank::die() {}

void Tank::calcAnim() {}

void Tank::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool Tank::receiveMsg(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other) {
    return false;
}

bool Tank::isMyBullet() {
    return false;
}

TankBullet Tank::shootByPlayer(const sead::Vector3f* vector, f32 float1, u32 int1) {

}

void Tank::isSwoon() {
    al::isNerve(this, &NrvTank.Swoon);
}

void Tank::appearCtrl() {}

void Tank::preInitHandleByMofumofu() {
    mIsHandled = true;
}

void Tank::appearAndDemoWait() {}

void Tank::endDemoWait() {
    al::setNerve(this, &NrvTank.Wait);
}

void Tank::startShootByMofumofu() {
    al::setNerve(this, &NrvTank.AttackSign);
}

void Tank::startBlowDownByMofumofu(al::HitSensor*) {}

void Tank::startRevive() {
    mEnemyStateReviveInsideScreen->startRevive();
}

void Tank::startRevivePrepare() {}

void Tank::forceEndHackByMofumofu() {}

void Tank::setSubjectiveCameraLimitDegree(f32 tmpname, f32 tmpname2) {}

void Tank::setSubjectiveCameraAimFollowRateV(f32 tmpname) {}

void Tank::isHacking() {}

bool Tank::isRevivePrepare() {
    return false;
}

bool Tank::isEnableStartAttack() {
    if (!al::isNerve(this, &NrvTank.Wait))
        return false;
    else
        return !al::isClipped(this);
}

void Tank::turn() {}

bool Tank::isEnableShoot() {
    return false;
}

void Tank::exeWait() {}

void Tank::exeMove() {}

void Tank::exeHack() {
    sead::Vector3f front;
    al::calcFrontDir(&front, this);
    f32 planeAngle = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);
    mJointXRotate = planeAngle;
}

void Tank::exeReset() {}

void Tank::exeSwoon() {}

void Tank::exeBlowDown() {
    if (al::isFirstStep(this))
        al::onCollide(this);
    if (al::updateNerveState(this)) {
        al::appearItem(this);
        al::setVelocityZero(this);
        al::setVelocityZero(this);
        al::startHitReaction(this, "死亡");
        mJointXScale = 0;
    }
}

void Tank::exeReviveInsideScreenNoAutoRevive() {}

void Tank::exeReviveInsideScreen() {}

void Tank::exeDamageCap() {}

void Tank::exePressDown() {}

void Tank::exeAppear() {}

void Tank::exeAttackSign() {}

void Tank::exeShoot() {}

void Tank::exeAttackHit() {
    sead::Vector3f front = {0.0, 0.0, 0.0};

    if (al::isFirstStep(this))
        al::startAction(this, "AttackHit");

    // sead::Vector3f playerPos = rs::getPlayerPos(this);
    al::turnToTarget(this, rs::getPlayerPos(this), 8.0);
    al::calcFrontDir(&front, this);
    f32 planeAngle = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);
    mJointXRotate = planeAngle;
    if (!al::isActionEnd(this))
        return;
    if (al::isExistRail(mIUseRail)) {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, mIUseRail, al::getTrans(this));
        sead::Vector3f transpost = al::getTrans(this);

        if (sead::Mathf::sqrt((nearestRail.x - transpost.x) * (nearestRail.x - transpost.x) +
                              (nearestRail.y - transpost.y) * (nearestRail.y - transpost.y) +
                              (nearestRail.z - transpost.z) * (nearestRail.z - transpost.z)) <=
            150) {
            al::setNerve(this, &NrvTank.Move);
            return;
        }
    }
    al::setNerve(this, &NrvTank.Wait);
}

void Tank::exePressReaction() {}

void Tank::exeDemoWait() {
    if (al::isFirstStep(this))
        al::startAction(this, "Wait");
}

s32 countAliveBullet() {
    return 0;
}
