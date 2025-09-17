#include "Enemy/TankBullet.h"
#include "Library/LiveActor/LiveActor.h"


    TankBullet::TankBullet(char const* name) : al::LiveActor(name) {

    }

    
void TankBullet::init(al::ActorInitInfo const&) { }
void TankBullet::attackSensor(al::HitSensor*, al::HitSensor*) { }
bool TankBullet::explode() { }
void TankBullet::appear() { }
bool TankBullet::receiveMsg(al::SensorMsg const*, al::HitSensor*, al::HitSensor*) { }
void TankBullet::shoot(sead::Vector3<float> const&, sead::Vector3<float> const&, int, bool, bool) { }
bool TankBullet::shootByPlayer(sead::Vector3<float> const&, sead::Vector3<float> const&, sead::Vector3<float> const&, sead::Vector3<float> const&, float, int) { }
bool TankBullet::exeStart() { }
bool TankBullet::exeMove() { }
void TankBullet::disappear() { }
bool TankBullet::exeMovePlayer() { }
void TankBullet::exeExplode() { }
void TankBullet::exeYoshiEat() { }