#include "Enemy/TankBullet.h"

#include "Library/LiveActor/LiveActor.h"

TankBullet::TankBullet(const char* name) : al::LiveActor(name) {}

void TankBullet::init(const al::ActorInitInfo&) {}

void TankBullet::attackSensor(al::HitSensor* self, al::HitSensor* other) {}

bool TankBullet::explode() {}

void TankBullet::appear() {}

bool TankBullet::receiveMsg(const al::SensorMsg*, al::HitSensor* self, al::HitSensor* other) {}

void TankBullet::shoot(const sead::Vector3f&, const sead::Vector3f&, s32, bool, bool) {}

bool TankBullet::shootByPlayer(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&,
                               const sead::Vector3f&, f32, s32) {}

bool TankBullet::exeStart() {}

bool TankBullet::exeMove() {}

void TankBullet::disappear() {}

bool TankBullet::exeMovePlayer() {}

void TankBullet::exeExplode() {}

void TankBullet::exeYoshiEat() {}
