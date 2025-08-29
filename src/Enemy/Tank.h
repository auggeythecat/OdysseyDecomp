#pragma once

#include <basis/seadTypes.h>

#include "Library/LiveActor/LiveActor.h"
#include "Library/Rail/IUseRail.h"

namespace al {
struct ActorInitInfo;

class HitSensor;
class LiveActor;
class LiveActorGroup;
class MtxConnector;
class NerveStateBase;
class SensorMsg;
}  // namespace al

class EnemyStateReset;


class Tank : public al::LiveActor {
public:
    Tank(const char* name);
    void init(const al::ActorInitInfo& initinfo) override;

    void enableShoot();
    void disableShoot();
    void initAfterPlacement() override;
    void appear() override;
    bool isExistAndNearRail();
    void kill() override;
    void control() override;
    void die();
    void calcAnim() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    bool isMyBullet();
    // TankBullet shootByPlayer(); //I don't want to have to deal with TankBullet related errors
    void isSwoon();
    void appearCtrl();
    void preInitHandleByMofumofu();
    void appearAndDemoWait();
    void endDemoWait();
    void startShootByMofumofu();
    void startBlowDownByMofumofu(al::HitSensor*);
    void startRevive();
    void startRevivePrepare();
    void forceEndHackByMofumofu();
    void setSubjectiveCameraLimitDegree(f32 tmpname, f32 tmpname2);
    void setSubjectiveCameraAimFollowRateV(f32 tmpname);
    void isHacking();
    bool isRevivePrepare();
    bool isEnableStartAttack();
    void turn();
    bool isEnableShoot();

    void exeWait();
    void exeMove();
    void exeHack();
    void exeReset();
    void exeSwoon();
    void exeBlowDown();
    void exeReviveInsideScreenNoAutoRevive();
    void exeReviveInsideScreen();
    void exeDamageCap();
    void exePressDown();
    void exeAppear();
    void exeAttackSign();
    void exeShoot();
    void exeAttackHit();
    void exePressReaction();
    void exeDemoWait();

    s32 countAliveBullets();

private:
        
        al::IUseRail* mIUseRail = nullptr; // 0x40

        f32 mJointZRotate = 0.0;
        f32 mJointXScale =  0.0;
        f32 mJointXRotate = 0.0;
        f32 mWheelRoateR = 0.0;
        f32 mWheelRoateL = 0.0;
        
        f32 mQuat1x = 0.0;

        al::LiveActorGroup* mActorGroup = nullptr;

        sead::Vector3f mFrontDir = {0.0, 0.0, 0.0};
        EnemyStateReset* mEnemyStateReset = nullptr;
        
        bool mIsMoonCave = false;
        al::MtxConnector* mMtxConnector = nullptr; // 0x148

        al::NerveStateBase* mNerveStateBase = nullptr;



        f32 mCannonRotator = 0;

        bool mCanShoot = false; // 0x1ac

 
};
