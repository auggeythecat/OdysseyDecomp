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
#include "Library/Rail/RailUtil.h"
#include "Library/Movement/EnemyStateBlowDown.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "System/GameDataFunction.h"
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


void Tank::init(const al::ActorInitInfo& info) {
    al::initActorWithArchiveName(this, info, "Tank", nullptr);
    al::initNerve(this, &NrvTank.Wait, 7);

    //TODO: Figure out how to add a member varible that is IUseRail/how this works.
/*
  mIUseRail = (IUseRail *)(this + 0x40);
  if (al::isExistRail(mIUseRail);) {
    al::setRailPosToStart(mIUseRail);
    al::syncRailTrans(this);
    al::setNerve(this, &NrvTank.Move);
  }
*/
    const char* currentStage = GameDataFunction::getCurrentStageName(GameDataHolderAccessor(this));
    if (al::isEqualString(currentStage,"MoonWorldCaptureParadeStage")) {
        mIsMoonCave = true;
    }

    //TODO: Implement TankStateHack
    // sead::Quatf tmpquat = mQuat;
    // TankStateHack* sTankStateHack = new TankStateHack(this, &info, mJointZRotate, mJointXScale, &mFrontDir, tmpquat, mCannonRotator);
    
//     TankStateHack::TankStateHack
//             (this_00,this,param_1,(float *)(this + 0x160),(float *)(this + 0x164),
//             (Vector3 *)(this + 0x16c),pQVar2,(float *)(this + 0x1a4));
//     *(TankStateHack **)(this + 0x110) = this_00;
// */

//     EnemyStateReset* sEnemyStateReset = new EnemyStateReset(this, info, nullptr);
//     mEnemyStateReset = sEnemyStateReset;                                          
//     // *(undefined4 *)(sEnemyStateReset + 0x38) = 0x45fa0000

    // al::initNerveState(this, sEnemyStateReset, &NrvTank.Reset, "リセット");
    EnemyStateSwoon* sEnemyStateSwoon = new EnemyStateSwoon(this, "SwoonStart", "Swoon", "SwoonEnd", false, true);
    
    

    sEnemyStateSwoon->initParams(gEnemyStateSwoonInitParam);
    al::initNerveState(this, mNerveStateBase, &NrvTank.Swoon, "気絶");


    //TODO: Figure out this thing
/*
    pEVar4 = (EnemyStateBlowDownParam *)&DAT_7101e62dc0;
    if (this[0x1ae] == (Tank)0x0) {
        pEVar4 = (EnemyStateBlowDownParam *)&DAT_7101e62da0;
    }
*/

    al::EnemyStateBlowDown* sEnemyStateBlowDown = new al::EnemyStateBlowDown(this, /*pEVar4*/nullptr, "吹き飛び状態");

    al::initNerveState(this, sEnemyStateBlowDown, &NrvTank.BlowDown, "吹き飛び");
    EnemyStateReviveInsideScreen* sEnemyStateReviveInsideScreen1 = new EnemyStateReviveInsideScreen(this);
     // TODO: Figure out this
     // *(EnemyStateReviveInsideScreen **)(this + 0x128) = sEnemyStateReviveInsideScreen; 
     // sEnemyStateReviveInsideScreen1[0x3c] = (EnemyStateReviveInsideScreen)0x1;
    al::initNerveState(this, sEnemyStateReviveInsideScreen1, &NrvTank.ReviveInsideScreenNoAutoRevive, "キャプチャ");
    EnemyStateReviveInsideScreen* sEnemyStateReviveInsideScreen2 = new EnemyStateReviveInsideScreen(this);
     // TODO: Figure out this
     // *(EnemyStateReviveInsideScreen **)(this + 0x130) = pEVar7;
    al::initNerveState(this, sEnemyStateReviveInsideScreen2, &NrvTank.ReviveInsideScreen, "画面内復活");

    const char* currentStageName = GameDataFunction::getCurrentStageName(GameDataHolderAccessor(this));
    al::isEqualString(currentStageName,"CityWorldHomeStage");
    /* TODO: You know the drill by now
    GameDataHolderAccessor::GameDataHolderAccessor((GameDataHolderAccessor *)&local_f0,(IUseSceneObjHolder *)(this + 0x20));
    pCVar8 = (CollisionPartsFilterBase *)operator.new(0x10);
    *(undefined ***)pCVar8 = &PTR_isInvalidParts_7101ddd028;
    pcVar6 = (char *)GameDataFunction::getCurrentStageName(local_f0);
    uVar5 = al::isEqualString(pcVar6,"CityWorldHomeStage");
    if ((uVar5 & 1) == 0) {
        pcVar6 = "TankMoveLimit";
        }
        else {
            pcVar6 = "MoveLimit";
    }
    */
    EnemyStateDamageCap* sEnemyStateDamageCap = new EnemyStateDamageCap(this);
    // *(EnemyStateDamageCap **)(this + 0x118) = pEVar9;
    al::initNerveState(this,sEnemyStateDamageCap, &NrvTank.DamageCap, "帽子ふきとび");

    /*
    if (al::isExistRail(mIUseRail)) {
        al::setRailPosToStart(pIVar1);
    }
    */
    al::MtxConnector* mtxConnector = al::tryCreateMtxConnector(this, info);
    mMtxConnector = mtxConnector;


    al::initJointControllerKeeper(this, 15);
    al::initJointLocalZRotator(this, &mJointZRotate,"Cannon1");
    al::initJointLocalXRotator(this, &mJointXRotate,"Hip");
    al::initJointLocalScaleControllerX(this, &mJointXScale,"Cannon2");
    mJointXScale = 1.0;
    sead::Vector3f frontdir;
    
    al::calcFrontDir(&frontdir, this);
    mFrontDir = frontdir;

    // f32 planeangle = al::calcAngleOnPlaneDegree(mFrontDir, frontdir, sead::Vector3f::ey);


    f32 wheelRoateR = mWheelRoateR;
    // *(undefined4 *)(this + 0x168) = uVar15;

    al::initJointLocalXRotator(this, &wheelRoateR,"WheelR1");
    al::initJointLocalXRotator(this, &wheelRoateR,"WheelR2");
    al::initJointLocalXRotator(this, &wheelRoateR,"WheelR3");
    al::initJointLocalXRotator(this, &wheelRoateR,"WheelR4");
    al::initJointLocalXRotator(this, &wheelRoateR,"WheelR5");
    f32 wheelRoateL = mWheelRoateL;
    al::initJointLocalXRotator(this, &wheelRoateL,"WheelR1");
    al::initJointLocalXRotator(this, &wheelRoateL,"WheelR2");
    al::initJointLocalXRotator(this, &wheelRoateL,"WheelR3");
    al::initJointLocalXRotator(this, &wheelRoateL,"WheelR4");
    al::initJointLocalXRotator(this, &wheelRoateL,"WheelR5");

    // TODO: Why am I even writing these. Its pretty obvious.
    sead::Quatf gotQuat = al::getQuat(this);
    // *(undefined8 *)(this + 0x19c) = puVar10[1];
    sead::Quatf* gotquatref = &gotQuat;
                gotQuat = al::getQuat(this);
    // *(undefined8 *)(this + 0x158) = puVar10[1];
    // *(undefined8 *)(this + 0x150) = *puVar10;

                                               
  al::initJointGlobalQuatController(this, gotquatref, "AllRoot");
  al::initJointLocalYRotator(this, &mCannonRotator, "Cannon1");

  al::LiveActorGroup* actorGroup = new al::LiveActorGroup("タンク弾グループ", 5);

  if(0 < actorGroup->getMaxActorCount()) {
    s32 actorCounter = 0;

    
    while(actorCounter < actorGroup->getMaxActorCount()) {
//        TankBullet* tankBullet = new TankBullet("タンク弾");
//        al::initCreateActorWithPlacementInfo(tankBullet, info);
//        al::LiveActorGroup::registerActor(tankBullet)
        actorCounter++;
    }
    actorGroup = mActorGroup;
  }
  actorGroup->makeActorDeadAll();

  if (al::isExistLinkChild(info, "ThroughCollision", 0)) {
    al::LiveActor* actorLinks = al::createLinksActorFromFactory(info,"ThroughCollision", 0);
    if (al::isExistCollisionParts(actorLinks)) {
        al::createAndSetColliderFilterExistActor(this, actorLinks);
        // al::LiveActorGroup* actorRef = mActorGroup;
    }
    //TODO: TBD
  }
}


void Tank::enableShoot() { mCanShoot = true; }

void Tank::disableShoot() { mCanShoot = false; }

void Tank::initAfterPlacement() {
    if (mMtxConnector != nullptr) {
        al::attachMtxConnectorToCollision(mMtxConnector, this, false);
    }
    al::startMtpAnim(this, "AppearStart");
    // bool iswet = al::isInAreaObj(this, "WetArea");
    // al::updateMaterialCodeWet(this, iswet);
}

void Tank::appear() {

}

bool Tank::isExistAndNearRail() {
    bool israil;
    if (al::isExistRail(mIUseRail)) {
        israil = false;
    } else {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, mIUseRail, al::getTrans(this));
        sead::Vector3f transpost = al::getTrans(this);
        
        israil = sead::Mathf::sqrt((nearestRail.x - transpost.x) * (nearestRail.x - transpost.x) +
                 (nearestRail.y - transpost.y) * (nearestRail.y - transpost.y) +
                 (nearestRail.z - transpost.z) * (nearestRail.z - transpost.z)) <= 150.0;
        return israil;
    }
    return false;
}

void Tank::kill() {
    al::showModelIfHide(this);
    al::LiveActor::kill();
}

void Tank::control() {}

void Tank::die() {
    
}

void Tank::calcAnim() {}

void Tank::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool Tank::receiveMsg(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other) {
        return false;
}

bool Tank::isMyBullet() {
    return false;
}

// TankBullet Tank::shootByPlayer() // I don't want to have to deal with TankBullet related errors

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
    if (!al::isNerve(this, &NrvTank.Wait)) {
        return false;
    } else {
        return !al::isClipped(this);
        }
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
    al::updateNerveState(this);
    
}

void Tank::exeReset() {}

void Tank::exeSwoon() {}

void Tank::exeBlowDown() {
    if (al::isFirstStep(this)) {
        al::onCollide(this);
    }
    if (al::updateNerveState(this)) {
        al::appearItem(this);
        al::setVelocityZero(this);
        al::setVelocityZero(this);
        al::startHitReaction(this,"死亡");
        mJointXScale =0;
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

    if (al::isFirstStep(this)) {
        al::startAction(this, "AttackHit");
    }

    // sead::Vector3f playerPos = rs::getPlayerPos(this);
    al::turnToTarget(this, rs::getPlayerPos(this), 8.0);
    al::calcFrontDir(&front, this);
    f32 planeAngle = al::calcAngleOnPlaneDegree(mFrontDir, front, sead::Vector3f::ey);
    mJointXRotate = planeAngle;
    if (!al::isActionEnd(this)) { return; }
    if (al::isExistRail(mIUseRail)) {
        sead::Vector3f nearestRail = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&nearestRail, mIUseRail, al::getTrans(this));
        sead::Vector3f transpost = al::getTrans(this);
        
        
        if (sead::Mathf::sqrt((nearestRail.x - transpost.x) * (nearestRail.x - transpost.x) +
                 (nearestRail.y - transpost.y) * (nearestRail.y - transpost.y) +
                 (nearestRail.z - transpost.z) * (nearestRail.z - transpost.z)) <= 150) {
        al::setNerve(this, &NrvTank.Move);
        return;
        }
    }
    al::setNerve(this, &NrvTank.Wait);
}

void Tank::exePressReaction() {}

void Tank::exeDemoWait() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
    }
}
s32 countAliveBullets() {
    return 0;
}
