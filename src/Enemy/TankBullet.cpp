#include "Enemy/TankBullet.h"

#include "Library/Collision/CollisionPartsKeeperUtil.h"
#include "Library/LiveActor/ActorActionFunction.h"
#include "Library/LiveActor/ActorClippingFunction.h"
#include "Library/LiveActor/ActorCollisionFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorMovementFunction.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorSensorUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"

#include "Enemy/Tank.h"
#include "Project/HitSensor/HitSensor.h"
#include "Util/SensorMsgFunction.h"
#include "math/seadVectorFwd.h"

namespace {
NERVE_IMPL(TankBullet, YoshiEat)
NERVE_IMPL(TankBullet, Start)
NERVE_IMPL(TankBullet, Explode)
NERVE_IMPL(TankBullet, MovePlayer)
NERVE_IMPL(TankBullet, Move)

NERVES_MAKE_STRUCT(TankBullet, Start, YoshiEat, Explode, MovePlayer);
NERVES_MAKE_NOSTRUCT(TankBullet, Move);
}  // namespace

TankBullet::TankBullet(const char* name) : al::LiveActor(name) {}

void TankBullet::init(const al::ActorInitInfo& info) {
    al::initActorWithArchiveName(this, info, "TankBullet", nullptr);
    al::initNerve(this, &NrvTankBullet.Start, 0);
    al::invalidateClipping(this);
    al::invalidateHitSensor(this, "Explosion");
    al::invalidateHitSensor(this, "ExplosionWide");
    makeActorDead();
}

void TankBullet::attackSensor(al::HitSensor* self, al::HitSensor* other) {
    if ((al::isNerve(this, &NrvTankBullet.Start) || (al::isNerve(this, &NrvTankBullet.YoshiEat))) ||
        (mIsShotByPlayer != false && (((!al::isSensorEye(self) && (al::isSensorMapObj(other))) &&
                                       rs::sendMsgWeaponItemGet(other, self))))) {
        return;
    }

    if (al::isNerve(this, &NrvTankBullet.Explode)) {
        if (!al::isSensorEye(self))
            return;

        if (al::isSensorName(self, "Explosion")) {
            if ((mIsCapAttack && al::isSensorPlayer(other)) || al::sendMsgExplosion(other, self, nullptr))
                return;
        }
        if (!mIsShotByPlayer)
            return;

        if (!al::isSensorName(self, "ExplosionWide"))
            return;
        rs::sendMsgTankExplosion(other, self);
        return;
    }
    // al::LiveActor* pLVar2;

    if (!mIsShotByPlayer) {
        if (al::sendMsgExplosion(other, self, nullptr)) {
            explode();
        }
        return;
    }

    if (rs::sendMsgTankBulletNoReaction(other, self)) {
        kill();
        return;
    }

    if (!rs::sendMsgTankBullet(other, self) && !al::sendMsgExplosion(other, self, nullptr))
        return;

    al::LiveActor* pLVar2 = al::getSensorHost(other);

    if (al::isValidCollisionParts(pLVar2) && al::isExistCollisionParts(pLVar2) && pLVar2) {
        al::startHitReaction(this, "キャプチャショット壁あたり");
        explode();
        return;
    }
    al::startHitReaction(this, "キャプチャショットオブジェあたり");
    explode();
    return;
}

void TankBullet::explode() {
    al::startAction(this, "Explode");
    al::startHitReaction(this, "爆発");
    al::setVelocityZero(this);
    al::validateHitSensor(this, "Explosion");
    al::validateHitSensor(this, "ExplosionWide");
    al::setNerve(this, &NrvTankBullet.Explode);
}

bool TankBullet::receiveMsg(const al::SensorMsg* message, al::HitSensor* self,
                            al::HitSensor* other) { 
    if(rs::isMsgPlayerDisregardTargetMarker(message))
        return true;

    if(al::isNerve(this, &NrvTankBullet.Explode))
        return false;

    if (rs::isMsgYoshiTongueEatBind(message) && !al::isNerve(this, &NrvTankBullet.YoshiEat)) {
        rs::setMsgYoshiTongueEatBindRadiusAndOffset(message, 0.0f, 0.0f);
        rs::requestHitReactionToAttacker(message, other, self);
        al::setNerve(this, &NrvTankBullet.YoshiEat);
        return true;
    }

    if (rs::isMsgYoshiTongueEatBindFinish(message)) {
        rs::requestHitReactionToAttacker(message, other, self);
        kill();
        return true;
    }

    if (rs::isMsgYoshiTongueEatBindCancel(message)) {
        kill();
        return true;
    }

    if (al::isNerve(this, &NrvTankBullet.YoshiEat))
        return false;

    if (rs::isMsgBlowDown(message) || rs::isMsgCapAttack(message)) {
        
        if(mIsShotByPlayer && rs::isMsgCapAttack(message))
            return false;
        rs::requestHitReactionToAttacker(message, other, self);
        
        if (rs::isMsgCapAttack(message))
            mIsCapAttack = true;

        if (!rs::isMsgMotorcycleDashAttack(message)) {
            explode();
            return true;
        }
    }
    return false;
}

void TankBullet::shoot(const sead::Vector3f& pos, const sead::Vector3f& velo, s32 sensorRadius,
                       bool isShotByPlayer, bool unk3) {
    mIsShotByPlayer = isShotByPlayer;
    al::setTrans(this, pos);
    al::setVelocity(this, velo);
    mSensorRadius = sensorRadius;
    al::setSensorRadius(this, "Explosion", 0.0f);
    al::startAction(this, "Appear");
    sead::Vector3f normalizedVelo = velo;
    al::tryNormalizeOrZero(&normalizedVelo);
    al::setFront(this, normalizedVelo);
    al::setNerve(this, &NrvTankBullet.Start);
    if (isShotByPlayer) {
        al::validateHitSensor(this, "Explosion");
        al::invalidateHitSensor(this, "Attack");
    } else {
        al::invalidateHitSensor(this, "Explosion");
        al::validateHitSensor(this, "Attack");
    }
    appear();
}

void TankBullet::shootByPlayer(const sead::Vector3f& vec1, const sead::Vector3f& startingPos,
                               const sead::Vector3f& vec2, const sead::Vector3f& vec3, f32 speed,
                               s32 sensorRadius) {
    mIsShotByPlayer = true;
    mStartingPos.set(startingPos);
    mVec2.set(vec2);
    mVec3.set(vec3);
    mSpeed = speed;
    mSensorRadius = sensorRadius;

    al::setTrans(this, startingPos);

    sead::Vector3f facingdir = vec2 - startingPos;
    al::normalize(&facingdir);
    al::setFront(this, facingdir);

    al::setVelocity(this, facingdir * speed);
    al::validateHitSensor(this, "Body");
    al::invalidateHitSensor(this, "Attack");

    sead::Vector3f collisionPos = {0.0, 0.0, 0.0};
    al::HitSensor* hitSensor = nullptr;
    if (alCollisionUtil::getFirstCollisionSensorOnArrow(this, &collisionPos, &hitSensor, vec1,
                                                        (startingPos - vec1), nullptr, nullptr)) {
        sead::Vector3f transition = collisionPos - vec1;
        al::tryNormalizeOrDirZ(&transition);
        al::setTrans(this, collisionPos - transition * 50);
        al::setVelocityZero(this);
        appear();
        if (hitSensor)
            al::sendMsgExplosion(hitSensor, al::getHitSensor(this, "Explosion"), nullptr);
        explode();
        return;
    }
    al::setNerve(this, &NrvTankBullet.MovePlayer);
    appear();
    return;
}

void TankBullet::exeStart() {
    if (al::isGreaterStep(this, -1))
        al::setNerve(this, &NrvTankBullet.Start);
}

void TankBullet::exeMove() {
    if (al::isFirstStep(this))
        al::startAction(this, "Move");

    if (al::isGreaterEqualStep(this, mSensorRadius - 5)) {
        s32 sensorRadius = mSensorRadius;
        s32 step = al::getNerveStep(this);
        f64 scale = (sensorRadius - step) / 5.0f;
        al::setScaleAll(this, scale);
    }

    if (al::isGreaterEqualStep(this, mSensorRadius)) {
        kill();
        return;
    }
    if (!al::isCollidedWall(this) && !al::isCollidedGround(this))
        return;

    al::HitSensor* wallcollide = al::tryGetCollidedWallSensor(this);
    if (wallcollide)
        al::sendMsgExplosion(wallcollide, al::getHitSensor(this, "Explosion"), nullptr);

    al::HitSensor* groundcollide = al::tryGetCollidedGroundSensor(this);
    if (groundcollide)
        al::sendMsgExplosion(groundcollide, al::getHitSensor(this, "Explosion"), nullptr);

    explode();
}

void TankBullet::disappear() {
    kill();
}

/*
void TankBullet::exeMovePlayer() {
    sead::Vector3f velocity;
    sead::Vector3f vec2;

    if (al::isFirstStep(this)) {
        al::startAction(this, "Appear");
        al::setSensorRadius(this, "Explosion", 0.0f);
    }
    if (al::isStep(this, 2))
        al::startAction(this, "HackMove");

    if (al::isGreaterEqualStep(this, mSensorRadius - 5)) {
        f32 value = (mSensorRadius - al::getNerveStep(this)) / 5.0f;
        al::setScaleAll(this, value);
    }

    if (al::isGreaterEqualStep(this, mSensorRadius)) {
        kill();
        return;
    }

    if (!al::isCollided(this) && !al::isInWater(this)) {
        vec2 = mStartingPos - mVec2;
        f32 vec2length = vec2.length();
        f32 unusedfloat = al::getNerveStep(this) * mSpeed;

        if (!(unusedfloat < vec2length)) {
            f32 rate = al::easeOut(al::normalize(unusedfloat, 0.0f, vec2length));
            sead::Vector3f vec = mVec2 - mStartingPos;
            // vec.normalize();
            sead::Vector3f front = {0.0f, 0.0f, 0.0f};

            al::turnVecToVecRate(&front, vec, mVec3, rate);
            front.normalize();
            al::setFront(this, front);

            f32 multiplier = vec2length - unusedfloat;
            sead::Vector3f local_70 = unusedfloat * vec + mStartingPos;
            sead::Vector3f local_80 = mVec2 - multiplier * mVec3;
            sead::Vector3f result = {0.0f, 0.0f, 0.0f};
            al::lerpVec(&result, local_70, local_80, rate);
            sead::Vector3f trans = al::getTrans(this);
            ;
            velocity = (result - trans);
        } else {
            // sead::Vector3f trans = al::getTrans(this);
            // sead::Vector3f vec3 = mVec3;
            // sead::Vector3f pos = mStartingPos;
            f32 somevalue = unusedfloat - vec2length;
            velocity = (somevalue * mVec3) + mVec2;
        }
        al::setVelocity(this, velocity);
        return;
    }
    if (mIsShotByPlayer) {
        al::startHitReaction(this, "キャプチャショット壁あたり");
    }
    al::HitSensor* hitsensor;
    if (al::isCollidedGround(this)) {
        hitsensor = al::tryGetCollidedGroundSensor(this);
    } else if (al::isCollidedWall(this)) {
        hitsensor = al::tryGetCollidedWallSensor(this);
    } else {
        if (!al::isCollidedCeiling(this)) {
            explode();
            return;
        }
        hitsensor = al::tryGetCollidedCeilingSensor(this);
    }
    if (hitsensor != nullptr)
        al::sendMsgExplosion(hitsensor, al::getHitSensor(this, "Explosion"), nullptr);

    explode();
}
*/

void TankBullet::exeMovePlayer() {
      
    if (al::isFirstStep(this)) {
        al::startAction(this, "Appear");
        al::setSensorRadius(this, "Explosion", 0.0f);
    }
    
    if (al::isStep(this, 2))
        al::startAction(this, "HackMove");
    
    if (al::isGreaterEqualStep(this, mSensorRadius - 5)) 
        al::setScaleAll(this, (mSensorRadius - al::getNerveStep(this)) / 5.0f);
    
    
    if (al::isGreaterEqualStep(this, mSensorRadius)) {
        kill();
        return;
    }
      
    if (!al::isCollided(this) && !al::isInWater(this)) {
        sead::Vector3f velo;
        sead::Vector3f vec = mStartingPos - mVec2;
        f32 distance = al::getNerveStep(this) * mSpeed;
        f32 vecLength = vec.length();
        if (distance <= vecLength) {
            f32 normalize = al::normalize(distance, 0.0f, vecLength);
            f32 rate = al::easeOut(normalize);
            sead::Vector3f vec = mVec2 - mStartingPos;
            al::normalize(&vec);
            
            sead::Vector3f turn = {0.0f, 0.0f, 0.0f};
            al::turnVecToVecRate(&turn, vec, mVec3, rate);
            al::normalize(&turn);
            al::setFront(this, turn);

            f32 multiplier = vecLength - distance;
            sead::Vector3f local_70 = distance * vec + mStartingPos;
            sead::Vector3f local_80 = mVec2 - multiplier * mVec3;
            sead::Vector3f result = {0.0f, 0.0f, 0.0f};

            al::lerpVec(&result, local_70, local_80, rate);
            sead::Vector3f trans = al::getTrans(this);
            velo = result - trans;
        } else {
            f32 multiplier = vecLength - distance;
            sead::Vector3f trans = al::getTrans(this);
            velo = (multiplier * mVec2 + mVec3) - trans;
        }
        al::setVelocity(this, velo);
        return;
    }


    if (mIsShotByPlayer) {
        al::startHitReaction(this, "キャプチャショット壁あたり");
    }
    al::HitSensor* hitsensor;
    if (al::isCollidedGround(this)) {
        hitsensor = al::tryGetCollidedGroundSensor(this);
    } else if (al::isCollidedWall(this)) {
        hitsensor = al::tryGetCollidedWallSensor(this);
    } else {
        if (!al::isCollidedCeiling(this)) {
            explode();
            return;
        }
        hitsensor = al::tryGetCollidedCeilingSensor(this);
    }
    if (hitsensor != nullptr)
        al::sendMsgExplosion(hitsensor, al::getHitSensor(this, "Explosion"), nullptr);

    explode();
}

void TankBullet::exeExplode() {
    al::setSensorRadius(this, "Explosion", al::calcNerveRate(this, 5) * 100.0);
    al::setSensorRadius(this, "ExplosionWide", al::calcNerveRate(this, 5) * 500.0);
    if (al::isGreaterEqualStep(this, 5)) {
        al::tryAddRippleLarge(this);
        al::setSensorRadius(this, "Explosion", 0.0);
        al::setSensorRadius(this, "ExplosionWide", 0.0);
        al::invalidateHitSensor(this, "Explosion");
        al::invalidateHitSensor(this, "ExplosionWide");
        kill();
    }
}

void TankBullet::exeYoshiEat() {}
