#include "Enemy/TankReviveCtrl.h"

#include "Library/LiveActor/ActorFlagFunction.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Placement/PlacementFunction.h"

#include "Enemy/Tank.h"
#include "Util/PlayerUtil.h"

TankReviveCtrl::TankReviveCtrl(const char* name) : al::LiveActor(name) {}

void TankReviveCtrl::init(const al::ActorInitInfo& info) {
    al::initActor(this, info);
    s32 childCount = al::calcLinkChildNum(info, "ControlTank");

    if (childCount <= 0) {
        makeActorDead();
        return;
    }

    mActorGroup = new al::DeriveActorGroup<Tank>("制御タンクロー", childCount);

    for (s32 i = 0; i < childCount; i++) {
        Tank* tank = new Tank("タンクロー");
        al::initLinksActor(tank, info, "ControlTank", i);
        tank->makeActorAlive();
        mActorGroup->registerActor(tank);
    }

    makeActorAlive();
    return;
}

void TankReviveCtrl::control() {
    if (rs::isPlayerHackTank(this))
        return;

    bool isAnySwoon = false;
    for (s32 i = 0; i < mActorGroup->getActorCount(); i++)
        isAnySwoon |= mActorGroup->getDeriveActor(i)->isSwoon();

    if (isAnySwoon || !rs::isPlayerOnGround(this))
        return;

    sead::Vector3f playerPos = rs::getPlayerPos(this);
    Tank* tank = nullptr;

    f32 minDistance = 1000000.0f;
    for (s32 i = 0; i < mActorGroup->getActorCount(); i++) {
        sead::Vector3f trans = al::getTrans(mActorGroup->getDeriveActor(i));

        f32 distance = sead::Mathf::abs(trans.y - playerPos.y);

        if (trans.y < playerPos.y + 200.0f && distance < minDistance) {
            tank = mActorGroup->getDeriveActor(i);
            minDistance = distance;
        }
    }

    if (tank) {
        if (al::isAlive(tank)) {
            if (tank->isRevivePrepare())
                tank->startRevive();
        } else {
            tank->appearCtrl();
        }

        for (s32 i = 0; i < mActorGroup->getActorCount(); i++) {
            Tank* newTank = mActorGroup->getDeriveActor(i);
            if (newTank != tank && al::isAlive(newTank))
                newTank->kill();
        }
    }
}
