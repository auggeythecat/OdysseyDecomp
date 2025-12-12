#pragma once

#include "Library/LiveActor/LiveActor.h"

namespace al {
template <class T>
class DeriveActorGroup;
}  // namespace al

class Tank;

class TankReviveCtrl : public al::LiveActor {
public:
    TankReviveCtrl(const char* name);
    void init(const al::ActorInitInfo& info) override;
    void control() override;

private:
    al::DeriveActorGroup<Tank>* mActorGroup = nullptr;
};

static_assert(sizeof(TankReviveCtrl) == 0x110);
