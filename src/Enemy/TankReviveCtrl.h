#pragma once

#include "Library/LiveActor/LiveActor.h"

class TankReviveCtrl : public al::LiveActor {
public:
    TankReviveCtrl(const char* name);
    void init(const al::ActorInitInfo& info) override;
    void control() override;

private:
};
