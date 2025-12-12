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
    void explode();
    void appear() override;
    bool receiveMsg(const al::SensorMsg*, al::HitSensor* self, al::HitSensor* other) override;
    void shoot(const sead::Vector3f&, const sead::Vector3f&, s32, bool, bool);
    void shootByPlayer(const sead::Vector3f&, const sead::Vector3f&, const sead::Vector3f&,
                       const sead::Vector3f&, f32, s32);
    void exeStart();
    void exeMove();
    void disappear();
    void exeMovePlayer();
    void exeExplode();
    void exeYoshiEat();

private:
    s32 mSensorRadius = -1;
    bool mIsShotByPlayer = false;
    sead::Vector3f mStartingPos = {0.0f, 0.0f, 0.0f};
    sead::Vector3f mVec2 = {
        0.0f, 0.0f, 0.0f};  // Waiting for TankStateHack to be decomplied so it can get a name.
    sead::Vector3f mVec3 = {
        0.0f, 0.0f, 0.0f};  // Waiting for TankStateHack to be decomplied so it can get a name.
    f32 mSpeed = 0.0f;
    bool mIsCapAttack;
    // bool mIsInvisible;
};

static_assert(sizeof(TankBullet) == 0x140);
