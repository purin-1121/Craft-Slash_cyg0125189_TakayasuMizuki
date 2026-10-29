#pragma once

#include "Tools/JsonConverter.h"

enum class StateID
{
    Null = -1,
    Idle,
    Stun,
    Walk,
    Run,
    Jump,
    Chase,
    Attack,
    Defeat,
    Notice,
};

NLOHMANN_JSON_SERIALIZE_ENUM(StateID, {
    {StateID::Idle,   "Idle"},
    {StateID::Stun,   "Stun"},
    {StateID::Walk,   "Walk"},
    {StateID::Run,    "Run"},
    {StateID::Chase,  "Chase"},
    {StateID::Notice, "Notice"},
    {StateID::Attack, "Attack"},
    {StateID::Defeat, "Defeat"},
    })

