#include "Enemy/Tank.h"

#include "Library/LiveActor/LiveActor.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

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

void Tank::enableShoot() {}

void Tank::disableShoot() {}

void Tank::initAfterPlacement() {}

void Tank::appear() {}

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

void Tank::endDemoWait() {}

void Tank::startShootByMofumofu() {}

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
