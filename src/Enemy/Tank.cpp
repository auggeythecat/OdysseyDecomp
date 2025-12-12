#include "Enemy/Tank.h"

#include "Library/Base/StringUtil.h"
#include "Library/Camera/CameraUtil.h"
#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/Collision/KCollisionServer.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/Effect/EffectSystemInfo.h"
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
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/Area/AreaObjUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Math/MathUtil.h"
#include "Library/Movement/EnemyStateBlowDown.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/RailUtil.h"
#include "Library/Stage/StageSwitchUtil.h"
#include "Library/Thread/FunctorV0M.h"

#include "Enemy/EnemyStateDamageCap.h"
#include "Enemy/EnemyStateReset.h"
#include "Enemy/EnemyStateReviveInsideScreen.h"
#include "Enemy/EnemyStateSwoon.h"
#include "Enemy/TankBullet.h"
#include "Enemy/TankStateHack.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolderAccessor.h"
#include "Util/ItemUtil.h"
#include "Util/PlayerUtil.h"
#include "Util/SensorMsgFunction.h"
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
                   PressReaction)
NERVES_MAKE_NOSTRUCT(Tank, DemoWait)

}  // namespace

const al::EnemyStateBlowDownParam gParams =
    al::EnemyStateBlowDownParam("BlowDown", 17.0f, 40.0f, 2.0f, 0.95f, 120, 1);
const al::EnemyStateBlowDownParam gMoonCaveParams =
    al::EnemyStateBlowDownParam("BlowDownCaptureParade", 17.0f, 40.0f, 2.0f, 0.95f, 120, 1);
const sead::Vector3f stransIn = {0.0, 0.0,
                                 0.0};  // I'm so confused by this. Used in Tank::receiveMsg

Tank::Tank(const char* name) : al::LiveActor(name) {}

void Tank::init(const al::ActorInitInfo& info) {
    using TankFunctor = al::FunctorV0M<Tank*, void (Tank::*)()>;

    al::initActorWithArchiveName(this, info, "Tank", nullptr);
    al::initNerve(this, &NrvTank.Wait, 7);

    if (al::isExistRail(this)) {
        al::setRailPosToStart(this);
        al::syncRailTrans(this);
        al::setNerve(this, &NrvTank.Move);
    }

    if (al::isEqualString(GameDataFunction::getCurrentStageName(this), "MoonWorldCaptureParadeStage"))
        mIsMoonCave = true;

    mTankStateHack = new TankStateHack(this, info, &mCannonRotator, &mCannonScalor, &mFrontDir, &mPose, &mYRotator);
    al::initNerveState(this, mTankStateHack, &NrvTank.Hack, "キャプチャ");

    mEnemyStateReset = new EnemyStateReset(this, info, nullptr);
    mEnemyStateReset->setValidDistance(8000.0f);
    al::initNerveState(this, mEnemyStateReset, &NrvTank.Reset, "リセット");

    
    EnemyStateSwoonInitParam initparams = EnemyStateSwoonInitParam("SwoonStart", "Swoon", "SwoonEnd", "SwoonTrampled", "SwoonStartFall", "SwoonStartLand");
    initparams.trampledAnimName = "SwoonReaction";
    mEnemyStateSwoon = new EnemyStateSwoon(this, "SwoonStart", "Swoon", "SwoonEnd", false, true);
    initparams.hasStartLandAnimation = true;
    
    mEnemyStateSwoon->initParams(initparams);
    al::initNerveState(this, mEnemyStateSwoon, &NrvTank.Swoon, "気絶");

    // const al::EnemyStateBlowDownParam* param = mIsMoonCave ? &gMoonCaveParams : &gParams;
    mEnemyStateBlowDown = new al::EnemyStateBlowDown(this, mIsMoonCave ? &gMoonCaveParams : &gParams, "吹き飛び状態");
    
    mEnemyStateReviveInsideScreen = new EnemyStateReviveInsideScreen(this);
    mEnemyStateReviveInsideScreen->changehidden(true);
    
    al::initNerveState(this, mEnemyStateReviveInsideScreen, &NrvTank.ReviveInsideScreenNoAutoRevive,
        "画面内復活(外部制御)");
        
    mEnemyStateReviveInsideScreenCopy = new EnemyStateReviveInsideScreen(this);
    al::initNerveState(this, mEnemyStateReviveInsideScreenCopy, &NrvTank.ReviveInsideScreen,
                        "画面内復活");



    const char* moveName = al::isEqualString( GameDataFunction::getCurrentStageName(this), "CityWorldHomeStage") ? "MoveLimit" : "TankMoveLimit";
    al::CollisionPartsFilterBase* collisionPartsFilter = new al::CollisionPartsFilterSpecialPurpose(moveName);
    al::setColliderFilterCollisionParts(this, collisionPartsFilter);

    mEnemyStateDamageCap = new EnemyStateDamageCap(this);
    al::initNerveState(this, mEnemyStateDamageCap, &NrvTank.DamageCap, "帽子ふきとび");

    if (al::isExistRail(this))
        al::setRailPosToStart(this);

    mMtxConnector = al::tryCreateMtxConnector(this, info);
    al::initJointControllerKeeper(this, 15);
    al::initJointLocalZRotator(this, &mCannonRotator, "Cannon1");
    al::initJointLocalXRotator(this, &mHipRotator, "Hip");
    al::initJointLocalScaleControllerX(this, &mCannonScalor, "Cannon2");
    mCannonScalor = 1.0;
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    mFrontDir = frontDir; // Uh is this a N's mistake?
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, frontDir, sead::Vector3f::ey);

    al::initJointLocalXRotator(this, &mWheelRRotator, "WheelR1");
    al::initJointLocalXRotator(this, &mWheelRRotator, "WheelR2");
    al::initJointLocalXRotator(this, &mWheelRRotator, "WheelR3");
    al::initJointLocalXRotator(this, &mWheelRRotator, "WheelR4");
    al::initJointLocalXRotator(this, &mWheelRRotator, "WheelR5");
    al::initJointLocalXRotator(this, &mWheelLRotator, "WheelL1");
    al::initJointLocalXRotator(this, &mWheelLRotator, "WheelL2");
    al::initJointLocalXRotator(this, &mWheelLRotator, "WheelL3");
    al::initJointLocalXRotator(this, &mWheelLRotator, "WheelL4");
    al::initJointLocalXRotator(this, &mWheelLRotator, "WheelL5");

    mPose.set(al::getQuat(this));
    mSpawnPose.set(al::getQuat(this));

    al::initJointGlobalQuatController(this, &mPose, "AllRoot");
    al::initJointLocalYRotator(this, &mYRotator, "Cannon1");

    al::DeriveActorGroup<TankBullet>* bulletGroup =
        new al::DeriveActorGroup<TankBullet>("タンク弾グループ", 5);

    mBulletGroup = bulletGroup;
    for (s32 actorCount = 0; actorCount < bulletGroup->getMaxActorCount(); actorCount++) {
        TankBullet* tankBullet = new TankBullet("タンク弾");
        al::initCreateActorNoPlacementInfo(tankBullet, info);
        bulletGroup->registerActor(tankBullet);
    }

    mBulletGroup->makeActorDeadAll();

    if (al::isExistLinkChild(info, "ThroughCollision", 0)) {
        al::LiveActor* actorLinks = al::createLinksActorFromFactory(info, "ThroughCollision", 0);

        if (al::isExistCollisionParts(actorLinks)) {
            al::createAndSetColliderFilterExistActor(this, actorLinks);
            for(s32 i=0;i<mBulletGroup->getActorCount();i++){
                al::createAndSetColliderFilterExistActor
                    (mBulletGroup->getDeriveActor(i),actorLinks);
            }
        }
    }

    if (!mIsCaptured) {
        const char* stringArg = nullptr;
        al::tryGetStringArg(&stringArg, info, "CapType");
        if (!stringArg) {
            mEnemyStateDamageCap->createEnemyCap(info, "EnemyCapTank");
        } else if (!al::isEqualString(stringArg, "None")) {
            mEnemyStateDamageCap->createEnemyCap(info, stringArg);
        }
    }
    
    al::startAction(this, "Wait");

    if (al::listenStageSwitchOnOff(this, "EnableShoot", TankFunctor(this, &Tank::enableShoot),
                                   TankFunctor(this, &Tank::disableShoot)))
        mIsShoot = false;

    if (!al::trySyncStageSwitchAppearAndKill(this)) {
        al::invalidateZPrePass(this);
        al::tryGetArg(&mIsShootToCamera, info, "IsShootToCamera");
        al::tryGetArg(&mIsOffCollideAtWait, info, "IsOffCollideAtWait");
        mClippingRadius = al::getClippingRadius(this);
        makeActorAlive();
    }
}


void Tank::initAfterPlacement() {
    if (mMtxConnector != nullptr)
        al::attachMtxConnectorToCollision(mMtxConnector, this, false);
    al::startMtpAnim(this, "AppearStart");
    al::updateMaterialCodeWet(this, al::isInAreaObj(this, "WetArea"));
}

void Tank::appear() {
    bool shouldMove;
    if (al::isAlive(this) && al::isNerve(this, &NrvTank.Reset))
        return;

    al::onCollide(this);
    al::startAction(this, "Wait");
    al::LiveActor::appear();

    if (al::isExistRail(this)) {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
        shouldMove = !((nearestRail - al::getTrans(this)).length() > 150.0f);
        if (shouldMove) {
            al::setNerve(this, &NrvTank.Move);
            return;
        }
    }
    al::setNerve(this, &NrvTank.Wait);
}

bool Tank::isExistAndNearRail() {
    bool shouldMove = false;
    if (al::isExistRail(this)) {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
        f32 length = (nearestRail - al::getTrans(this)).length();
        shouldMove = !(length > 150.0f);
    }
    return shouldMove;
}

void Tank::kill() {
    al::showModelIfHide(this);
    al::LiveActor::kill();
}

void Tank::control() {
    bool shouldMove = false;

    if (mMtxConnector != nullptr) {
            if (al::isExistRail(this)) {
            sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
            al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
            shouldMove = !((nearestRail - al::getTrans(this)).length() > 150.0f);
        }
        if (!shouldMove)
            al::connectPoseQT(this, mMtxConnector);
    }
    if (!al::isNerve(this, &NrvTank.Hack) && !al::isNerve(this, &NrvTank.Swoon))
        mPose.set(al::getQuat(this));

    if (!al::isNerve(this, &NrvTank.Hack))
        al::tryStartVisAnimIfNotPlaying(this, "HackOffCapOff");

    if ((!al::isNerve(this, &NrvTank.Reset) && !al::isNerve(this, &NrvTank.ReviveInsideScreen) &&
         !al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive)) &&
        (!al::isNerve(this, &NrvTank.BlowDown) && !al::isNerve(this, &NrvTank.PressDown))) {
        
        if ((((al::isInDeathArea(this, al::getTrans(this)) || (al::isCollidedFloorCode(this, "DamageFire") ||
                                           al::isCollidedFloorCode(this, "Needle"))) ||
              al::isCollidedFloorCode(this, "Poison")) ||
             al::isInWater(this)) &&
            (!al::isNerve(this, &NrvTank.Hack) || mTankStateHack->forceEndIfHack())) {
            al::setVelocityZero(this);
            al::startHitReaction(this, "死亡");
            mCannonScalor = 1.0;
            if (mIsNerveReset) {
                al::setNerve(this, &NrvTank.Reset);
                kill();
                return;
            }
            mIsCaptured ? al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive) :
                          al::setNerve(this, &NrvTank.Reset);
        }
    }
}

void Tank::die() {
    al::setVelocityZero(this);
    al::startHitReaction(this, "死亡");
    mCannonScalor = 1.0;
    if (mIsNerveReset) {
        al::setNerve(this, &NrvTank.Reset);
        kill();
        return;
    }

    if (!mIsCaptured)
        al::setNerve(this, &NrvTank.Reset);
    else
        al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
}

void Tank::calcAnim() {
    al::LiveActor::calcAnim();
    if (al::isNerve(this, &NrvTank.Hack))
        al::setModelAlphaMask(this, mTankStateHack->mAlphaMask);
}

void Tank::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if (al::isNerve(this, &NrvTank.Hack))
        mTankStateHack->attackSensor(self, other);

    if ((((!al::isNerve(this, &NrvTank.Hack) && !al::isNerve(this, &NrvTank.Reset)) &&
          !al::isNerve(this, &NrvTank.ReviveInsideScreen)) &&
         ((!al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive) &&
           !al::isNerve(this, &NrvTank.BlowDown)))) &&
        !al::isNerve(this, &NrvTank.PressDown)) {
        if (al::isSensorEnemyBody(self)) {
            if (!al::isSensorPlayer(other)) {
                al::sendMsgPush(other, self);
                return;
            }
        } else if (al::isSensorEnemyAttack(self) && !rs::sendMsgTankKickEnemy(other, self)) {
            if (al::isNerve(this, &NrvTank.Swoon) ||
                (al::isNerve(this, &NrvTank.Appear) || !al::sendMsgEnemyAttack(other, self))) {
                rs::sendMsgPushToPlayer(other, self);
                return;
            }
            if (!al::isNerve(this, &NrvTank.AttackSign) && !al::isNerve(this, &NrvTank.Shoot)) {
                al::setNerve(this, &NrvTank.AttackHit);
                return;
            }
        }
    }
}

bool Tank::receiveMsg(const al::SensorMsg* message, al::HitSensor* other, al::HitSensor* self) {
    al::LiveActor* bullet = al::getSensorHost(other);

    for (s32 count = 0; count < mBulletGroup->getActorCount(); count++) {
        if (bullet == mBulletGroup->getDeriveActor(count))
            return false;
    }

    if (((al::isNerve(this, &NrvTank.Reset) || al::isNerve(this, &NrvTank.ReviveInsideScreen)) ||
         al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive)) &&
        !al::isMsgPlayerDisregard(message)) {
        return true;
    }

    if (rs::isMsgKillByShineGet(message) || rs::isMsgKillByHomeDemo(message)) {
        if (al::isNerve(this, &NrvTank.Hack))
            mTankStateHack->forceEndIfHack();
        al::setNerve(this, &NrvTank.ReviveInsideScreen);
        al::tryKillEmitterAndParticleAll(this);
        movement();
        return true;
    }

    if (al::isNerve(this, &NrvTank.Hack) &&
        mTankStateHack->receiveMsgHackEnd(message, other, self)) {
        al::invalidateZPrePass(this);
        al::setNerve(this, &NrvTank.Swoon);
    } else {
        if ((((rs::isMsgCapStartLockOn(message) || rs::isMsgCapKeepLockOn(message)) &&
              !mEnemyStateDamageCap->isCapOn()) &&
             (((!al::isNerve(this, &NrvTank.DamageCap) || al::isGreaterEqualStep(this, 8)) &&
                !al::isNerve(this, &NrvTank.Hack)) &&
               (!al::isNerve(this, &NrvTank.Reset) &&
                 !al::isNerve(this, &NrvTank.ReviveInsideScreen)))) &&
            (!al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive) &&
              (!al::isNerve(this, &NrvTank.BlowDown) &&
                !al::isNerve(this, &NrvTank.PressDown)))) {
            return true;
        }

        if (!rs::isMsgCapCancelLockOn(message) || !mTankStateHack->receiveMsgInitCapTargetInfo(message) || !mTankStateHack->receiveMsgNpcScareByEnemy(message))
            return true;

        if (mTankStateHack->receiveMsgHackStart(message, other, self)) {
            if (al::isNerve(this, &NrvTank.Hack)) {
                if (rs::isMsgKillBySwitchTimer(message)) {
                    mIsNerveReset = true;
                    return true;
                }

                if (!rs::isMsgHackMarioCheckpointFlagWarp(message))
                    return mTankStateHack->receiveMsg(message, other, self);
                al::startAction(this, "Wait");
                mTankStateHack->forceEndIfHack();
                return true;
            }
        }

        if (al::isNerve(this, &NrvTank.Swoon) &&
            !al::tryReceiveMsgPushAndAddVelocityH(this, message, other, self, 4.0)) {
            return false;
        }

        if (((!al::isNerve(this, &NrvTank.Hack) && !al::isNerve(this, &NrvTank.Reset)) &&
             !al::isNerve(this, &NrvTank.ReviveInsideScreen)) &&
            ((!al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive) &&
              !al::isNerve(this, &NrvTank.BlowDown)) &&
             !al::isNerve(this, &NrvTank.PressDown))) {
            if (!rs::isMsgYoshiTongueAttack(message))
                return false;

            // if (rs::isMsgTargetMarkerPosition(message)) {
            //     sead::Vector3f offset = {0.0, 0.0, 0.0};

            //     al::calcTransLocalOffset(&offset, this, stransIn);

            //     if (mEnemyStateDamageCap->isCapOn())
            //         offset.y += 50.0;
            //     rs::setMsgTargetMarkerPosition(message, offset);
            //     return false;
            // }

            if (!rs::isMsgGrowerAttack(message))
                return false;

            if (rs::isMsgBlowObjAttackReflect(message)) {
                rs::requestHitReactionToAttacker(message, self, other);
                mEnemyStateDamageCap->blowCap(other);
                return true;
            }

            if (rs::isMsgBlowDown(message)) {
                rs::requestHitReactionToAttacker(message, self, other);
                rs::setAppearItemFactorAndOffsetByMsg(this, message, other);
                mEnemyStateBlowDown->start(other);
                mEnemyStateDamageCap->blowCap(other);
                al::setNerve(this, &NrvTank.BlowDown);
            }

            if (mEnemyStateDamageCap->tryReceiveMsgCapBlow(message, other, self))
                al::setNerve(this, &NrvTank.DamageCap);

            if (rs::isMsgPlayerAndCapObjHipDropAll(message) || rs::isMsgSenobiTrample(message)) {
                rs::requestHitReactionToAttacker(message, self, other);
                rs::setAppearItemFactorAndOffsetByMsg(this, message, other);
                al::startHitReaction(this, "ヒップドロップされた");
                mEnemyStateDamageCap->blowCap(other);
                al::setNerve(this, &NrvTank.PressDown);
            }
            if ((al::isNerve(this, &NrvTank.Swoon) && al::getNerveStep(this) > mNerveStep + 15) &&
                mEnemyStateSwoon->tryReceiveMsgTrampleReflect(message)) {
                mNerveStep = al::getNerveStep(this);
                rs::requestHitReactionToAttacker(message, self, other);
                return true;
            }

            if (al::isMsgPlayerTrampleReflect(message)) {
                if ((al::isNerve(this, &NrvTank.PressReaction) && al::isLessEqualStep(this, 15)) ||
                    (al::isNerve(this, &NrvTank.Swoon) &&
                     al::getNerveStep(this) < mNerveStep + 15)) {
                    return false;
                }
                mNerveStep = al::getNerveStep(this);

                rs::requestHitReactionToAttacker(message, self, other);
                mEnemyStateDamageCap->blowCap(other);

                if (al::isNerve(this, &NrvTank.Swoon))
                    return true;
                al::setNerve(this, &NrvTank.PressReaction);
            }
            if (rs::isMsgKillByShineGet(message)) {
                al::setVelocityZero(this);
                al::startHitReaction(this, "死亡");
                mCannonScalor = 1.0;
                if (mIsNerveReset) {
                    al::setNerve(this, &NrvTank.Reset);
                    movement();
                }
                mIsCaptured ? al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive) :
                              al::setNerve(this, &NrvTank.Reset);

                if (rs::isMsgKillBySwitchTimer(message)) {
                    al::startHitReaction(this, "消滅");
                    return false;
                }
            }

            if (!al::isMsgRestart(message) || al::isNerve(this, &NrvTank.Hack))
                return al::isMsgChangeAlpha(message);

            if ((al::isAlive(this)) && (!al::isHideModel(this)))
                al::startHitReaction(this, "消滅");
            //   al::resetRotatePosition(this, mEnemyStateReset->getRot(),
            //   mEnemyStateReset->getPos());
            mEnemyStateDamageCap->resetCap();
            appear();
        }
        al::onCollide(this);
        al::validateZPrePass(this);
        // al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
        al::setNerve(this, &NrvTank.Hack);
    }
    return true;
}

bool Tank::isMyBullet(al::HitSensor* self) {
    al::LiveActor* bullet = al::getSensorHost(self);
    for (s32 count = 0; count < mBulletGroup->getActorCount(); count++)
        if (bullet == mBulletGroup->getDeriveActor(count))
            return true;
    return false;
}

TankBullet Tank::shootByPlayer(const sead::Vector3f* vector, f32 f321, u32 int1) {
    return nullptr;
}

bool Tank::isSwoon() const {
    return al::isNerve(this, &NrvTank.Swoon);
}

void Tank::appearCtrl() {
    al::onCollide(this);
    al::startAction(this, "Wait");
    al::LiveActor::appear();
    al::hideModel(this);
    al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
}

void Tank::preInitHandleByMofumofu() {
    mIsCaptured = true;
}

void Tank::appearAndDemoWait() {
    appear();
    al::setNerve(this, &DemoWait);
}

void Tank::endDemoWait() {
    al::setNerve(this, &NrvTank.Wait);
}

void Tank::startShootByMofumofu() {
    al::setNerve(this, &NrvTank.AttackSign);
}

void Tank::startBlowDownByMofumofu(al::HitSensor* self) {
    if ((((!al::isNerve(this, &NrvTank.Hack)) && !al::isNerve(this, &NrvTank.Reset)) &&
         !al::isNerve(this, &NrvTank.ReviveInsideScreen)) &&
        (((!al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive) &&
           !al::isNerve(this, &NrvTank.BlowDown))) &&
         !al::isNerve(this, &NrvTank.PressDown))) {
        al::invalidateClipping(this);
        mBulletGroup->killAll();
        al::setAppearItemFactor(this, "間接攻撃", self);
        sead::Vector3f back = {0.0, 0.0, 0.0};
        al::calcBackDir(&back, this);
        mEnemyStateBlowDown->start(back);
        al::setNerve(this, &NrvTank.BlowDown);
    }
}

void Tank::startRevive() {
    mEnemyStateReviveInsideScreen->startRevive();
}

void Tank::startRevivePrepare() {
    al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
}

void Tank::forceEndHackByMofumofu() {
    mTankStateHack->forceEndIfHack();
    mBulletGroup->makeActorDeadAll();
    makeActorDead();
}

void Tank::setSubjectiveCameraLimitDegree(f32 camlimitx, f32 camlimity) {
    TankStateHack* statehack = mTankStateHack;
    statehack->mCamLimitx = camlimitx;
    statehack->mCamLimity = camlimity;
}

void Tank::setSubjectiveCameraAimFollorRateV(f32 aimfollowrateV) {
    mTankStateHack->mAimFollowRateV = aimfollowrateV;
}

void Tank::isHacking() const {
    al::isNerve(this, &NrvTank.Hack);
}

bool Tank::isRevivePrepare() const {
    if (al::isNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive))
        return mEnemyStateReviveInsideScreen->isDisappear();
    return false;
}

bool Tank::isEnableStartAttack() const {
    if (!al::isNerve(this, &NrvTank.Wait))
        return false;
    else
        return !al::isClipped(this);
}

void Tank::turn() {
    sead::Vector3f turnTarget = {0.0f, 0.0f, 0.0f};
    sead::Vector3f random;
    sead::Vector3f pos;
    sead::Vector3f front;

    f32 Float;
    bool checkbodypos = (rs::getPlayerBodyPos(this) - al::getTrans(this)).length() > (rs::getPlayerPos(this) - al::getTrans(this)).length();
    checkbodypos ? pos.set(rs::getPlayerBodyPos(this)) : pos.set(rs::getPlayerPos(this));

    pos.y += 20.0f;
    turnTarget = pos;
    if (mIsShootToCamera) {
        turnTarget = al::getCameraPos(this, 0);
    }

    
    if (al::isJudgedToClipFrustumWithoutFar(this, mClippingRadius, 300.0)) {
        random = sead::Vector3f::zero;
        al::getRandomVector(&random, 400.0);
        turnTarget += random;
    }
    al::turnToTarget(this, turnTarget, 1.1);
    
    random = {0.0, 0.0, 0.0};
    al::calcJointPos(&random, this, "Shoot");
    turnTarget.y += 50.0f;
    sead::Vector3f corrected = (turnTarget) - random;
    al::tryNormalizeOrZero(&corrected);

    al::calcFrontDir(&front, this);
    
    Float = -al::calcAngleDegree(corrected, front);
    if (random.y <= turnTarget.y) {
        Float = -Float;
    }
    Float = Float - mCannonRotator;
    if ((-0.47 <= Float) && (0.47 < Float)) {
        Float = 0.47 + mCannonRotator;
    } else {
        Float = -0.47 + mCannonRotator;
    }
    if ((-10.0 <= Float ) && ( 25.0 < Float)) {
        mCannonRotator = 25.0f;
        return;
    }
    mCannonRotator = -10.0f;
}

bool Tank::isEnableShoot() {
    sead::Vector3f startPos = al::getTrans(this);
    sead::Vector3f jointpos;
    sead::Vector3f front;
    
    al::calcJointPos(&jointpos, this, "Shoot");
    startPos.y += jointpos.z;

    al::calcFrontDir(&front, this);
    
    sead::Vector3f hitPos;
    sead::Vector3f hitNormal;

    return !alCollisionUtil::getHitPosAndNormalOnArrow(this, &hitPos, &hitNormal, startPos, front * 100.0f, nullptr, nullptr);
}

void Tank::exeWait() {
    if (al::isFirstStep(this)) {
        al::validateClipping(this);
        al::startVisAnim(this, "HackOff");
        al::startAction(this, "Wait");
        mIsOffCollideAtWait ? al::offCollide(this) : al::onCollide(this);    
    }

    turn();

    sead::Vector3f front;
    al::calcFrontDir(&front, this);
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);

    if (!mIsOffCollideAtWait) {
        al::scaleVelocity(this, 0.8);
        al::isOnGround(this, 0) ? al::addVelocityToGravityFittedGround(this, 0.7, 0) : al::addVelocityToGravity(this, 8.0);
    }
    
    if ((!mIsCaptured && al::isGreaterEqualStep(this, 90)) && !rs::isPlayerHackFukankun(this) && !rs::isPlayer2D(this) && mIsShoot) {
        al::setNerve(this, &NrvTank.AttackSign);
    }
}

void Tank::exeMove() {
    if (al::isFirstStep(this)) {
        al::validateClipping(this);
        al::startVisAnim(this, "HackOff");
        al::startAction(this, "Move");
        if (al::isRailReachedEnd(this)) {
            mIsRailAtEnd = true;
        } else {
            if (al::isRailReachedStart(this)) {
                mIsRailAtEnd = false;
            }
        }
        
        al::setVelocityZero(this);
        al::offCollide(this);
    }
    
    al::startMtsAnimAndSetFrameAndStop(this, "Move", mAnimFrame);
    mAnimFrame += mIsRailAtEnd ? 1 : -1;
    
    sead::Vector3f rail = al::getRailDir(this);
    
    turn();
    
    al::lerpVec(&mFrontDir, mFrontDir, rail, 0.02f);
    sead::Vector3f newFront;
    al::calcFrontDir(&newFront, this);
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, newFront, sead::Vector3f::ey);

    mWheelRRotator += mIsRailAtEnd ? 5.0f : -5.0f;
    mWheelLRotator -= mIsRailAtEnd ? 5.0f : -5.0f;

    if (al::isExistRail(this)) {
        if (!al::isRailReachedNearRailPoint(this, 10.0f) || !al::isGreaterEqualStep(this, 10.0f)) {
            al::isLoopRail(this) ? al::moveSyncRailLoop(this, 3.0f) : al::moveSyncRailTurn(this, 3.0f);
        } else {
            al::setSyncRailToRailPoint(this, al::calcCoordNearestRailPointNo(this));
        mIsShoot ?  al::setNerve(this, &NrvTank.AttackSign) : al::setNerve(this, &NrvTank.Move);
        }
    }
}

void Tank::exeHack() {
    sead::Vector3f front;
    al::calcFrontDir(&front,this);
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);
    al::updateNerveState(this);
}

void Tank::exeReset() {
    if (al::isFirstStep(this)) {
        mCannonRotator = 0.0f;
        mResetCounter = 0;
        al::updateNerveState(this);
        mTankStateHack->reset();
        mPose.set(mSpawnPose);
        return;
    }
    
    if (rs::isPlayerHackTank(this))
        return;
    
    if (mResetCounter++ >= 240 && (al::getTrans(this) - rs::getPlayerPos(this)).length() < 1500.0f){
        al::setNerve(this, &NrvTank.ReviveInsideScreen);
        return;
    }

    
    
    if (al::updateNerveState(this)) {
        mEnemyStateDamageCap->resetCap();
        if (isExistAndNearRail()) {
            al::setRailPosToStart(this);    
            al::setNerve(this, &NrvTank.Move);
            return;
        }
        al::setNerve(this, &NrvTank.Wait);
    }
}

void Tank::exeSwoon() {
    if (al::isFirstStep(this)) {
        mNerveStep = -15;
        al::invalidateClipping(this);
        al::setVelocityZero(this);
        al::onCollide(this);
        al::startVisAnim(this, "HackOff");
        al::startMtsAnimAndSetFrameAndStop(this, "Move", mAnimFrame);
    }
    sead::Vector3f newFrontDir;
    al::calcFrontDir(&newFrontDir, this);

    al::lerpVec(&mFrontDir, mFrontDir, newFrontDir, 0.2);
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, newFrontDir, sead::Vector3f::ey);
    al::scaleVelocity(this, 0.8);

    if (al::isOnGround(this, 0))
        al::addVelocityToGravityFittedGround(this, 0.7, 0);
    else
        al::addVelocityToGravity(this, 8.0);

    mCannonScalor = al::lerpValue(
        mCannonScalor, 1.0, al::calcNerveRate(this, al::getActionFrameMax(this, "SwoonStart")));

    if (al::updateNerveState(this)) {
        al::startHitReaction(this, "消滅");
        if (!mIsNerveReset)
            if (!mIsCaptured)
                al::setNerve(this, &NrvTank.ReviveInsideScreen);
            else
                al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
        else
            kill();
    }
}

void Tank::exeBlowDown() {
    if (al::isFirstStep(this))
        al::onCollide(this);
    if (al::updateNerveState(this)) {
        al::appearItem(this);
        al::setVelocityZero(this);
        al::setVelocityZero(this);
        al::startHitReaction(this, "死亡");
        mCannonScalor = 1;
        if (mIsNerveReset) {
            al::setNerve(this, &NrvTank.Reset);
            kill();
            return;
        }
        if (mIsCaptured) {
            al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
            return;
        }

        al::setNerve(this, &NrvTank.Reset);
        return;
    }
}

void Tank::exeReviveInsideScreenNoAutoRevive() {
    if (al::updateNerveStateAndNextNerve(this, &NrvTank.Wait)) {
        al::validateClipping(this);
        mTankStateHack->reset();
        if (!mIsCaptured) {
            mEnemyStateDamageCap->resetCap();
            al::showModelIfHide(this);
        }
    }
}

void Tank::exeReviveInsideScreen() {
    bool shouldMove;
    if (!al::isFirstStep(this)) {
        if (al::isExistRail(this)) {
            sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
            al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
            f32 length = (nearestRail - al::getTrans(this)).length();
            shouldMove = !(length > 150.0f);
            if (shouldMove) {
                al::setRailPosToStart(this);
                al::syncRailTrans(this);
            }
        }
    } else {
        mPose.set(mSpawnPose);
    }
    if (al::updateNerveState(this)) {
        mEnemyStateDamageCap->resetCap();
        mTankStateHack->reset();
        al::setNerve(this, &NrvTank.Appear);
    }
}

void Tank::exeDamageCap() {
    if (al::updateNerveState(this)) {
        bool shouldMove = false;
        if (al::isExistRail(this)) {
            sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
            al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
            f32 length = (nearestRail - al::getTrans(this)).length();
            shouldMove = !(length > 150.0f);
        }
        if (shouldMove) {
            al::setSyncRailToNearestPos(this);
            al::setNerve(this, &NrvTank.Move);
        } else {
            al::setNerve(this, &NrvTank.Wait);
        }
    }
}

void Tank::exePressDown() {
    if (al::isFirstStep(this)) {
        const char* actionName;
        al::onCollide(this);
        actionName = "PressDownCaptureParade";
        if (!mIsMoonCave)
            actionName = "PressDown";
        al::startAction(this, actionName);
    }

    if (al::isActionEnd(this)) {
        al::appearItem(this);
        al::setVelocityZero(this);
        al::startHitReaction(this, "死亡");
        mCannonScalor = 1.0;
        if (mIsNerveReset) {
            al::setNerve(this, &NrvTank.Reset);
            kill();
            return;
        }
        if (mIsCaptured)
            al::setNerve(this, &NrvTank.ReviveInsideScreenNoAutoRevive);
        else
            al::setNerve(this, &NrvTank.Reset);
        return;
    }
}

void Tank::exeAppear() {
    if (al::isFirstStep(this)) {
        sead::Vector3f front;
        al::invalidateClipping(this);
        al::startVisAnim(this, "HackOff");
        al::startAction(this, "AppearStart");
        if (!al::isExistRail(this)) {
            al::calcFrontDir(&front, this);
        } else {
            mFrontDir = al::getRailDir(this);
            al::calcFrontDir(&front, this);
        }
        mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);
    }
    al::startMtsAnimAndSetFrameAndStop(this, "Move", mAnimFrame);
    if (al::isActionEnd(this)) {
        al::validateClipping(this);
        if (!al::isExistRail(this))
            al::setNerve(this, &NrvTank.Wait);
        else
            al::setNerve(this, &NrvTank.Move);
    }
    return;
}

void Tank::exeAttackSign() {
    if (al::isFirstStep(this)) {
        al::invalidateClipping(this);
        al::startAction(this, "AttackSign");
        mTotalShots = 0.0;
    }
    if (!al::isExistRail(this) && !mIsOffCollideAtWait) {
        al::scaleVelocity(this, 0.8);
        if (!al::isOnGround(this, 0))
            al::addVelocityToGravity(this, 8.0);
        else
            al::addVelocityToGravityFittedGround(this, 0.7, 0);
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &NrvTank.Shoot);
}

void Tank::exeShoot() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Shoot");
        mTotalShots = mTotalShots + 1;
        sead::Vector3f jointPos;
        sead::Vector3f frontDir;
        al::calcJointPos(&jointPos, this, "Shoot");
        al::calcFrontDir(&frontDir, this);
        sead::Vector3f pos;
        sead::Vector3f normal;

        if (!alCollisionUtil::getHitPosAndNormalOnArrow(this, &pos, &normal, al::getTrans(this), frontDir * 100.0, nullptr,
                                                        nullptr)) {
            TankBullet* bullet = mBulletGroup->tryFindDeadDeriveActor();
            if (!bullet) {
                al::setNerve(this, &NrvTank.AttackSign);
                return;
            }

            al::calcJointOffsetPos(&pos, this, "Shoot", sead::Vector3f(0.0f, -50.0f, 0.0f));
            al::calcFrontDir(&frontDir, this);

            if (al::isJudgedToClipFrustumWithoutFar(this, mClippingRadius, 300.0f)) {
                al::rotateVectorDegreeY(&frontDir, ( (s32) al::getRandomDegree() % 40) -20);
            }

                sead::Vector3f gravity = al::getGravity(this);
                sead::Vector3f local_50;
                local_50.x = (jointPos.y * gravity.z) - (jointPos.z * gravity.y);
                local_50.y = (jointPos.z * gravity.x) - (jointPos.x * gravity.z);
                local_50.z = (jointPos.x * gravity.y) - (jointPos.y * gravity.x);
                al::rotateVectorCenterDegree(&jointPos, jointPos, local_50, sead::Vector3f::zero, -mCannonRotator);
                bullet->shoot(al::getTrans(this), jointPos * 25.0f, 200, true, false);

        }
    }

    if (!al::isExistRail(this) && !mIsOffCollideAtWait) {
        al::scaleVelocity(this, 0.8);
        al::isOnGround(this, 0) ? al::addVelocityToGravityFittedGround(this, 0.7f, 0) : al::addVelocityToGravity(this,8.0);
    }
    if (mTotalShots < 1 && al::isGreaterEqualStep(this, 15)) {
        turn();
        al::setNerve(this, &NrvTank.Shoot);
        return;
    } else {
        if (!al::isActionEnd(this)) 
            return;
        al::validateClipping(this);
        if (al::isExistRail(this)) {
        
            sead::Vector3f rail;

            al::calcNearestRailPos(&rail, this, al::getTrans(this));
            
            if ((rail - al::getTrans(this)).length() <= 150.0) {
            al::setNerve(this, &NrvTank.Move);
            return;
            }
        }
        al::setNerve(this, &NrvTank.Wait);
    }
}

void Tank::exeAttackHit() {
    sead::Vector3f nearestRail;
    sead::Vector3f front;
    bool shouldMove = false;
    
    
    if (al::isFirstStep(this))
        al::startAction(this, "AttackHit");

    al::turnToTarget(this, rs::getPlayerPos(this), 8.0);
    al::calcFrontDir(&front, this);
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);

    if (al::isActionEnd(this)) {
        if (al::isExistRail(this)) {
            nearestRail = {0.0, 0.0, 0.0};
            al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
            shouldMove = !((nearestRail - al::getTrans(this)).length() > 150.0f);
        }
        shouldMove ? al::setNerve(this, &NrvTank.Move) : al::setNerve(this, &NrvTank.Wait);
    }
}

void Tank::exePressReaction() {
    if (al::isFirstStep(this))
        al::startAction(this, "Reaction");
    if (al::isActionEnd(this)) {
        bool shouldMove = false;

        if (al::isExistRail(this)) {
            sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
            al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
            f32 length = (nearestRail - al::getTrans(this)).length();
            shouldMove = !(length > 150.0f);
        }
        if (shouldMove)
            al::setNerve(this, &NrvTank.Move);
        else
            al::setNerve(this, &NrvTank.Wait);
    }
}

void Tank::exeDemoWait() {
    if (al::isFirstStep(this))
        al::startAction(this, "Wait");
}

s32 Tank::countAliveBullet() const {
    if (mBulletGroup->getActorCount() > 0) {
        s32 aliveCount = 0;
        for (s32 i = 0; i < mBulletGroup->getActorCount(); i++)
            if (al::isAlive(mBulletGroup->getActor(i)))
                aliveCount++;
        return aliveCount;
    } else {
        return 0;
    }
}
