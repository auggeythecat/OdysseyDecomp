#include "Enemy/Tank.h"
#include <cmath>

#include "Enemy/EnemyStateDamageCap.h"
#include "Enemy/EnemyStateReset.h"
#include "Enemy/EnemyStateReviveInsideScreen.h"
#include "Enemy/EnemyStateSwoon.h"
#include "Library/Collision/KCollisionServer.h"
#include "Library/Joint/JointControllerKeeper.h"
#include "Library/Collision/PartsConnectorUtil.h"
#include "Library/LiveActor/ActorAnimFunction.h"
#include "Library/LiveActor/ActorAreaFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorModelFunction.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorInitinfo.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Movement/EnemyStateBlowDown.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Rail/RailUtil.h"

#include "Enemy/TankStateHack.h"
#include "Library/Stage/StageSwitchUtil.h"
#include "System/GameDataFunction.h"
#include "System/GameDataHolderAccessor.h"
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



Tank::Tank(const char* name) : al::LiveActor(name) {}

void Tank::init(const al::ActorInitInfo& info) {
    GameDataHolderAccessor* gamedata;

    al::initActorWithArchiveName(this, info, "Tank", nullptr) ;
    al::initNerve(this, &NrvTank.Wait, 7);

    IUseRail* rail = mIUseRail;
    if (al::isExistRail(rail)){
        al::setRailPosToStart(rail);
        al::syncRailTrans(this);
        al::setNerve(this, &NrvTank.Move);
    }

    GameDataHolderAccessor::GameDataHolderAccessor(gamedata, )



}
void Tank::enableShoot() {
    mIsEnableShoot = true;
}

void Tank::disableShoot() {
    
}

void Tank::initAfterPlacement() {}

void Tank::appear() {
    if ((al::isAlive(this)) && al::isNerve(this, &NrvTank.Reset)){
        return;
    }
    al::onCollide(this);
    al::startAction(this, "Wait");
    al::LiveActor::appear();
    if (al::isExistRail(mIUseRail)) {
        sead::Vector3f railpos = {0.0, 0.0, 0.0};
        al::calcNearestRailPos(&railpos, mIUseRail, al::getTrans(this));
        sead::Vector3f gettrans = al::getTrans(this);;
        if (sqrt((railpos.x - gettrans.x) * (railpos.x - gettrans.x))){

        }
    }

}

void Tank::isExistAndNearRail() {}

void Tank::kill() {}

void Tank::control() {}

void Tank::die() {}

void Tank::calcAnim() {}

void Tank::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool Tank::receiveMsg(const al::SensorMsg* message, al::HitSensor* self, al::HitSensor* other) {}

bool Tank::isMyBullet() {}

// TankBullet Tank::shootByPlayer() // I don't want to have to deal with TankBullet related errors

void Tank::isSwoon() {}

void Tank::appearCtrl() {}

void Tank::preInitHandleByMofumofu() {}

void Tank::appearAndDemoWait() {}

void Tank::endDemoWait() {
    al::setNerve(this, &NrvTank.Wait);
}

void Tank::startShootByMofumofu() {
    al::setNerve(this, &NrvTank.AttackSign);
}

void Tank::startBlowDownByMofumofu(al::HitSensor*) {}

void Tank::startRevive() {}

void Tank::startRevivePrepare() {}

void Tank::forceEndHackByMofumofu() {}

void Tank::setSubjectiveCameraLimitDegree(f32 tmpname, f32 tmpname2) {}

void Tank::setSubjectiveCameraAimFollowRateV(f32 tmpname) {}

void Tank::isHacking() {}

bool Tank::isRevivePrepare() {}

bool Tank::isEnableStartAttack() {}

void Tank::turn() {}

bool Tank::isEnableShoot() {}

void Tank::exeWait() {}

void Tank::exeMove() {}

void Tank::exeHack() {}

void Tank::exeReset() {}

void Tank::exeSwoon() {}

void Tank::exeBlowDown() {}

void Tank::exeReviveInsideScreenNoAutoRevive() {}

void Tank::exeReviveInsideScreen() {}

void Tank::exeDamageCap() {}

void Tank::exePressDown() {}

void Tank::exeAppear() {}

void Tank::exeAttackSign() {}

void Tank::exeShoot() {}

void Tank::exeAttackHit() {}

void Tank::exePressReaction() {}

void Tank::exeDemoWait() {}

s32 countAliveBullets() {}
