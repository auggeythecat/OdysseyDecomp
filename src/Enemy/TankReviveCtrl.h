#pragma once

#include "Library/LiveActor/LiveActor.h"

class TankReviveCtrl : public al::LiveActor{
public:

    TankReviveCtrl(char const* name);
    void init(al::ActorInitInfo const& info) override;
    void control() override;



private:


};