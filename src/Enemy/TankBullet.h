#pragma once


#include "Library/LiveActor/LiveActor.h"

namespace al {
    class HitSensor;

}


class TankBullet : public al::LiveActor{
public:

    TankBullet(char const*);
    void init(al::ActorInitInfo const&) override;
    void attackSensor(al::HitSensor*, al::HitSensor*) override;
    bool explode();
    void appear() override;
    bool receiveMsg(al::SensorMsg const*, al::HitSensor*, al::HitSensor*) override;
    void shoot(sead::Vector3<float> const&, sead::Vector3<float> const&, int, bool, bool);
    bool shootByPlayer(sead::Vector3<float> const&, sead::Vector3<float> const&, sead::Vector3<float> const&, sead::Vector3<float> const&, float, int);
    bool exeStart();
    bool exeMove();
    void disappear();
    bool exeMovePlayer();
    void exeExplode();
    void exeYoshiEat();

    private:


};