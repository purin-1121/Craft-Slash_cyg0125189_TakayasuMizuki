
#pragma once

#include <iostream>

#include "Common/Collider/Collider.h"
#include "Tools/nlohmann/json.hpp"

struct BreakableObjectData
{
    std::string id = "-1";      // 識別番号
    std::string name = "None";  // オブジェクトの名前
    int maxHp = 0;              // 最大体力
    float mass = 0.0f;          // 質量       
    bool isKinematic = false;   // 動くかどうか
    ColliderData collider{};    // コライダー情報           
};

// 代入を簡略化するマクロ
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    BreakableObjectData, id, name, maxHp, mass, isKinematic,collider
)