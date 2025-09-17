#include "Enemy/TankReviveCtrl.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Placement/PlacementFunction.h"


TankReviveCtrl::TankReviveCtrl(char const* name) : al::LiveActor(name) {}

void TankReviveCtrl::init(al::ActorInitInfo const& info) {
    u32 childrenNum = al::calcLinkChildNum(info, "ControlTank");
    if (childrenNum < 1) {
        getCameraDirector(); // Literally no clue.
    } else {
        al::LiveActorGroup* actorGroup = new al::LiveActorGroup("制御タンクロー", childrenNum);
        
    }
}
void TankReviveCtrl::control() {}