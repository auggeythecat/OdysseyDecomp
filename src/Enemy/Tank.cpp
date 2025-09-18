#include "Enemy/Tank.h"

#include "Enemy/EnemyStateDamageCap.h"
#include "Enemy/EnemyStateReset.h"
#include "Enemy/EnemyStateReviveInsideScreen.h"
#include "Enemy/EnemyStateSwoon.h"
#include "Enemy/TankBullet.h"
#include "Enemy/TankStateHack.h"
#include "Library/Base/StringUtil.h"
#include "Library/Collision/KCollisionServer.h"
#include "Library/Collision/PartsConnectorUtil.h"
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
#include "Library/Stage/StageSwitchUtil.h"
#include "Library/Thread/FunctorV0M.h"
#include "System/GameDataHolder.h"
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

static EnemyStateSwoonInitParam gEnemyStateSwoonInitParam = EnemyStateSwoonInitParam("SwoonStart", "Swoon", "SwoonEnd", nullptr, "SwoonStartFall", "SwoonStartLand");
const al::EnemyStateBlowDownParam sEnemyCapBlowDownParam = al::EnemyStateBlowDownParam("BlowDown", 17.0f, 40.0f, 2.0f, 0.95f, 120, 1);
const al::EnemyStateBlowDownParam sEnemyCapBlowDownParamParade = al::EnemyStateBlowDownParam("BlowDownCaptureParade", 17.0f, 40.0f, 2.0f, 0.95f, 120, 1);


Tank::Tank(const char* name) : al::LiveActor(name) {}

void Tank::init(const al::ActorInitInfo& info) {

    using TankFunctor = al::FunctorV0M<Tank*, void (Tank::*)()>;

    al::initActorWithArchiveName(this, info, "Tank", nullptr);
    al::initNerve(this, &NrvTank.Wait, 7);
    al::IUseRail* iUseRail = this;

    if (al::isExistRail(iUseRail)) {
        al::setRailPosToStart(iUseRail);
        al::syncRailTrans(this);
        al::setNerve(this, &NrvTank.Move);
    }
    GameDataHolderAccessor stageNameAccessor = GameDataHolderAccessor(this);
    const char* stageName = stageNameAccessor->getCurrentStageName();

    if (al::isEqualString(stageName, "MoonWorldCaptureParadeStage")) {
        mIsMoonCave = true;
    }

    TankStateHack* tankStateHack = new TankStateHack(this, info, &mCannonRotator, &mCannonScalor, &mFrontDir, &mPose, &mYRotator);
    mTankStateHack = tankStateHack;
    al::initNerveState(this, tankStateHack, &NrvTank.Hack, "キャプチャ");

    EnemyStateReset * enemyStateReset = new EnemyStateReset(this, info, 0);
    mEnemyStateReset = enemyStateReset;
    al::initNerveState(this, enemyStateReset, &NrvTank.Reset, "リセット");
    EnemyStateSwoon* enemyStateSwoon = new EnemyStateSwoon(this, "SwoonStart", "Swoon", "SwoonEnd", false, true);
    enemyStateSwoon->initParams(gEnemyStateSwoonInitParam);
    al::initNerveState(this, mEnemyStateSwoon, &NrvTank.Swoon, "気絶");

    const al::EnemyStateBlowDownParam* blowDownParam;
    blowDownParam = &sEnemyCapBlowDownParam;
    if (mIsMoonCave) {
        blowDownParam = &sEnemyCapBlowDownParamParade;
    }

    al::EnemyStateBlowDown* enemyStateBlowDown = new al::EnemyStateBlowDown(this, blowDownParam, "吹き飛び状態");
    mEnemyStateBlowDown = enemyStateBlowDown;
    al::initNerveState(this, enemyStateBlowDown, &NrvTank.BlowDown, "吹き飛び");

    EnemyStateReviveInsideScreen* enemyStateReviveInsideScreen1 = new EnemyStateReviveInsideScreen(this);
    mEnemyStateReviveInsideScreen1 = enemyStateReviveInsideScreen1;
    al::initNerveState(this, enemyStateReviveInsideScreen1, &NrvTank.ReviveInsideScreenNoAutoRevive, "画面内復活(外部制御)");

    EnemyStateReviveInsideScreen* enemyStateReviveInsideScreen2 = new EnemyStateReviveInsideScreen(this);
    mEnemyStateReviveInsideScreen2 = enemyStateReviveInsideScreen2;
    al::initNerveState(this, enemyStateReviveInsideScreen2, &NrvTank.ReviveInsideScreen, "画面内復活");

    GameDataHolderAccessor stageNameAccessor2 = GameDataHolderAccessor(this);
    const char* stageName2 = stageNameAccessor2->getCurrentStageName();

    bool isMetro = al::isEqualString(stageName2, "CityWorldHomeStage");

    const char* moveLimit;
    if (isMetro) {
        moveLimit = "MoveLimit";
    } else {
        moveLimit = "TankMoveLimitconst ";
    }

    al::setColliderFilterCollisionParts(this, new al::CollisionPartsFilterSpecialPurpose(moveLimit));
    
    EnemyStateDamageCap* enemyStateDamageCap = new EnemyStateDamageCap(this);
    mEnemyStateDamageCap = enemyStateDamageCap;

    al::initNerveState(this, enemyStateDamageCap,&NrvTank.DamageCap, "帽子ふきとび");
    bool isExistRail = al::isExistRail(this);
    if (isExistRail) {
        al::setRailPosToStart(this);
    }

    mMtxConnector = al::tryCreateMtxConnector(this, info);
    
    al::initJointControllerKeeper(this, 15);
    al::initJointLocalZRotator(this, &mCannonRotator, "Cannon1");
    al::initJointLocalXRotator(this, &mHipRotator, "Hip");
    al::initJointLocalScaleControllerX(this, &mCannonScalor, "Cannon2");
    mCannonScalor = 1.0;
    
    sead::Vector3f frontDir;
    al::calcFrontDir(&frontDir, this);
    mFrontDir = frontDir;
    mHipRotator = al::calcAngleOnPlaneDegree(mFrontDir, frontDir, sead::Vector3<float>::ey);

    float rWheelRotator = mRWheelRotator;
    al::initJointLocalXRotator(this, &rWheelRotator,"WheelR1");
    al::initJointLocalXRotator(this, &rWheelRotator,"WheelR2");
    al::initJointLocalXRotator(this, &rWheelRotator,"WheelR3");
    al::initJointLocalXRotator(this, &rWheelRotator,"WheelR4");
    al::initJointLocalXRotator(this, &rWheelRotator,"WheelR5");
    float lWheelRotator = mLWheelRotator;
    al::initJointLocalXRotator(this, &lWheelRotator,"WheelL1");
    al::initJointLocalXRotator(this, &lWheelRotator,"WheelL2");
    al::initJointLocalXRotator(this, &lWheelRotator,"WheelL3");
    al::initJointLocalXRotator(this, &lWheelRotator,"WheelL4");
    al::initJointLocalXRotator(this, &lWheelRotator,"WheelL5");


    sead::Quatf pose = al::getQuat(this);
    mPose = pose;
    sead::Quatf pose2 = al::getQuat(this);
    mPose2 = pose2;

    al::initJointGlobalQuatController(this, &pose, "AllRoot");
    al::initJointLocalYRotator(this, &mYRotator, "Cannon1");

    al::LiveActorGroup* bulletGroup = new al::LiveActorGroup("タンク弾グループ", 5);
    mBulletGroup = bulletGroup;
    if (0 < bulletGroup->getActorCount()) {
        for (int i = 0; i < bulletGroup->getActorCount(); ++i) {
            TankBullet* tankBullet = new TankBullet("タンク弾");
            al::initCreateActorNoPlacementInfo(tankBullet, info);
            bulletGroup->registerActor(tankBullet);
        }
        bulletGroup = mBulletGroup;
    }
    bulletGroup->makeActorDeadAll();
    if (al::isExistLinkChild(info, "ThroughCollision", 0)) {
        al::LiveActor* linkedActor = al::createLinksActorFromFactory(info, "ThroughCollision", 0);
        if (al::isExistCollisionParts(linkedActor)) {
            al::createAndSetColliderFilterExistActor(this, linkedActor);
            bulletGroup = mBulletGroup;
            if (0 < bulletGroup->getActorCount()) {
                for (int i = 0; i < bulletGroup->getActorCount(); ++i) {
                    al::createAndSetColliderFilterExistActor(bulletGroup->getActor(i), linkedActor);
                }
            }
        }
    }
    const char* capArg;
    al::tryGetStringArg(&capArg, info, "CapType");
    if (capArg == nullptr) {
        enemyStateDamageCap = mEnemyStateDamageCap;
        enemyStateDamageCap->createEnemyCap(info, "EnemyCapTank");
    } else {
        if (!al::isEqualString(capArg, "None")) {
            enemyStateDamageCap = mEnemyStateDamageCap;
            enemyStateDamageCap->createEnemyCap(info, "None");
        }
    }
    al::startAction(this, "Wait");
    if (al::listenStageSwitchOnOff(this, "EnableShoot", TankFunctor(this, &Tank::enableShoot), TankFunctor(this, &Tank::disableShoot))) {
        mCanShoot = false;
    }

    if (al::trySyncStageSwitchAppearAndKill(this)) {
        al::invalidateZPrePass(this);
        al::tryGetArg(&mIsShootToCamera, info, "IsShootToCamera");
        al::tryGetArg(&mIsOffCollideAtWait, info, "IsOffCollideAtWait");
        mClippingRadius = al::getClippingRadius(this);
    }
}

void Tank::enableShoot() {
    mCanShoot = true;
}

void Tank::disableShoot() {
    mCanShoot = false;
}

void Tank::initAfterPlacement() {
    // if (mMtxConnector != nullptr)
        // al::attachMtxConnectorToCollision(mMtxConnector, this, false);
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
    if (al::isExistRail(this)) {
        sead::Vector3f nearestRail;
        al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
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
    if (al::isExistRail(this)) {
        return false;
    } else {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
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
    return nullptr;
}

void Tank::isSwoon() {
    al::isNerve(this, &NrvTank.Swoon);
}

void Tank::appearCtrl() {}

void Tank::preInitHandleByMofumofu() {
    // mIsHandled = true;
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
    // mEnemyStateReviveInsideScreen->startRevive();
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

void Tank::exeHack() {}

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
        // mJointXScale = 0;
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
    // f32 planeAngle = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);
    // mJointXRotate = planeAngle;
    if (!al::isActionEnd(this))
        return;
    if (al::isExistRail(this)) {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, this, al::getTrans(this));
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
