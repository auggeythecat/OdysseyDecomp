#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
class HitSensor;

}

class TankBullet : public al::LiveActor {
public:
    TankBullet(const char*);
    void init(const al::ActorInitInfo&) override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool explode();
    void appear() override;
    bool receiveMsg(const al::SensorMsg*, al::HitSensor* self, al::HitSensor* other) override;
    void shoot(const sead::Vector3f&, const sead::Vector3f&, s32, bool, bool);
    bool shootByPlayer(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&,
                       const sead::Vector3f&, f32, s32);
    bool exeStart();
    bool exeMove();
    void disappear();
    bool exeMovePlayer();
    void exeExplode();
    void exeYoshiEat();

private:
};
