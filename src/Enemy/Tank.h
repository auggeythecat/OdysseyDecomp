#pragma once

#include "Enemy/EnemyStateReviveInsideScreen.h"
#include <basis/seadTypes.h>

#include "Enemy/TankStateHack.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Movement/EnemyStateBlowDown.h"
#include "Library/Rail/IUseRail.h"
#include "Library/Scene/IUseSceneObjHolder.h"
#include "math/seadQuat.h"
#include "math/seadVectorFwd.h"

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
    al::IUseStageSwitch* mIUseStageSwitch = nullptr;  // 0x18
    al::IUseSceneObjHolder* mIUseSceneObj = nullptr;  // 0x20
    void* spacefiller1 = nullptr;
    al::IUseRail* mIUseRail = nullptr;                // 0x40

    al::LiveActorGroup* mActorGroup = nullptr;  // 0x108
    TankStateHack* mTankStateHack = nullptr;    // 0x110

    EnemyStateReviveInsideScreen* mEnemyStateReviveInsideScreen = nullptr; // 0x128
    EnemyStateReset* mEnemyStateReset = nullptr;

    al::MtxConnector* mMtxConnector = nullptr;              // 0x148
    al::EnemyStateBlowDown* mEnemyStateBlowDown = nullptr;  // 0x140
    bool mIsMoonCave = false;

    f32 mJointZRotate = 0.0;  // 0x160
    f32 mJointXScale = 0.0;   // 0x164
    f32 mJointXRotate = 0.0;  // 0x168

    sead::Vector3f mFrontDir = {0.0, 0.0, 0.0}; // 0x16c

    f32 mWheelRoateR = 0.0;  // 0x178
    f32 mWheelRoateL = 0.0;  // 0x17c

    bool mIsHandled = false; // 0x190
    sead::Quatf mQuat = {0.0, 0.0, 0.0, 0.0}; // 0x194

    al::NerveStateBase* mNerveStateBase = nullptr;

    f32 mCannonRotator = 0.0;  // 0x1a4
    
    bool filler1[8];
    // void* filler[0x1];
    bool mCanShoot = false;  // 0x1ac
};
