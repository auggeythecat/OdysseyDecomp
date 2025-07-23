#pragma once

#include <basis/seadTypes.h>

#include "Library/LiveActor/LiveActor.h"
#include "math/seadQuat.h"

namespace al {
struct ActorInitInfo;
class HitSensor;
class SensorMsg;
class EnemyStateBlowDown;
class MtxConnector;
class LiveActorGroup;

}  // namespace al

class EnemyStateDamageCap;
class TankStateHack;
class EnemyStateReviveInsideScreen;
class EnemyStateSwoon;
class EnemyStateReset;
class IUseRail;

class Tank : public al::LiveActor {
public:
    Tank(const char* name);
    void init(const al::ActorInitInfo& initinfo) override;

    void enableShoot();
    void disableShoot();
    void initAfterPlacement() override;
    void appear() override;
    void isExistAndNearRail();
    void kill() override;
    void control() override;
    void die();
    void calcAnim() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    bool isMyBullet();
    // TankBullet shootByPlayer(); // I don't want to have to deal with TankBullet related errors
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

// Member variables (offsets based on Ghidra analysis)
    // Offset 0x40: IUseRail
    IUseRail* mIUseRail = nullptr;

    // Offset 0x108: al::LiveActorGroup for bullets
    al::LiveActorGroup* mBulletGroup = nullptr;
    
    // Offset 0x110: TankStateHack
    TankStateHack* mStateHack = nullptr;
    
    // Offset 0x118: EnemyStateDamageCap
    EnemyStateDamageCap* mStateDamageCap = nullptr;
    
    // Offset 0x120: EnemyStateReset
    EnemyStateReset* mStateReset = nullptr;
    
    // Offset 0x128: EnemyStateReviveInsideScreen (external control)
    EnemyStateReviveInsideScreen* mStateReviveExternal = nullptr;
    
    // Offset 0x130: EnemyStateReviveInsideScreen
    EnemyStateReviveInsideScreen* mStateRevive = nullptr;
    
    // Offset 0x138: EnemyStateSwoon
    EnemyStateSwoon* mStateSwoon = nullptr;
    
    // Offset 0x140: EnemyStateBlowDown
    al::EnemyStateBlowDown* mStateBlowDown = nullptr;
    
    // Offset 0x148: Matrix connector (undefined8)
    al::MtxConnector* mMtxConnector = nullptr;
    
    // Offset 0x150-0x158: Original quaternion
    sead::Quatf mOriginalQuat = {0.0, 0.0, 0.0, 0.0};
    
    // Offset 0x160: Cannon1 rotation (Z)
    f32 mCannon1RotZ = 0.0;
    
    // Offset 0x164: Cannon2 scale (X)
    f32 mCannon2ScaleX = 0.0;
    
    // Offset 0x168: Hip rotation (X)
    f32 mHipRotX = 0.0;
    
    // Offset 0x16c-0x174: Front direction vector
    sead::Vector3f mFrontDir = {0.0, 0.0, 0.0};
    
    // Offset 0x178: Right wheel rotation
    f32 mWheelRightRot = 0.0;
    
    // Offset 0x17c: Left wheel rotation
    f32 mWheelLeftRot = 0.0;
    
    // Offset 0x194-0x19c: Current quaternion
    sead::Quatf mCurrentQuat = {0.0, 0.0, 0.0, 0.0};
    
    // Offset 0x1a4: Cannon1 Y rotation
    f32 mCannon1RotY = 0.0;
    
    // Offset 0x1a8: Clipping radius
    f32 mClippingRadius = 0.0;
    
    // Offset 0x1ac: Shoot enabled flag
    bool mIsEnableShoot = true;
    
    // Offset 0x1ad: Shoot to camera flag
    bool mIsShootToCamera = false;
    
    // Offset 0x1ae: Moon world flag
    bool mIsMoonWorld = false;
    
    // Offset 0x193: Off collide at wait flag
    bool mIsOffCollideAtWait = false;

};